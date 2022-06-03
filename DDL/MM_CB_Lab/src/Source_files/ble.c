/**
 * @file ble.c
 * @author Keith Graham - Updated Malcolm McKellips (3/29)
 * @date November 28th, 2019
 * @brief Contains all the functions to interface the application with the HM-18
 *   BLE module and the LEUART driver
 *
 */


//***********************************************************************************
// Include files
//***********************************************************************************
#include "ble.h"
#include <string.h>

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// private variables
//***********************************************************************************
CIRC_TEST_STRUCT test_struct; //added in lab 6. (might need to be static)
BLE_CIRCULAR_BUF ble_cbuf;
//char cbuf[64]; //CHECK, MIGHT WANT * like in lab doc???
/***************************************************************************//**
 * @brief BLE module
 * @details
 *  This module contains all the functions to interface the application layer
 *  with the HM-18 Bluetooth module.  The application does not have the
 *  responsibility of knowing the physical resources required, how to
 *  configure, or interface to the Bluetooth resource including the LEUART
 *  driver that communicates with the HM-18 BLE module.
 *
 ******************************************************************************/

//***********************************************************************************
// Private functions
//***********************************************************************************
static uint8_t ble_circ_space(void);
static void update_circ_wrtindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by);
static void update_circ_readindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by);

//***********************************************************************************
// Global functions
//***********************************************************************************


/***************************************************************************//**
 * @brief
 * 	This function will format a configuration structure to open a LEUART peripheral for BLE operation
 * @description
 * 	The funciton will save the events passed in to it as well as the transmission details from ble.h
 * 	to format a struct which will then be passed to leuart_open for leuart operation with the ble module
 * @param[in] tx_event
 * 	The transmit event to be saved
 * @param[in] string
 * 	The receive event to be saved
  ******************************************************************************/

void ble_open(uint32_t tx_event, uint32_t rx_event){
	LEUART_OPEN_STRUCT ble_leuart_open_struct;

	//configure transmission details
	ble_leuart_open_struct.baudrate = HM10_BAUDRATE;
	ble_leuart_open_struct.databits = HM10_DATABITS;
	ble_leuart_open_struct.enable 	= HM10_ENABLE;
	ble_leuart_open_struct.parity 	= HM10_PARITY;
	ble_leuart_open_struct.refFreq 	= HM10_REFFREQ;
	ble_leuart_open_struct.stopbits = HM10_STOPBITS;

	//Configure routing information
	ble_leuart_open_struct.tx_loc = LEUART0_TX_ROUTE;
	ble_leuart_open_struct.rx_loc = LEUART0_RX_ROUTE;

	//Configure the enable signals
	ble_leuart_open_struct.tx_pin_en = true;
	ble_leuart_open_struct.rx_pin_en = true;
	ble_leuart_open_struct.tx_en 	 = true;
	ble_leuart_open_struct.rx_en 	 = true;

	//Configure the scheduler events
	ble_leuart_open_struct.tx_done_evt = tx_event;
	ble_leuart_open_struct.rx_done_evt = rx_event;

	leuart_open(HM10_LEUART0,&ble_leuart_open_struct);
	ble_circ_init(); //added lab 6
}

/***************************************************************************//**
 * @brief
 * 	Function to write a string to the LEUART
 * @description
 *  Start an leuart tx operation using the input string
 * @param[in] string
 *	The input string to be transmitted
 *    ******************************************************************************/

void ble_write(char* string){
	//leuart_start(HM10_LEUART0, string, strlen(string)); //old (LAB 5)
	ble_circ_push(string); //add string to circular buffer
	ble_circ_pop(false);   //Print the string over leuart
}

