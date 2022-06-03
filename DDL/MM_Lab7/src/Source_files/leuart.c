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
uint32_t	rx_done_evt;
uint32_t	tx_done_evt;
bool		leuart0_tx_busy; //will also maintain the state of the state machine

//Local Payload variables for the state machine (can also put into a structure)
static LEUART_TypeDef *payload_leuart;
static uint32_t payload_index;		//index for current point in the string
static uint32_t payload_length; 		//number of bytes left to transfer
static char     payload_string[100];  //The actual string to transmit


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
 * @brief this function will open the LEUART periphera
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
 * @brief Function for handling IRQ0 Interrupts
 * @details
 * 	The proper LEUART function for handling writes will be called if there is a TXBL interrupt.
 * 	If a TXC interrupt is received, transmission is over and we can unblock sleep mode and schedule
 * 	a tx_done_evt
 *    ******************************************************************************/

void LEUART0_IRQHandler(void){
	uint32_t int_flag = LEUART0->IF & LEUART0->IEN;
	LEUART0->IFC = int_flag;

	if (int_flag & LEUART_IEN_TXBL){
		leuart_tx_busy(payload_leuart);
	}
	if(int_flag & LEUART_IEN_TXC){
		//we have finished the LEUART transmission. Make sure we are done then unblock
		//EFM_ASSERT(!leuart0_tx_busy);
		if (!leuart0_tx_busy){
			sleep_unblock_mode(LEUART_TX_EM);
			add_scheduled_event(tx_done_evt);
		}
	}
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
 *    ******************************************************************************/

void leuart_start(LEUART_TypeDef *leuart, char *string, uint32_t string_len){
	sleep_block_mode(LEUART_TX_EM);

	//initialize the private payload variables
	leuart0_tx_busy = true;
	payload_leuart = leuart;
	//transmit_string = *string; //could also try using strcpy
	strcpy(payload_string,string);
	payload_length = string_len;
	payload_index = 0;

	//Begin the state machine.
	leuart->IEN |= LEUART_IEN_TXBL; //enable the txbl interrupt, and should recieve one right away

}

/***************************************************************************//**
 * @brief Function for servicing a txbl interrupt
 * @details
 * 	If a TXBL interrupt occurs, and we have yet to transmit the entire string, stay in the current state
 *  but transmit the next character. If a TXBL interrupt occurs, and we have transmitted the entire string
 *  set the boolean state (tx_busy) to false, and disable txbl. We then now waiting for a TXC interrupt.
 * @param[in] leuart
 * 	The LEUART peripheral to use
  ******************************************************************************/

bool leuart_tx_busy(LEUART_TypeDef *leuart){
	//We still have some of the string to send
	if (payload_index < payload_length){
		//send the current byte
		leuart_app_transmit_byte(leuart, payload_string[payload_index]);
		while(leuart->SYNCBUSY);
		payload_index++;
		//return true
	}
	//We are finished transmitting the data, transmit is no longer busy, wait for txc
	if (payload_index >= payload_length) {
		leuart->IEN &= ~LEUART_IEN_TXBL;  //disable txbl
		leuart0_tx_busy = false;
		return false;
	}
	return true;

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


