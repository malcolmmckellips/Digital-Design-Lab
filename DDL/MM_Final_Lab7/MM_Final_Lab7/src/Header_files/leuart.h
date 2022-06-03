
#ifndef SRC_HEADER_FILES_LEUART_H_
#define SRC_HEADER_FILES_LEUART_H_
//***********************************************************************************
// Include files
//***********************************************************************************

#include "em_leuart.h"
#include "sleep_routines.h"


//***********************************************************************************
// defined files
//***********************************************************************************

#define LEUART_TX_EM		EM3 //check this
#define LEUART_RX_EM		EM3 //check this


//***********************************************************************************
// Enumerations
//***********************************************************************************
enum leuart_states{
	INITIALIZE,
	SEND_CHAR,
	WAIT_TXC,
	LEUART_FINISH
};

//Added Lab 7
enum leuart_rx_states{
	RX_INIT,
	RX_IDLE,
	RX_STARTED,
	RX_RECEIVE,
	RX_FINISH
};

//***********************************************************************************
// data structures
//***********************************************************************************

/***************************************************************************//**
 * @addtogroup leuart
 * @{
 ******************************************************************************/

typedef struct {
	//Variables for LEUART_Init_TypeDef struct
	uint32_t							baudrate;
	LEUART_Databits_TypeDef				databits;
	LEUART_Enable_TypeDef				enable;
	LEUART_Parity_TypeDef 				parity;
	uint32_t							refFreq; //Added manually.
	LEUART_Stopbits_TypeDef				stopbits;
	bool						rxblocken;
	bool						sfubrx;
	bool						startframe_en;
	char						startframe;
	bool						sigframe_en;
	char						sigframe;
	uint32_t					rx_loc;
	uint32_t					rx_pin_en;
	uint32_t					tx_loc;
	uint32_t					tx_pin_en;
	bool						rx_en;		//LEUART RX enable
	bool						tx_en;		//LEUART TX enable
	uint32_t					rx_done_evt;
	uint32_t					tx_done_evt;
} LEUART_OPEN_STRUCT;


/** @} (end addtogroup leuart) */

//Payload struct to store variables for the state machine
typedef struct {
	LEUART_TypeDef *leuart;
	uint32_t index;		//index for current point in the string
	uint32_t length; 		//number of bytes left to transfer
	char     string[100];  //The actual string to transmit
	enum leuart_states state;
} LEUART_PAYLOAD_STRUCT;

//Added in phase 7. Structure for receive data
typedef struct {
	LEUART_TypeDef 			*leuart;
	uint32_t 				index;
	char 	 				rx_buffer[100];
	enum leuart_rx_states	rx_state;
} LEUART_RX_PAYLOAD_STRUCT;
//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void leuart_open(LEUART_TypeDef *leuart, LEUART_OPEN_STRUCT *leuart_settings);
void LEUART0_IRQHandler(void);
void leuart_start(LEUART_TypeDef *leuart, char *string, uint32_t string_len);
bool leuart_tx_busy(LEUART_TypeDef *leuart);

uint32_t leuart_status(LEUART_TypeDef *leuart);
void leuart_cmd_write(LEUART_TypeDef *leuart, uint32_t cmd_update);
void leuart_if_reset(LEUART_TypeDef *leuart);
void leuart_app_transmit_byte(LEUART_TypeDef *leuart, uint8_t data_out);
uint8_t leuart_app_receive_byte(LEUART_TypeDef *leuart);

//Added in Lab 7
void leuart_rx_test(void);

#endif