/***************************************************************************//**
 * @brief
 *   BLE Test performs two functions.  First, it is a Test Driven Development
 *   routine to verify that the LEUART is correctly configured to communicate
 *   with the BLE HM-18 module.  Second, the input argument passed to this
 *   function will be written into the BLE module and become the new name
 *   advertised by the module while it is looking to pair.
 *
 * @details
 * 	 This global function will use polling functions provided by the LEUART
 * 	 driver for both transmit and receive to validate communications with
 * 	 the HM-18 BLE module.  For the assignment, the communication with the
 * 	 BLE module must use low energy design principles of being an interrupt
 * 	 driven state machine.
 *
 * @note
 *   For this test to run to completion, the phone most not be paired with
 *   the BLE module.  In addition for the name to be stored into the module
 *   a breakpoint must be placed at the end of the test routine and stopped
 *   at this breakpoint while in the debugger for a minimum of 5 seconds.
 *
 * @param[in] *mod_name
 *   The name that will be written to the HM-18 BLE module to identify it
 *   while it is advertising over Bluetooth Low Energy.
 *
 * @return
 *   Returns bool true if successfully passed through the tests in this
 *   function.
 ******************************************************************************/

bool ble_test(char *mod_name){
	uint32_t	str_len;

	__disable_irq();
	// This test will limit the test to the proper setup of the LEUART
	// peripheral, routing of the signals to the proper pins, pin
	// configuration, and transmit/reception verification.  The test
	// will communicate with the BLE module using polling routines
	// instead of interrupts.
	// How is polling different than using interrupts?
	// ANSWER: Polling is busy waiting, it takes up the CPU's time and energy whereas
	//			interrupts allow the CPU to do other things until an asynchronous interrupt tells it to do
	//			do something outside of its regular synchronous operation.
	// How does interrupts benefit the system for low energy operation?
	// ANSWER: Interrupts allow the CPU to enter sleep when it is only running background tasks and then wake up
	//			when an interrupt forces it to perform a task with higher energy requirements.
	// How does interrupts benefit the system that has multiple tasks?
	// ANSWER: Instead of doing nothing during a polling busy wait, interrupts allow the cpu to be
	//			getting another task done while one task is not executing. In a system with multiple tasks,
	//			this helps create a higher throughput of work accomplished by the system.

	// First, you will need to review the DSD HM10 datasheet to determine
	// what the default strings to write data to the BLE module and the
	// expected return statement from the BLE module to test / verify the
	// correct response

	// The break_str is used to tell the BLE module to end a Bluetooth connection
	// such as with your phone.  The ok_str is the result sent from the BLE module
	// to the micro-controller if there was not active BLE connection at the time
	// the break command was sent to the BLE module.
	// Replace the break_str "" with the command to break or end a BLE connection
	// Replace the ok_str "" with the result that will be returned from the BLE
	//   module if there was no BLE connection
	char		break_str[80] = "AT";
	char		ok_str[80] = "OK";

	// output_str will be the string that will program a name to the BLE module.
	// From the DSD HM10 datasheet, what is the command to program a name into
	// the BLE module?
	// The  output_str will be a string concatenation of the DSD HM10 command
	// and the input argument sent to the ble_test() function
	// Replace the otput_str "" with the command to change the program name
	// Replace the result_str "" with the first part of the expected result
	//  the backend of the expected response will be concatenated with the
	//  input argument
	char		output_str[80] = "AT+NAME";
	char		result_str[80] = "OK+Set:"; //Check this. It might be OK+NAME or OK+SetName

	// To program the name into your module, you must reset the module after you
	// have sent the command to update the modules name.  What is the DSD HM10
	// name to reset the module?
	// Replace the reset_str "" with the command to reset the module
	// Replace the reset_result_str "" with the expected BLE module response to
	//  to the reset command
	char		reset_str[80] = "AT+RESET";
	char		reset_result_str[80] = "OK+RESET";
	char		return_str[80];

	bool		success;
	bool		rx_disabled, rx_en, tx_en;
	uint32_t	status;

	// These are the routines that will build up the entire command and response
	// of programming the name into the BLE module.  Concatenating the command or
	// response with the input argument name
	strcat(output_str, mod_name);
	strcat(result_str, mod_name);

	// The test routine must not alter the function of the configuration of the
	// LEUART driver, but requires certain functionality to insure the logical test
	// of writing and reading to the DSD HM10 module.  The following c-lines of code
	// save the current state of the LEUART driver that will be used later to
	// re-instate the LEUART configuration

	status = leuart_status(HM10_LEUART0);
	if (status & LEUART_STATUS_RXBLOCK) {
		rx_disabled = true;
		// Enabling, unblocking, the receiving of data from the LEUART RX port
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXBLOCKDIS);
	}
	else rx_disabled = false;
	if (status & LEUART_STATUS_RXENS) {
		rx_en = true;
	} else {
		rx_en = false;
		// Enabling the receiving of data from the RX port
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXEN);
		while (!(leuart_status(HM10_LEUART0) & LEUART_STATUS_RXENS));
	}

	if (status & LEUART_STATUS_TXENS){
		tx_en = true;
	} else {
		// Enabling the transmission of data to the TX port
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_TXEN);
		while (!(leuart_status(HM10_LEUART0) & LEUART_STATUS_TXENS));
		tx_en = false;
	}
