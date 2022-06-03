/**
 * @file leuart.c
 * @author Malcolm McKellips
 * @date 3/29/2020
 * @brief Contains all the functions of the LEUART peripheral
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Library includes
#include <string.h>

//** Silicon Labs include files
#include "em_gpio.h"
#include "em_cmu.h"

//** Developer/user include files
#include "leuart.h"
#include "scheduler.h"

//***********************************************************************************
// defined files
//***********************************************************************************
//added
#define STARTFRAME_BIT 16


//***********************************************************************************
// private variables
//***********************************************************************************
uint32_t		rx_done_evt;
uint32_t		tx_done_evt;
volatile bool	leuart0_tx_busy; //updated to be volatile in lab 7
volatile bool	leuart0_rx_busy; //added in lab 7

//Changed to a struct for best practices in lab 6
LEUART_PAYLOAD_STRUCT leuart_payload;
LEUART_RX_PAYLOAD_STRUCT rx_payload; //added in lab 7


/***************************************************************************//**
 * @brief LEUART driver
 * @details
 *  This module contains all the functions to support the driver's state
 *  machine to transmit a string of data across the LEUART bus.  There are
 *  additional functions to support the Test Driven Development test that
 *  is used to validate the basic set up of the LEUART peripheral.  The
 *  TDD test for this class assumes that the LEUART is connected to the HM-18
 *  BLE module.  These TDD support functions could be used for any TDD test
 *  to validate the correct setup of the LEUART.
 *
 ******************************************************************************/

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************



/***************************************************************************//**
 * @brief this function will open the LEUART peripheral
 *
 * @details
 *	The function will open a leuart peripheral by configuring its transmission and receive
 *	details and routing via the leuart_settings struct. It will also enable necessary interrupts and perform checks to ensure that
 *	the peripheral has been opened correctly.
 *
 * @param[in] leuart
 * 	The leuart peripheral to be opened
 *
 * @param[in] leuart_settings
 * 	The configuration settings for the leuart being opened
 * ******************************************************************************/

