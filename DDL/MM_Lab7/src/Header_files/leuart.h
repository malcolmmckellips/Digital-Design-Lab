
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

void leuart_finish (void); //new

uint32_t leuart_status(LEUART_TypeDef *leuart);
void leuart_cmd_write(LEUART_TypeDef *leuart, uint32_t cmd_update);
void leuart_if_reset(LEUART_TypeDef *leuart);
void leuart_app_transmit_byte(LEUART_TypeDef *leuart, uint8_t data_out);
uint8_t leuart_app_receive_byte(LEUART_TypeDef *leuart);
void leuart_rx_test(void);
#endif