//	leuart_cmd_write(HM10_LEUART0, (LEUART_CMD_CLEARRX | LEUART_CMD_CLEARTX));

	// This sequence of instructions is sending the break ble connection
	// to the DSD HM10 module.
	// Why is this command required if you want to change the name of the
	// DSD HM10 module?
	// ANSWER: In order to configure the HM10 module, it cannot currently be doing another task like
	//			being connected to our phone. It is similar to the analogy in the lab doc of switching gears while driving.
	str_len = strlen(break_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM10_LEUART0, break_str[i]);
	}

	// What will the ble module response back to this command if there is
	// a current ble connection?
	// ANSWER:OK+LOST (assuming that AT+NOTI has been set to 1)
	str_len = strlen(ok_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (ok_str[i] != return_str[i]) {
				EFM_ASSERT(false);;
		}
	}

	// This sequence of code will be writing or programming the name of
	// the module to the DSD HM10
	str_len = strlen(output_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM10_LEUART0, output_str[i]);
	}

	// Here will be the check on the response back from the DSD HM10 on the
	// programming of its name
	str_len = strlen(result_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (result_str[i] != return_str[i]) {
				EFM_ASSERT(false);;
		}
	}

	// It is now time to send the command to RESET the DSD HM10 module
	str_len = strlen(reset_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM10_LEUART0, reset_str[i]);
	}

	// After sending the command to RESET, the DSD HM10 will send a response
	// back to the micro-controller
	str_len = strlen(reset_result_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (reset_result_str[i] != return_str[i]) {
				EFM_ASSERT(false);;
		}
	}

	// After the test and programming have been completed, the original
	// state of the LEUART must be restored
	if (!rx_en) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXDIS);
	if (rx_disabled) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXBLOCKEN);
	if (!tx_en) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_TXDIS);
	leuart_if_reset(HM10_LEUART0);

	success = true;

	__enable_irq();
	return success;
}


//------------------------------------------LAB 6----------------------------------------
//New ble function for Lab 6
/***************************************************************************//**
 * @brief
 *   Test driven development test for operation of a circular buffer
 *
 * @details
 * 	 This function will initialize several test strings and then push and pop them to
 * 	 and from a circular buffer, and check success after each operation. If each of the tests
 * 	 passes, the function will print that it has succeeded over bluetooth.
 *
 ******************************************************************************/