void leuart_open(LEUART_TypeDef *leuart, LEUART_OPEN_STRUCT *leuart_settings){
	if (leuart == LEUART0) CMU_ClockEnable(cmuClock_LEUART0, true);

	//Check proper clock initialization
	if((leuart->STARTFRAME & 0x01 ) == 0){
		leuart->STARTFRAME = 0x01;
		//EFM_ASSERT((leuart->SYNCBUSY) & STARTFRAME_BIT); //Added this, possible error source...Check that leuart is in lower clock domain
		while (leuart->SYNCBUSY); //wait for syncronization to finish
		EFM_ASSERT(leuart->STARTFRAME & 0x01);
		leuart->STARTFRAME = 0x00; //clear start frame
		while (leuart->SYNCBUSY); //wait for synchronization
	}
	else{
		leuart->STARTFRAME &= ~(0x01); //clear the zero bit
		//EFM_ASSERT((leuart->SYNCBUSY) & STARTFRAME_BIT); //Added this, possible error source...Check that leuart is in lower clock domain
		while (leuart->SYNCBUSY); //wait for sync
		EFM_ASSERT (!(leuart->STARTFRAME & 0x01)); //ensure the bit has been cleared
	}

	//Using LEUART_Init (might want to move to the end)
	//Declare local init struct
	LEUART_Init_TypeDef leuart_init_struct;

	//Setup the local events
	rx_done_evt = leuart_settings->rx_done_evt;
	tx_done_evt = leuart_settings->tx_done_evt;

	//initialize the leuart struct for peripheral initialization
	leuart_init_struct.baudrate = leuart_settings->baudrate;
	leuart_init_struct.databits = leuart_settings->databits;
	leuart_init_struct.enable = leuart_settings->enable;
	leuart_init_struct.parity = leuart_settings->parity;
	leuart_init_struct.refFreq = leuart_settings->refFreq;
	leuart_init_struct.stopbits = leuart_settings->stopbits;

	//Initialize the leuart peripheral
	LEUART_Init(leuart,&leuart_init_struct);
	while(leuart->SYNCBUSY);
	//Secondary check (only necessary for TXEN and RXEN)
	if (leuart_settings->tx_en)
		while(!(leuart->STATUS & LEUART_STATUS_TXENS)); //Wait for the tx enable to be set
	if (leuart_settings->rx_en)
		while(!(leuart->STATUS & LEUART_STATUS_RXENS)); //Wait for the rx enable to be set

	//Perform the routing for the TX and RX signals
	//As per the data sheet:
	//PD10:   LEU0_TX#18 We have tx on d10 so use 18
	//		  LEU0_RX#17
	//PD11:   LEU0_TX#19
	//		  LEU0_RX#18 We have rx on d11 so use 18

	//Setup TX and Rx locations (will be correctly shifted when passed in)
	leuart->ROUTELOC0 = leuart_settings->rx_loc;
	leuart->ROUTELOC0 |= leuart_settings->tx_loc;

	//Enable the routed pins for tx
	if (leuart_settings->tx_pin_en)
		leuart->ROUTEPEN |= LEUART_ROUTEPEN_TXPEN;
	else
		leuart->ROUTEPEN &= ~LEUART_ROUTEPEN_TXPEN;

	//Enable the routed pins for rx
	if(leuart_settings->rx_pin_en)
		leuart->ROUTEPEN |= LEUART_ROUTEPEN_RXPEN;
	else
		leuart->ROUTEPEN &= ~LEUART_ROUTEPEN_RXPEN;


	//Clear the tx and rx buffers and shift registers to put in a known state
	leuart->CMD = LEUART_CMD_CLEARRX;
	while(leuart->SYNCBUSY);

	leuart->CMD = LEUART_CMD_CLEARTX;
	while(leuart->SYNCBUSY);

	//Configure and enable interrupts
	//Initialize and clear the desired interrupts;
	leuart->IEN = 0;
	leuart->IFC = LEUART_IEN_TXBL | LEUART_IEN_TXC;

	//Enable the desired interrupts
	//Enable interupts at the CPU level
	leuart->IEN |= LEUART_IEN_TXC; //might want to wait to enable this until state machine (old)
	//NOTE: txbl will be enabled in the start function

	//Enable Interrupts at the CPU level
	if (leuart == LEUART0)
		NVIC_EnableIRQ(LEUART0_IRQn);

	//Confirm that the enable signals have been correctly configured
	//Confirm rx is enabled if it should be
	if (leuart_settings->rx_en)
		EFM_ASSERT(leuart->STATUS & LEUART_STATUS_RXENS);
	else
		EFM_ASSERT((leuart->STATUS & LEUART_STATUS_RXENS) == 0);

	//Confirm TX is enabled if it should be
	if (leuart_settings->tx_en)
		EFM_ASSERT(leuart->STATUS & LEUART_STATUS_TXENS);
	else
		EFM_ASSERT((leuart->STATUS & LEUART_STATUS_TXENS) == 0);

}

/***************************************************************************//**
 * @brief Function for starting the LEUART module
 * @details
 * 	The necessary sleep mode will be blocked and the boolean state of the state machine
 * 	will be switched to leuart0_tx_busy = true to indicate that a write is in progress.
 * 	Then the TXBL interrupt will be enabled, allowing an immediate TXBL interrupt
 * 	and sending our state machine to the next state.
 * @param[in] leuart
 * 	The leuart peripheral to start
 * @param[in] string
 * 	The string data to send over LEUART
 * @param[in] string_len
 * 	The length of the string being sent
 * ******************************************************************************/

void leuart_start(LEUART_TypeDef *leuart, char *string, uint32_t string_len){
	sleep_block_mode(LEUART_TX_EM);

	leuart_payload.state = INITIALIZE;

	//initialize the private payload variables
	leuart0_tx_busy = true;
	leuart_payload.leuart = leuart;
	//transmit_string = *string; //could also try using strcpy
	strcpy(leuart_payload.string,string);
	leuart_payload.length = string_len;
	leuart_payload.index = 0;

	//Begin the state machine.
	leuart_payload.state = SEND_CHAR;
	leuart->IEN |= LEUART_IEN_TXBL; //enable the txbl interrupt, and should recieve one right away
}

