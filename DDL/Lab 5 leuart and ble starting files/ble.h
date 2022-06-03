/*
 * ble.h
 *
 *  Created on: May 22, 2019
 *      Author: xxx
 */
//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Libraries
#include <stdbool.h>
#include <stdint.h>

#include "leuart.h"

//***********************************************************************************
// defined files
//***********************************************************************************

#define HM10_LEUART0		XXX
#define HM10_BAUDRATE		XXX
#define	HM10_DATABITS		XXX
#define HM10_ENABLE		XXX
#define HM10_PARITY		XXX
#define HM10_REFFREQ		XXX		// use reference clock
#define HM10_STOPBITS		XXX

#define LEUART0_TX_ROUTE	XXX	
#define LEUART0_RX_ROUTE	XXX	


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void ble_open(uint32_t tx_event, uint32_t rx_event);
void ble_write(char *string);

bool ble_test(char *mod_name);
