/**
 * @file i2c.c
 * @author Malcolm McKellips
 * @date 2 February 2020
 * @brief GPIO setup for LED's and SI7021
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"
#include "em_cmu.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************

//module header...
/***************************************************************************//**
 * @brief GPIO driver
 * @details
 *	The driver consists of a single function, used for formatting the GPIO information
 *	necesesary for a program's operation at prograam start.
 *
 ******************************************************************************/


//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Set up GPIO for LED's and SI7021
 *
 * @details
 * 	 Configure the drive strengnth and pin mode of LED1, LED0 and SI7021
 *
 *
 *
 ******************************************************************************/

void gpio_open(void){

	CMU_ClockEnable(cmuClock_GPIO, true);

	// Set LED ports to be standard output drive with default off (cleared)
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
//	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);

	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
//	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);

	//Driving Enable signal for temperature/humidity sensor (lab 4) (default enabled)
	GPIO_DriveStrengthSet(SI7021_SENSOR_EN_PORT,gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(SI7021_SENSOR_EN_PORT,SI7021_SENSOR_EN_PIN,gpioModePushPull,true); //check in failure
	//Setup WiredAnd Mode (default enabled)
	GPIO_PinModeSet(SI7021_SCL_PORT,SI7021_SCL_PIN,gpioModeWiredAnd,true);
	GPIO_PinModeSet(SI7021_SDA_PORT,SI7021_SDA_PIN,gpioModeWiredAnd,true);


	//Driving LEUART pins (Lab 5)
	//Drive TX line and set pin mode (CHECK ENABLE IN PINMODESET!!!)
	GPIO_DriveStrengthSet(UART_TX_PORT,gpioDriveStrengthStrongAlternateWeak);
	GPIO_PinModeSet(UART_TX_PORT,UART_TX_PIN,gpioModePushPull,true);

	//Set RX line and set pin mode (CHECK ENABLE IN PINMODESET!!!)
	GPIO_PinModeSet(UART_RX_PORT,UART_RX_PIN,gpioModeInput,false);

}