/***************************************************************************//**
 * @brief Function for servicing a txbl interrupt
 * @details
 * 	If a TXBL interrupt occurs, and we have yet to transmit the entire string, stay in the current state
 *  but transmit the next character. If a TXBL interrupt occurs, and we have transmitted the entire string
 *  set the boolean state (tx_busy) to false, and disable txbl. We then now waiting for a TXC interrupt.
 ******************************************************************************/
static void leuart_send_char(void){
	switch(leuart_payload.state){
		case INITIALIZE:
			EFM_ASSERT(false);
			break;
		case SEND_CHAR:
			//We still have some of the string to send
			if (leuart_payload.index < leuart_payload.length){
				//send the current byte
				leuart_app_transmit_byte(leuart_payload.leuart, leuart_payload.string[leuart_payload.index]);
				while(leuart_payload.leuart->SYNCBUSY);
				leuart_payload.index++;
			}
			//We are finished transmitting the data, transmit is no longer busy, wait for txc
			if (leuart_payload.index >= leuart_payload.length) {
				leuart_payload.leuart->IEN &= ~LEUART_IEN_TXBL;  //disable txbl
				leuart_payload.state = WAIT_TXC;
			}
			break;
		case WAIT_TXC:
			EFM_ASSERT(false);
			break;
		case LEUART_FINISH:
			EFM_ASSERT(false);
			break;
		default:
			EFM_ASSERT(false);
			break;
	}
}
/***************************************************************************//**
 * @brief Function for servicing a txc interrupt
 * @details
 * 	If a TXC interrupt occurs while we are waiting for it, the process is finished and we can
 * 	unblock the sleep mode and add the scheduled event that the transmission has finished.
 ******************************************************************************/
static void leuart_finish(void){
	switch(leuart_payload.state){
		case INITIALIZE:
			EFM_ASSERT(false);
			break;
		case SEND_CHAR:
			//We still have some of the string to send
			EFM_ASSERT(false);
			break;
		case WAIT_TXC:
			leuart0_tx_busy = false;
			sleep_unblock_mode(LEUART_TX_EM);
			add_scheduled_event(tx_done_evt);
			leuart_payload.state = LEUART_FINISH;
			break;
		case LEUART_FINISH:
			EFM_ASSERT(false);
			break;
		default:
			EFM_ASSERT(false);
			break;
	}
}

/***************************************************************************//**
 * @brief Function for handling IRQ0 Interrupts
 * @details
 * 	The proper LEUART function for handling writes will be called if there is a TXBL interrupt.
 * 	If a TXC interrupt is received, transmission is over and we can unblock sleep mode and schedule
 * 	a tx_done_evt
 *******************************************************************************/

void LEUART0_IRQHandler(void){
	uint32_t int_flag = LEUART0->IF & LEUART0->IEN;
	LEUART0->IFC = int_flag;

	if (int_flag & LEUART_IEN_TXBL){
		leuart_send_char();
	}
	if(int_flag & LEUART_IEN_TXC){
		leuart_finish();
	}
}

/***************************************************************************//**
 * @brief Function for accessing the private leuart0_tx_busy variable from outside this module
 * @details
 * 	This function will return the private leuart0_tx_busy variable.
 * @param[in] leuart
 * 	The address o fthe leuart peripheral to use.
 *
 *******************************************************************************/
bool leuart_tx_busy(LEUART_TypeDef *leuart){
	//__disable_irq();
	if (leuart == LEUART0)
		return leuart0_tx_busy;
	EFM_ASSERT(false);
	return false;
	//__enable_irq();
}

/***************************************************************************//**
 * @brief
 *   LEUART STATUS function returns the STATUS of the peripheral for the
 *   TDD test
 *
 * @details
 * 	 This function enables the LEUART STATUS register to be provided to
 * 	 a function outside this .c module.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @return
 * 	 Returns the STATUS register value as an uint32_t value
 *
 ******************************************************************************/

uint32_t leuart_status(LEUART_TypeDef *leuart){
	uint32_t	status_reg;
	status_reg = leuart->STATUS;
	return status_reg;
}

/***************************************************************************//**
 * @brief
 *   LEUART CMD Write sends a command to the CMD register
 *
 * @details
 * 	 This function is used by the TDD test function to program the LEUART
 * 	 for the TDD tests.
 *
 * @note
 *   Before exiting this function to update  the CMD register, it must
 *   perform a SYNCBUSY while loop to ensure that the CMD has by synchronized
 *   to the lower frequency LEUART domain.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @param[in] cmd_update
 * 	 The value to write into the CMD register
 *
 ******************************************************************************/