void circular_buff_test(void){
	 bool buff_empty;
	 int test1_len = 50;
	 int test2_len = 25;
	 int test3_len = 5;

	 // Why this 0 initialize of read and write pointer?
	 // Student Response: Initially, the buffer is empty, so both the read and the write will be pointing to the same location
	 // 				  which will be at the beginning of the buffer.
	 //
	 ble_cbuf.read_ptr = 0;
	 ble_cbuf.write_ptr = 0;

	 // Why do none of these test strings contain a 0?
	 // Student Response: The zero character could be misinterpreted later in strcpy and other string functions as a null terminating character, so we
	 //					  avoid it in our test strings.
	 for (int i = 0;i < test1_len; i++){
		 test_struct.test_str[0][i] = i+1;
	 }
	 for (int i = 0;i < test2_len; i++){
		 test_struct.test_str[1][i] = i + 20;
	 }
	 for (int i = 0;i < test3_len; i++){
		 test_struct.test_str[2][i] = i +  35;
	 }

	 // Why is there only one push to the circular buffer at this stage of the test
	 // Student Response: Test driven development tests should be designed to be incremental and test only small parts of a system at a time
	 //					  in order to to see exactly where in the system a failure is.
	 //
	 ble_circ_push(&test_struct.test_str[0][0]);

	 // Why is the expected buff_empty test = false?
	 // Student Response: We performed a push before this pop, so the buffer should not be empty as we have added to it.
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test1_len; i++){
		 EFM_ASSERT(test_struct.test_str[0][i] == test_struct.result_str[i]);
	 }

	 // What does this next push on the circular buffer test?
	 // Student Response: Writing to the buffer when it is not in its initializaed state, i.e. it is
	 //					  empty but the read and write pointers have been moved from 0 by a previous write.

	 ble_circ_push(&test_struct.test_str[1][0]);

	 // What does this next push on the circular buffer test?
	 // Student Response: Writing to the buffer when there is already a string present in the buffer. Adding multiple strings at once.
	 ble_circ_push(&test_struct.test_str[2][0]);


	 // Why is the expected buff_empty test = false?
	 // Student Response: There are two strings in the buffer that we have just pushed on. It is not empty
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test2_len; i++){
		 EFM_ASSERT(test_struct.test_str[1][i] == test_struct.result_str[i]);
	 }

	 // Why is the expected buff_empty test = false?
	 // Student Response: We will still have one string left in our buffer so we still have one string to return on this pop.
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test3_len; i++){
		 EFM_ASSERT(test_struct.test_str[2][i] == test_struct.result_str[i]);
	 }

	 // Using these three writes and pops to the circular buffer, what other test
	 // could we develop to better test out the circular buffer?
	 // Student Response: We could do writes and reads to better test the edge cases of our buffer. If we put strings 1 and 2 in
	 // 				  the buffer at once we could test that it performs properly on an overflow.


	 // Why is the expected buff_empty test = true?
	 // Student Response: We have popped all of the strings out of the buffer so it should be empty when we attempt this pop.
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == true);
	 ble_write("\nPassed Circular Buffer Test\n");

}

/***************************************************************************//**
 * @brief
 *	Calculate and return the amount of space left in the buffer to write
 *
 * @details
 *	Will calculate the current amount of data and subtract this from the buffer size -1.
 *
 * @return
 * 	 Returns the current amount of space left in circular buffer
 *
 ******************************************************************************/
static uint8_t ble_circ_space(void){
	 //free space = size - length
	//NOTE: might want it to be size - 1 instead of size -> (size_mask)
	 return ble_cbuf.size_mask - ((ble_cbuf.write_ptr - ble_cbuf.read_ptr) & ble_cbuf.size_mask);
}

/***************************************************************************//**
 * @brief
 *	Function for initializing a circular buffer
 *
 * @details
 *	The private circular buffer struct will be loaded with all of the necessary
 *	fields. The mask is CSIZE - 1 for proper &ing later as per the textbook.
 *
 *
 ******************************************************************************/
void ble_circ_init(void){
	ble_cbuf.size      = CSIZE;
	ble_cbuf.size_mask = CSIZE - 1;
	ble_cbuf.read_ptr  = 0;
	ble_cbuf.write_ptr = 0;
}


/***************************************************************************//**
 * @brief
 *   Function for adding a string to a circular buffer.
 *
 * @details
 * 	 The string input will first be checked to see if there is space for it in the circular buffer.
 * 	 Then, if there is space, the string will be added to the buffer and the write pointer will
 * 	 be updated accordingly.
 *
 *
 * @param[in] *string
 *   The string to be pushed onto the circular buffer.
 ******************************************************************************/
