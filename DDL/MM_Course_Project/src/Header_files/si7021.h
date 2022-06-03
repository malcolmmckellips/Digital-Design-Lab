
#ifndef SRC_HEADER_FILES_si7021_H_
#define SRC_HEADER_FILES_si7021_H_

//***********************************************************************************
// Include files
//***********************************************************************************
#include "i2c.h"
//#include "gpio.h" should already be included in app.h
#include "app.h"
#include <stdio.h> //for sprintf
//***********************************************************************************
// defined files
//***********************************************************************************
#define Si7021_dev_addr  		0x40 // Si7021 I2C device address
#define SI7021_TEMP_NO_HOLD   	0xF3 // Si7021 temp read/no hold cmd
//http://devtools.silabs.com/dl/documentation/doxygen/5.7/efm32pg12/html/group__I2C.html#gabb1516548b4528328682d6be09a3e3a5
#define SI7021_I2C_FREQ  		I2C_FREQ_FAST_MAX   //1/(Tlow + Thigh + 0.3us + 0.3us) = 1/(1.3 + 0.65 + 0.6)us = 392157Hz
#define SI7021_I2C_CLK_RATIO 	i2cClockHLRAsymetric
#define SI7021_SCL_LOC  		I2C_ROUTELOC0_SCLLOC_LOC15
#define SI7021_SCL_EN  			true	//maybe boolean true, maybe gpio enable thingy
#define SI7021_SDA_LOC  		I2C_ROUTELOC0_SDALOC_LOC15
#define SI7021_SDA_EN 			true	//maybe boolean true, maybe gpio enable thingy
#define SI7021_I2C   			I2C0 // The PG I2C peripheral to use

//***********************************************************************************
// global variables
//***********************************************************************************

//***********************************************************************************
// function prototypes
//***********************************************************************************
void si7021_i2c_open(void);
void si7021_read(void);
float si7021_get_tempF (void);

#endif /* SRC_HEADER_FILES_si7021_H_ */