void leuart_cmd_write(LEUART_TypeDef *leuart, uint32_t cmd_update){

	leuart->CMD = cmd_update;
	while(leuart->SYNCBUSY);
}

/***************************************************************************//**
 * @brief
 *   LEUART IF Reset resets all interrupt flag bits that can be cleared
 *   through the Interrupt Flag Clear register
 *
 * @details
 * 	 This function is used by the TDD test program to clear interrupts before
 * 	 the TDD tests and to reset the LEUART interrupts before the TDD
 * 	 exits
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 ******************************************************************************/

void leuart_if_reset(LEUART_TypeDef *leuart){
	leuart->IFC = 0xffffffff;
}

/***************************************************************************//**
 * @brief
 *   LEUART App Transmit Byte transmits a byte for the LEUART TDD test
 *
 * @details
 * 	 The BLE module will respond to AT commands if the BLE module is not
 * 	 connected to the phone app.  To validate the minimal functionality
 * 	 of the LEUART peripheral, write and reads to the LEUART will be
 * 	 performed by polling and not interrupts.
 *
 * @note
 *   In polling a transmit byte, a while statement checking for the TXBL
 *   bit in the Interrupt Flag register is required before writing the
 *   TXDATA register.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @param[in] data_out
 *   Byte to be transmitted by the LEUART peripheral
 *
 ******************************************************************************/

void leuart_app_transmit_byte(LEUART_TypeDef *leuart, uint8_t data_out){
	while (!(leuart->IF & LEUART_IF_TXBL));
	leuart->TXDATA = data_out;
}


/***************************************************************************//**
 * @brief
 *   LEUART App Receive Byte polls a receive byte for the LEUART TDD test
 *
 * @details
 * 	 The BLE module will respond to AT commands if the BLE module is not
 * 	 connected to the phone app.  To validate the minimal functionality
 * 	 of the LEUART peripheral, write and reads to the LEUART will be
 * 	 performed by polling and not interrupts.
 *
 * @note
 *   In polling a receive byte, a while statement checking for the RXDATAV
 *   bit in the Interrupt Flag register is required before reading the
 *   RXDATA register.
 *
 * @param[in] leuart
 *   Defines the LEUART peripheral to access.
 *
 * @return
 * 	 Returns the byte read from the LEUART peripheral
 *
 ******************************************************************************/

uint8_t leuart_app_receive_byte(LEUART_TypeDef *leuart){
	uint8_t leuart_data;
	while (!(leuart->IF & LEUART_IF_RXDATAV));
	leuart_data = leuart->RXDATA;
	return leuart_data;
}

//Added in Lab 7~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/***************************************************************************//**
 * @brief
 *  Test driven development test to confirm proper functionality of the receive function of LEUART module
 *
 * @details
 * 	 Verifies the LEUART0 start and signal frame operation and proper initialization of the LEUART
 * 	 module for receives. The state machine for receiving a string message delimited with
 * 	 the start and singal frames will also be tested. To perform these tests, the leuart peripheral will be
 * 	 set to loopback mode for the duration of the test function so that all transmissions over leuart will be fed back
 * 	 into the receive of the leuart to test the receive functionality.
 * 	 The tests are as follows:
 * 	 Test 1: Confirm that the correct sleep mode has been blocked when rx is initialized
 * 	 Test 2: Verify that the start frame and the signal frames have been formatted properly as well as the block enable and the necessary interrupts.
 *	 Test 3: Test that data is being blocked before the start frame is received, also verify proper index movement after start frame
 *   Test 4: Test signal frame and the leuart receive state machine.
 *   Test 5: Test A second string transmission/receive to ensure that the state machine allows multiple receptions off of a signal initialization.
 *
 *   Test Escapes:
 *   	-The test does not explicitly verify that the old rx buffer gets cleared when a new start frame is received (although several of the tests will likely test this)
 *		-The test does not explicitly verify state transitions. However, the overall function of the state machine is tested, so the functionality itself is verified.
 *		-The test does not account for errors in the leuart transmit process. If there are errors in tx, the rx test will not be air tight.
 *		-The test does not test performance in case of an rx_buffer overflow
 *		-The test does not test when multiple start frames or multiple sig frames are in a test string.
 *
 * @note
 *   This test will assume already functional transmission over LEUART as it operates in loopback mode
 *
 ******************************************************************************/

