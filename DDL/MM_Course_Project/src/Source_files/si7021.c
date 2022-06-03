/**
 * @file si7021.c
 * @author Malcolm McKellips
 * @date 2 February 2020
 * @brief Functions for working with Si7021 temperature sensor
 *
 */

//** Standard Libraries

//** Silicon Lab include files

//** User/developer include files
#include "si7021.h"
//***********************************************************************************
// defined files
//***********************************************************************************

//***********************************************************************************
// global variables
//***********************************************************************************

//***********************************************************************************
// private variables
//***********************************************************************************
static uint32_t temperature_data;
//***********************************************************************************
// functions
//***********************************************************************************


/***************************************************************************//**
 * @brief
 *   Function open and initialize the si7021 device driver
 *
 * @details
 *	The function will define an initialization i2c struct for opening i2c for the si7021 device
 *	and will load all of the necessary routing information into an i2c_io_struct. Then i2c will be
 *	opened based on these structs loaded with the parameters specified in si7021.c
 *
 ******************************************************************************/
void si7021_i2c_open(void){
	//Struct for configuring io for si7021
	I2C_IO_STRUCT si7021_io_struct;

	//Initialize the values for the si7021 struct
	si7021_io_struct.scl_pin 	= SI7021_SCL_PIN;
	si7021_io_struct.scl_port 	= SI7021_SCL_PORT;
	si7021_io_struct.sda_pin	= SI7021_SDA_PIN;
	si7021_io_struct.sda_port	= SI7021_SDA_PORT;

	//Struct for initialization parameters of the si7021
	I2C_OPEN_STRUCT si7021_open_struct;

	//Input the values for the initialization struct
	si7021_open_struct.clhr				  = SI7021_I2C_CLK_RATIO;
	si7021_open_struct.enable			  = true;
	si7021_open_struct.freq				  = SI7021_I2C_FREQ;
	si7021_open_struct.master			  = true;
	si7021_open_struct.refFreq			  = 0;
	si7021_open_struct.scl_route_enable	  = SI7021_SCL_EN;
	si7021_open_struct.sda_route_enable   = SI7021_SDA_EN;
	si7021_open_struct.scl_route_location = SI7021_SCL_LOC;
	si7021_open_struct.sda_route_location = SI7021_SDA_LOC;

	//Open the si7021 with all of the required parameters.
	i2c_open(SI7021_I2C,&si7021_open_struct,&si7021_io_struct,SI7021_READ_EVT);

}

/***************************************************************************//**
 * @brief
 *   Function to read from si7021 over i2c
 *
 * @details
 *	The function will start an i2c operation to read the temperature from the si7021
 *	with the specified i2c peripheral and will store the data in the private
 *	temperature_data variable in si7021.c
 *
 ******************************************************************************/
void si7021_read(void){
	i2c_start(SI7021_I2C, Si7021_dev_addr, SI7021_TEMP_NO_HOLD, true, 2, &temperature_data);

}

/***************************************************************************//**
 * @brief
 *   Function to handle an i2c_ack interrupt
 *
 * @details
 *	The function will return the current private temperature_data variable after
 *	converting it to a F float value. As of lab 5, it will also format this temperature
 *	and then output it via bluetooth and the BLE module. Updated Lab 7: the transmission of
 *	and formatting of the string have been shifted to the application layer.
 *
 ******************************************************************************/
float si7021_get_tempF (void){
	float tempC = ( ((175.72 * (float) temperature_data)/65536) - 46.85);
	float tempF = (tempC * 1.8) + 32;

	//String formatting process courtesy of Wade in Slack - Lab 5 (moved to application layer, lab 7)
	//char temptransmit [20];
	//double number = (double)tempF;
	//sprintf(temptransmit, "\nTemp = %4.1f", number);
	//transmit the temperature string
	//ble_write(temptransmit);

	return tempF;
}