void ble_circ_push(char *string){
	//The length of the packet is the length of the string + 1 (for the header info)

	//there isn't space to add the packet.
	if (ble_circ_space() < strlen(string) + 1){
		EFM_ASSERT(false);
	}
	else {
		//Set the header information (length of string + 1) and update the write pointer
		ble_cbuf.cbuf[ble_cbuf.write_ptr] = strlen(string) + 1;
		update_circ_wrtindex(&ble_cbuf,1);
		//add the rest of the packet (the string)
		for (int i = 0; i < strlen(string); i++){
			ble_cbuf.cbuf[ble_cbuf.write_ptr] = string[i];
			update_circ_wrtindex(&ble_cbuf,1);
		}
		//update the write pointer to the end of the data we have just written (move it forward by the size of the packet)
		//update_circ_wrtindex(&ble_cbuf,strlen(string)+1);
	}
}



/***************************************************************************//**
 * @brief
 *   Function for printing and removing a string from a circular buffer.
 *
 * @details
 * 	 Remove string from the buffer in FIFO manner. If testing is enabled, load the
 * 	 test string with this removed string. If testing is not enabled, send this string over
 * 	 leuart to be printed by the BLE module. Update the read pointer accordingly.
 *
 *
 * @param[in] test
 *   A boolean value for whether we are operating in test mode or normal mode.
 *
 * @return
 * 	Whether or not there is a string to be popped off. i.e. return if the buffer is empty at function call time.
 ******************************************************************************/
bool ble_circ_pop(bool test){
	//Make sure that the buffer isn't empty
	if (ble_cbuf.write_ptr == ble_cbuf.read_ptr) return true;

	//Make sure that leuart isn't busy
	if (leuart_tx_busy(HM10_LEUART0)) return false;

	//Extract the header
	uint32_t string_length;
	string_length = ble_cbuf.cbuf[ble_cbuf.read_ptr] - 1;
	update_circ_readindex(&ble_cbuf,1);

	//Extract the string
	char print_str[string_length + 1]; //Length of the string (+1 for the null character b/c I use strcpy in leuart)
	for (int i = 0; i < string_length; i++){
		print_str[i] = ble_cbuf.cbuf[ble_cbuf.read_ptr];
		update_circ_readindex(&ble_cbuf,1);
	}
	print_str[string_length] = '\0'; //add the null character after extracting the rest of the string
	//Implement the test if enabled.
	if (test){
		for (int i = 0; i < string_length; i++){
			test_struct.result_str[i] = print_str[i];
		}
	}
	else{
		leuart_start(HM10_LEUART0, print_str, strlen(print_str));
	}

	return false;
}

/***************************************************************************//**
 * @brief
 *	Update increment and modulo of the write pointer
 *
 * @details
 *	Updates the write pointer and accounts for wrapping with modulus math described
 *	in the textbook.
 *
 * @param[in] *index_struct
 *	The location of the struct containing the buffer information to be modified.
 *
 * @param[in] update_by
 * 	How much to update the write pointer by.
 *
 ******************************************************************************/
static void update_circ_wrtindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by){
	//__disable_irq();
	index_struct->write_ptr = (index_struct->write_ptr + update_by) & (index_struct->size_mask);
	//__enable_irq();
}

/***************************************************************************//**
 * @brief
 *	Update decrement and modulo of the read pointer
 *
 * @details
 *	Updates the read pointer and accounts for wrapping with modulus math described
 *	in the textbook.
 *
 * @param[in] *index_struct
 *	The location of the struct containing the buffer information to be modified.
 *
 * @param[in] update_by
 * 	How much to update the read pointer by.
 ******************************************************************************/
static void update_circ_readindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by){
	//__disable_irq();
	index_struct->read_ptr = (index_struct->read_ptr + update_by) & (index_struct->size_mask);
	//__enable_irq();
}
