/**
 * @file i2c.c
 * @author Malcolm McKellips
 * @date 2 February 2020
 * @brief Modular i2c driver
 *
 */


//** Standard Libraries

//** Silicon Lab include files
#include "em_assert.h"
#include "em_cmu.h"
//** User/developer include files
#include "i2c.h"
#include "gpio.h"
//***********************************************************************************
// defined files
//***********************************************************************************
#define ACK_INTERRUPT  			(1 << 6)
#define NACK_INTERRUPT 			(1 << 7)
#define DATA_VALID_INTERRUPT 	(1 << 5)
#define MASTER_STOP_INTERRUPT	(1 << 8)

//***********************************************************************************
// global variables
//***********************************************************************************

//***********************************************************************************
// private variables
//***********************************************************************************
// Defines struct for the I2C operation and keeps state of the I2C state machine
static I2C_PAYLOAD private_payload;
static uint32_t	i2c_done_evt;

//***********************************************************************************
// functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 *   Driver to open and initialize an i2c peripheral
 *
 * @details
 *	The function will enable the clock tree to the requested I2C and then utilizing 3 input structs,
 *	will initialize all of the necessary i2c information for the requested I2C peripheral. It will also
 *	enable interrupts and finally, will reset the i2c bus
 *
 * @param[in] i2c
 * 	The i2c peripheral base address (I2C0 or I2C1)
 *
 *
 * @param[in] i2c_setup
 *  Struct which contains the i2c_init parameters for initialization
 *
 * @param[in] i2c_io
 * 	Struct containing the i2c routing information
 *
 * @param[in] i2c_event
 * 	The defined event for when an I2C event has finished
 *
 *
 *
 ******************************************************************************/
void i2c_open(I2C_TypeDef *i2c, I2C_OPEN_STRUCT *i2c_setup, I2C_IO_STRUCT *i2c_io, uint32_t i2c_event){

	//set the private i2c_done_evt variable
	i2c_done_evt = i2c_event;
	//enable the clock to the appropriate I2C peripheral
	if (i2c == I2C0) CMU_ClockEnable(cmuClock_I2C0, true); //double check we don't have to mess with cm_ctrl
	if (i2c == I2C1) CMU_ClockEnable(cmuClock_I2C1, true);

	//Check proper clock initialization
	if((i2c->IF & 0x01 ) == 0){
		i2c->IFS = 0x01;
		EFM_ASSERT(i2c->IF & 0x01);
		i2c->IFC = 0x01;
	}
	else{
		i2c->IFC = 0x01;
		EFM_ASSERT (!(i2c->IF & 0x01));
	}

	//------------------Initialize the i2c peripheral---------------------
	 I2C_Init_TypeDef i2c_init_struct; //Struct for initialization of I2C peripheral

	 //Place the values from the input struct into the correct places in the init struct
	 i2c_init_struct.clhr	 = i2c_setup->clhr;
	 i2c_init_struct.enable  = i2c_setup->enable;
	 i2c_init_struct.freq	 = i2c_setup->freq;
	 i2c_init_struct.master  = i2c_setup->master;
	 i2c_init_struct.refFreq = i2c_setup->refFreq;

	 //Initialize the I2C peripheral with the necessary em_lib routine
	 I2C_Init(i2c,&i2c_init_struct);

	 //----------Route the I2C signals from internal I2C to pins-----------
	 //Look in the data sheet for the ROUTELOC0's corresponding to:
	 //PC11: I2C0_SDA #16
	 //		 I2C0_SCL #15 PC11 will be the SCL location
	 //		 I2C1_SDA #20
	 //		 I2C1_SCL #19
	 //PC10: I2C0_SDA #15 PC10 will be the SDA location
	 //		 I2C0_SCL #14
	 //		 I2C1_SDA #19
	 //		 I2C1_SCL #18
	 //For this project, we are using PC11 = SCL and PC10 = SDA
	 //si7021.c/.h will decide whether to utilize I2C0 or I2C1
	 //For example i2c->ROUTELOC0 = I2C_ROUTELOC0_SCLLOC_LOC15;
	 // will set the I2C0_SCL to the correct location with #15

	 //Setup SDA route location
	 i2c->ROUTELOC0  = i2c_setup->sda_route_location;
	 //Setup SCL route loation
	 i2c->ROUTELOC0 |= i2c_setup->scl_route_location;

	 //Set or clear the routing pin enable for SDA
	if (i2c_setup->sda_route_enable)
		i2c->ROUTEPEN |= I2C_ROUTEPEN_SDAPEN;
	else
		i2c->ROUTEPEN &= ~I2C_ROUTEPEN_SDAPEN;

		 //Set or clear the routing pin enable for SCL
	if (i2c_setup->scl_route_enable)
		i2c->ROUTEPEN |= I2C_ROUTEPEN_SCLPEN;
	else
		i2c->ROUTEPEN &= ~I2C_ROUTEPEN_SCLPEN;

	//Configure and enable interrupts
	//Initialize and clear the desired interrupts
	i2c->IEN = 0;
	i2c->IFC = ACK_INTERRUPT | NACK_INTERRUPT | DATA_VALID_INTERRUPT | MASTER_STOP_INTERRUPT;

	//Enable the desired interrupts
	i2c->IEN |= ACK_INTERRUPT;
	i2c->IEN |= NACK_INTERRUPT;
	i2c->IEN |= DATA_VALID_INTERRUPT;
	i2c->IEN |= MASTER_STOP_INTERRUPT;

	//Enable Interrupts at the CPU level
	if (i2c == I2C0)
		NVIC_EnableIRQ(I2C0_IRQn);
	if (i2c == I2C1)
		NVIC_EnableIRQ(I2C1_IRQn);

	//Reset the i2c state machines of both the external i2c peripherals and PG
	//Might want before the interrupt stuff
	i2c_bus_reset(i2c,i2c_io);
}