void leuart_rx_test(void){
	//Assume that the setup of the start frame, sig frame, RXBLOCKEN, SFUBRX, etc. has been formatted in leuart_open prior to test


	rx_payload.leuart->CTRL |= LEUART_CTRL_LOOPBK; //Configure the module to loopback mode to connect leuart_rx to leuart_tx

	//Assume that leuart_open has configured the following start and sig frame in leuart_open
	char startF = '#';
	char sigF   = '!';

	//Test 1: Confirm that the correct sleep mode has been blocked when rx is initialized
	EFM_ASSERT(current_block_energy_mode() <= EM2);

	//Test 2: Verify that the start frame and the signal frames have been formatted properly as well as the block enable and the necessary interrupts
	EFM_ASSERT(rx_payload.leuart->STARTFRAME == startF);
	EFM_ASSERT(rx_payload.leuart->SIGFRAME == sigF);
	EFM_ASSERT(rx_payload.leuart->STATUS & LEUART_STATUS_RXBLOCK);
	EFM_ASSERT(rx_payload.leuart->IEN & LEUART_IEN_STARTF);
	EFM_ASSERT(rx_payload.leuart->IEN & LEUART_IEN_SIGF);
	EFM_ASSERT(rx_payload.leuart->IEN & LEUART_IEN_RXDATAV);

	//Test 3 : Test that data is being blocked before the start frame is received, also verify proper index movement after start frame
	//Transmit 'a'
	rx_payload.leuart->TXDATA = 'a';
	while (!(rx_payload.leuart->IF & LEUART_IF_TXC)); //wait for the transmit to occur
	rx_payload.leuart->IFC |= LEUART_IFC_TXC;		  //Clear the TXC interrupt

	//Transmit '#'. This is the start frame so we expect the previous 'a' to be blocked, and this '#' to be received
	rx_payload.leuart->TXDATA = '#';
	while (!(rx_payload.leuart->IF & LEUART_IF_TXC)); //wait for the transmit to occur
	rx_payload.leuart->IFC |= LEUART_IFC_TXC;		  //Clear the TXC interrupt
	while (!(rx_payload.leuart->IF & LEUART_IF_RXDATAV)); //wait for the receive to occur

	EFM_ASSERT(rx_payload.leuart->RXDATA == startF); //Ensure the start frame has been received correctly
	EFM_ASSERT(rx_payload.index == 1); //Ensure that the index has updated properly upon a start fram initialization

	//Test 4: Test signal frame and the leuart receive state machine
	//Send a complete string sequence. Expect to receive only the data delimited by the start and sig frames
	char rx_test_str  [80] = "fakedata#TEST!baddata";
	char rx_result_str[80] = "#TEST!";
	leuart_start(rx_payload.leuart,rx_test_str,strlen(rx_test_str));
	while(leuart0_tx_busy);
	while(leuart0_rx_busy);

	EFM_ASSERT(rx_payload.rx_buffer == rx_result_str);

	//Test 5: Test A second string transmission/receive to ensure that the state machine allows multiple receptions off of a signal initialization
	// 		  This test will also verify that blocking has been re-enabled after a transmission has completed
	EFM_ASSERT(rx_payload.leuart->STATUS & LEUART_STATUS_RXBLOCK);
	char rx_test_str2[80] = "attempt2#SUCCESS!nonsense";
	char rx_result_str2[80] = "#SUCCESS!";
	leuart_start(rx_payload.leuart,rx_test_str2,strlen(rx_test_str2));
	while(leuart0_tx_busy);
	while(leuart0_rx_busy);

	EFM_ASSERT(rx_payload.rx_buffer == rx_result_str2);

	//Finally, stop loopback mode and then ensure that the sleep mode still allows for further receives after the test
	rx_payload.leuart->CTRL &=  !LEUART_CTRL_LOOPBK;
	EFM_ASSERT(current_block_energy_mode() <= EM2);
}
