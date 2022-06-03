#ifndef SRC_HEADER_FILES_I2C_H_
#define SRC_HEADER_FILES_I2C_H_

//***********************************************************************************
// Include files
//***********************************************************************************
//** Silicon Lab include files
#include "em_i2c.h" //if there's an error replace "" with <>
#include "em_gpio.h"

//User include files
#include "sleep_routines.h"
#include "scheduler.h"

//***********************************************************************************
// defined files
//***********************************************************************************
//define the first sleep mode that i2c peripheral cannot enter
#define I2C_EM_BLOCK     EM2

//Define the read/write bits for SI7021
#define WRITE	0
#define READ	1

//***********************************************************************************
// Enumerations
//***********************************************************************************
enum i2c_states{
	INIT,
	SEND_MEASURE,
	SEND_READ,
	WACK3,
	GET_MSB,
	GET_LSB_STOP,
	END
};

//***********************************************************************************
// global variables
//***********************************************************************************
//I2C Open Struct to open an I2C peripheral
typedef struct {
	//Variables defined in I2C_Init_TypeDef Struct of PG HAL
	I2C_ClockHLR_TypeDef 	clhr;	//Clock low/high ratio control
	bool 					enable;	//enable i2C peripheral when initialization completed
	uint32_t 				freq;	//(Max) I2C bus frequency to use. Only applicable operating in master mode.
	bool 					master; //Set to master (true) or slave (false)
	uint32_t 				refFreq;//i2c reference clock assumed when configuring bus frequency
	//Variables for routing I2C peripheral to external pins for SCL and SDA
	bool 					sda_route_enable; 	//I2Cn_ROUTEPEN[0]
	bool 					scl_route_enable; 	//I2Cn_ROUTEPEN[1]
	uint16_t 				sda_route_location; //I2Cn_ROUTELOC0[5:0]
	uint16_t  				scl_route_location; //I2Cn_ROUTELOC0[13:8]
	//NOTE: The locations will be defined in a #define in app.c (already shifted, thus the 16 bits)
} I2C_OPEN_STRUCT;



//I2C IO struct to reset the I2C communication bus
typedef struct {
	GPIO_Port_TypeDef 	scl_port;
	unsigned int 		scl_pin;
	GPIO_Port_TypeDef 	sda_port;
	unsigned int 		sda_pin;
} I2C_IO_STRUCT;



//I2C payload struct to define the I2C operation and keep the state of
// the I2C state machine
typedef struct {
	enum i2c_states	state; 					//stores the state via the i2c_state enum
	I2C_TypeDef 	*i2c;					//which i2c peripheral to use
	uint32_t		device_address; 		//i2c device address
	uint32_t		register_address; 		//i2c register address being requested
	bool			read_or_write;			//read = 1, write = 0
	uint32_t		*data_buffer;			//Pointer of where to store a read result or get write data
	uint32_t 		bytes;
} I2C_PAYLOAD;

//***********************************************************************************
// function prototypes
//***********************************************************************************
void i2c_open(I2C_TypeDef *i2c, I2C_OPEN_STRUCT *i2c_setup, I2C_IO_STRUCT *i2c_io, uint32_t i2c_event); //added star on io
void i2c_bus_reset(I2C_TypeDef *i2c, I2C_IO_STRUCT *i2c_io);
void I2C0_IRQHandler(void);
void i2c_start(I2C_TypeDef *i2c, uint32_t device_address, uint32_t register_address, bool read_or_write,uint32_t bytes, uint32_t *data);

#endif /* SRC_HEADER_FILES_i2c_H_ */