/***************************************************************************//**
 * @brief
 *   Function to reset the i2c bus
 *
 * @details
 *	The function will toggle scl 9 times while sda is heald high in order to reset the bus
 *
 * @param[in] i2c
 * 	The i2c peripheral base address (I2C0 or I2C1)
 *
 * @param[in] i2c_io
 * 	Struct containing the i2c routing information
 *
 *
 *
 ******************************************************************************/
void i2c_bus_reset(I2C_TypeDef *i2c, I2C_IO_STRUCT *i2c_io){
	//Verify I2c SCL and SDA are both high in their active state
	EFM_ASSERT(GPIO_PinInGet(i2c_io->scl_port, i2c_io->scl_pin));
	EFM_ASSERT(GPIO_PinInGet(i2c_io->sda_port, i2c_io->sda_pin));

	//Reset the bus by toggling SCL 9 times while SDA is held high
	for (int i = 0; i<9; i++){
		//HOLD SDA HIGH
		GPIO_PinOutSet(i2c_io->sda_port,i2c_io->sda_pin); //might not need
		//TOGGLE SCL
		GPIO_PinOutClear(i2c_io->scl_port,i2c_io->scl_pin);
		GPIO_PinOutSet(i2c_io->scl_port,i2c_io->scl_pin);
	}

	//Reset the Pearl Gecko's i2c state machine by performing the following command
	i2c->CMD = I2C_CMD_ABORT;
}

/***************************************************************************//**
 * @brief
 *   Function to start the i2c peripheral
 *
 * @details
 *	The function will initialize the private i2c payload struct with the parameters specified and
 *	then send the initialization features to start an i2c transaction. Then it will advance the state
 *	of the state machine to the second state.
 *
 * @param[in] i2c
 * 	The i2c peripheral base address (I2C0 or I2C1)
 *
 *
 * @param[in] device_address
 * 	The address of the i2c device to be communicated with
 *
 *
 * @param[in] register_address
 * 	The function command to be sent to the device being communicated with
 *
 * @param[in] read_or_write
 * 	A boolean value stipulating whether to read or write to the device
 *
 * @param[in] bytes
 *  An integer specifying the number of bytes to be send or received
 *
 *  @param[in] data
 *  The address of where to store the i2c data
 *
 *
 *
 ******************************************************************************/
void i2c_start(I2C_TypeDef *i2c, uint32_t device_address, uint32_t register_address, bool read_or_write,uint32_t bytes, uint32_t *data){
	//Ensure i2c has been setup properly
	EFM_ASSERT((i2c->STATE & _I2C_STATE_STATE_MASK) == I2C_STATE_STATE_IDLE);  // X = the I2C peripheral #

	//Block the necessary sleep mode for peripheral
	sleep_block_mode(I2C_EM_BLOCK);

	//Initialize the private state machine structure with values passed in via payload_config
	private_payload.i2c				 = i2c;
	private_payload.bytes 			 = bytes;
	private_payload.data_buffer 	 = data;
	private_payload.device_address 	 = device_address;
	private_payload.register_address = register_address;
	private_payload.read_or_write	 = read_or_write;
	//initialize state (might want this to just be hard coded init)
	private_payload.state = INIT;


	//Send in initialization features
	private_payload.i2c->CMD    =  I2C_CMD_START;
	private_payload.i2c->TXDATA = (private_payload.device_address << 1 )| WRITE;

	private_payload.state = SEND_MEASURE;
	//go to the second state and wait for an
}

/***************************************************************************//**
 * @brief
 *   Function to handle an i2c_ack interrupt
 *
 * @details
 *	The function will perform the necessary actions required based on receiving an
 *	ack in the current state machine state and will EFM_ASSERT if the current state
 *	machine state is not a valid one for an ack to be received in.
 *
 ******************************************************************************/
