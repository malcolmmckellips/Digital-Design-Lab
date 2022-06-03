
#ifndef SRC_HEADER_FILES_APP_H_
#define SRC_HEADER_FILES_APP_H_

//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_cmu.h"
#include "em_prs.h"
#include "cmu.h"
#include "gpio.h"
#include "si7021.h"
#include "ble.h"

//***********************************************************************************
// defined files
//***********************************************************************************
//Lab 2 PWM defines
#define		PWM_PER				3.1		// PWM period in seconds
#define		PWM_ACT_PER			0.10	// PWM active period in seconds
#define		LETIMER0_ROUTE_OUT0	LETIMER_ROUTELOC0_OUT0LOC_LOC28
#define		LETIMER0_OUT0_EN	false
#define		LETIMER0_ROUTE_OUT1	0
#define		LETIMER0_OUT1_EN	false

//Lab3 Event defines
#define LETIMER0_COMP0_EVT  	0x00000001  //0b0000001
#define LETIMER0_COMP1_EVT 		0x00000002  //0b0000010
#define LETIMER0_UF_EVT   		0x00000004 	//0b0000100
#define SI7021_READ_EVT			0x00000008	//0b0001000

//Lab 4 Event Defines
#define BOOT_UP_EVT				16			//0b0010000

//Lab 5 defines
#define LEUART0_TX_DONE_EVT			32			//0b0100000
#define LEUART0_RX_DONE_EVT			64			//0b1000000
//#define BLE_TEST_ENABLED

//Lab 7 defines
#define LEUART_F_CODE		"#TEMPF!"
#define LEUART_C_CODE		"#TEMPC!"
//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void app_peripheral_setup(void);
void app_letimer_pwm_open(float period, float act_period);

//Schedule event handler added in lab 3
void scheduled_letimer0_comp0_evt (void);
void scheduled_letimer0_comp1_evt (void);
void scheduled_letimer0_uf_evt (void);

//Schedule event handler added in lab4
void scheduled_si7021_done_evt (void);

//Schedule event handler added in lab5
void scheduled_boot_up_evt(void);

//Schedule tx done event handler added in lab5
void scheduled_tx_done_evt(void);

//Schedule rx done event added in lab 6
void scheduled_rx_done_evt(void);

#endif /* SRC_HEADER_FILES_APP_H_ */