static void i2c_ack(void){
	switch(private_payload.state){
		case INIT:
			EFM_ASSERT(false);
			break;
		case SEND_MEASURE:
			//The first ack has just been received. Send the measure command
			private_payload.i2c->TXDATA = private_payload.register_address;
			private_payload.state = SEND_READ;
			break;
		case SEND_READ:
			//The second ack has been received,send the read command
			private_payload.i2c->CMD =  I2C_CMD_START;
			private_payload.i2c->TXDATA = (private_payload.device_address << 1 )| READ;
			private_payload.state = WACK3;
			break;
		case WACK3:
			//We have finally received the data, so we don't have to send anymore read attempts
			private_payload.state = GET_MSB;
			break;
		case GET_MSB:
			EFM_ASSERT(false);
			break;
		case GET_LSB_STOP:
			EFM_ASSERT(false);
			break;
		case END:
			EFM_ASSERT(false);
			break;
		default:
			EFM_ASSERT(false);
			break;
	}

}

/***************************************************************************//**
 * @brief
 *   Function to handle an i2c_nack interrupt
 *
 * @details
 *	The function will perform the necessary actions required based on receiving a
 *	nack in the current state machine state and will EFM_ASSERT if the current state
 *	machine state is not a valid one for an nack to be received in.
 *
 ******************************************************************************/
static void i2c_nack(void){
	switch(private_payload.state){
		case INIT:
			EFM_ASSERT(false);
			break;
		case SEND_MEASURE:
			EFM_ASSERT(false);
			break;
		case SEND_READ:
			EFM_ASSERT(false);
			break;
		case WACK3:
			private_payload.i2c->CMD =  I2C_CMD_START;
			private_payload.i2c->TXDATA = (private_payload.device_address << 1 )| READ;
			break;
		case GET_MSB:
			EFM_ASSERT(false);
			break;
		case GET_LSB_STOP:
			EFM_ASSERT(false);
			break;
		case END:
			EFM_ASSERT(false);
			break;
		default:
			EFM_ASSERT(false);
			break;
	}
}

/***************************************************************************//**
 * @brief
 *   Function to handle an i2c_rxdatav interrupt
 *
 * @details
 *	The function will perform the necessary actions required based on receiving an
 *	rx_datav in the current state machine state and will EFM_ASSERT if the current state
 *	machine state is not a valid one for an rxdatav to be received in.
 *
 ******************************************************************************/
static void i2c_rxdatav(void){
	switch(private_payload.state){
		case INIT:
			EFM_ASSERT(false);
			break;
		case SEND_MEASURE:
			EFM_ASSERT(false);
			break;
		case SEND_READ:
			EFM_ASSERT(false);
			break;
		case WACK3:
			EFM_ASSERT(false);
			break;
		case GET_MSB:
			//We are ready to receive the MSB
			*private_payload.data_buffer = private_payload.i2c->RXDATA << 8; //receive data
			private_payload.i2c->CMD	 = I2C_CMD_ACK;					     //Send ACK
			private_payload.state 		 = GET_LSB_STOP;
			break;
		case GET_LSB_STOP:
			//We are ready to reccive the LSB
			*private_payload.data_buffer |= private_payload.i2c->RXDATA;
			private_payload.i2c->CMD	 = I2C_CMD_NACK;
			private_payload.i2c->CMD	 = I2C_CMD_STOP;
			private_payload.state = END;
			break;
		case END:
			EFM_ASSERT(false);
			break;
		default:
			EFM_ASSERT(false);
			break;
	}
}

/***************************************************************************//**
 * @brief
 *   Function to handle an i2c_mstop interrupt
 *
 * @details
 *	The function will perform the necessary actions required based on receiving an
 *	mstop in the current state machine state and will EFM_ASSERT if the current state
 *	machine state is not a valid one for an msotp to be received in.
 *
 ******************************************************************************/
static void i2c_mstop(void){
	switch(private_payload.state){
		case INIT:
			EFM_ASSERT(false);
			break;
		case SEND_MEASURE:
			EFM_ASSERT(false);
			break;
		case SEND_READ:
			EFM_ASSERT(false);
			break;
		case WACK3:
			EFM_ASSERT(false);
			break;
		case GET_MSB:
			EFM_ASSERT(false);
			break;
		case GET_LSB_STOP:
			EFM_ASSERT(false);
			break;
		case END:
			//We have received the MSTOP command and thus have completed i2C operation
			sleep_unblock_mode(I2C_EM_BLOCK);
			add_scheduled_event(i2c_done_evt);
			break;
		default:
			EFM_ASSERT(false);
			break;
	}
}

/***************************************************************************//**
 * @brief
 *   Function to handle an i2c0 interrupt by routing it to one of the above functions
 *
 * @details
 *	The function will clear the interrupt flag for I2C0 and then will call the
 *	necessary function above to advance the state machine based on which interrupt was
 *	received.
 *
 ******************************************************************************/
void I2C0_IRQHandler(void){
	uint32_t int_flag = I2C0->IF & I2C0->IEN;
	I2C0->IFC = int_flag;

	if (int_flag & ACK_INTERRUPT){
		i2c_ack();
	}

	if (int_flag & NACK_INTERRUPT){
		i2c_nack();
	}

	if (int_flag & DATA_VALID_INTERRUPT){
		i2c_rxdatav();
	}

	if (int_flag & MASTER_STOP_INTERRUPT){
		i2c_mstop();
	}
}

