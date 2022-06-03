/**
 * @file app.c
 * @author Malcolm McKellips
 * @date 2 February 2020
 * @brief Contains functions that will set up the clocks, gpio, letimer,si7021, i2c, and event handlers temperature reads
 *
 */


//***********************************************************************************
// Include files
//***********************************************************************************
#include "app.h"
#include "letimer.h"
#include "scheduler.h"


//***********************************************************************************
// defined files
//***********************************************************************************

//***********************************************************************************
// global variables
//***********************************************************************************

//***********************************************************************************
// private variables
//***********************************************************************************
volatile bool is_temp_F; //Will be 1 if we are transmitting temp in F, and 0 if transmitting temp in C.


//module header...
/***************************************************************************//**
 * @brief Application layer tying together I2C temperature reads from si7021 and interfacing with a BLE module.
 * @details
 *	The application will receive temperature data from the on board si7021 over I2C.
 *	The application will then display the temperature over BLE through LEUART transmissions.
 *	If a string is received from the BLE through the LEUART receive functionality, it will be decoded.
 *	If the string received is "#TEMPF!" the temperature data will begin to be printed in Fahrenheit and if the
 *	string received is "#TEMPC!" the temperature data will begin to be printed in Celcius.
 *
 ******************************************************************************/

//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *	Setup peripherals for pwm, sleep, and scheduling
 *
 * @details
 *	Calls functions to open the cmu, gpio, and letimer in order to produce pwm operation.
 *	Also, this function will open and initialize the scheuler and sleep modules. As of lab 4, it
 *	also opens the i2c peripheral for si7021 and ble modules. It will then schedule the bootup event.
 *
 * @note
 *	Will open the letimer for pwm with the macro parameters defined in app.h
 *
 ******************************************************************************/
void app_peripheral_setup(void){
	//Open the scheduler and sleep routines(lab 3)
	scheduler_open();
	sleep_open();

	cmu_open();
	gpio_open();
	app_letimer_pwm_open(PWM_PER, PWM_ACT_PER);

	//Temperature/humidity sensor (Lab 4)
	si7021_i2c_open();

	//Bluetooth open (Lab 5)
	ble_open(LEUART0_TX_DONE_EVT,LEUART0_RX_DONE_EVT);


	//Boot up event (Lab 5)
	add_scheduled_event(BOOT_UP_EVT);

}


/***************************************************************************//**
 * @brief
 *	Configures an letimer pwm structure with the necessary pwm parameter
 *
 * @details
 *	This function will set the period and active period of a pwm structure based on input parameters.
 *	It will also route and enable/disable pin 0 and pin 1 for pwm output as defined by macro parameters in app.h
 *	It will also set the enable to false (as letimer is enabled later) and debugRun to true
 *
 * @note
 * 	The struct for pwm operation is created in this function and then passeed to the letimer_pwm_open function
 *
 * @param[in] period
 *	The total period to be set for pwm operation.
 *
 * @param[in] act_period
 * 	The active(on) period to be set for pwm operation
 *
 ******************************************************************************/
void app_letimer_pwm_open(float period, float act_period){
	// Initializing LETIMER0 for PWM operation by creating the
	// letimer_pwm_struct and initializing all of its elements


	APP_LETIMER_PWM_TypeDef  my_PWM_struct; //Declaration of personal pwm struct
	//Lab 2 PWM parameter setup
	my_PWM_struct.period		 = period;	// defined by PWM_PER
	my_PWM_struct.active_period  = act_period; //defined by PWM_ACT_PER
	my_PWM_struct.out_pin_route0 = LETIMER0_ROUTE_OUT0;
	my_PWM_struct.out_pin_route1 = LETIMER0_ROUTE_OUT1;
	my_PWM_struct.out_pin_0_en	 = LETIMER0_OUT0_EN;
	my_PWM_struct.out_pin_1_en	 = LETIMER0_OUT1_EN;
	my_PWM_struct.enable		 = false;
	my_PWM_struct.debugRun		 = false;
	//Lab 3 event parameter setup
	my_PWM_struct.uf_irq_enable	 	= true;
	my_PWM_struct.comp0_irq_enable 	= false;
	my_PWM_struct.comp1_irq_enable	= false;
	my_PWM_struct.comp0_evt			= LETIMER0_COMP0_EVT;
	my_PWM_struct.comp1_evt			= LETIMER0_COMP1_EVT;
	my_PWM_struct.uf_evt			= LETIMER0_UF_EVT;

	letimer_pwm_open(LETIMER0, &my_PWM_struct); //Call letimer_pwm_open
}

/***************************************************************************//**
 * @brief
 *   Handles an letimer0 comp0 event that has been scheduled.
 *
 * @details
 * 	 Currently, this function simply clears the comp0 event and enters an assert.
 *
 * @note
 *   We should not enter this function in lab 3
 *
 ******************************************************************************/
void scheduled_letimer0_comp0_evt (void){
	//clear the scheduled event
	remove_scheduled_event(LETIMER0_COMP0_EVT);
	//We should not have reached this. Enter an assert
	EFM_ASSERT(false);
}

/***************************************************************************//**
 * @brief
 *   Handles an letimer0 comp1 event that has been scheduled.
 *
 * @details
 * 	 Currently, this function simply clears the comp1 event and enters an assert.
 *
 * @note
 *   We should not enter this function in lab 3
 *
 ******************************************************************************/
void scheduled_letimer0_comp1_evt (void){
	//clear the scheduled event
	remove_scheduled_event(LETIMER0_COMP1_EVT);
	//We should not have reached this. Enter an assert
	EFM_ASSERT(false);
}

/***************************************************************************//**
 * @brief
 *   Handles an letimer0 uf event that has been scheduled.
 *
 * @details
 * 	 Removes the scheduled event and then unblocks the current energy mode, and
 * 	 initiates a read from the si7021 temperature sensor
 *
 * @note
 *   The temperature should thus be read every period of letimer0.
 *
 ******************************************************************************/
void scheduled_letimer0_uf_evt (void){
	EFM_ASSERT(get_scheduled_events() & LETIMER0_UF_EVT);
	//clear the event
	remove_scheduled_event(LETIMER0_UF_EVT); //might want to clear at the end
	si7021_read();

}

/***************************************************************************//**
 * @brief
 *   Handles an si7021 read event that has been scheduled.
 *
 * @details
 * 	 Removes the scheduled event and then retrieves the temperature in F.
 * 	 Then, depending on a threshold temperature of 80 degrees, LED1 will either
 * 	 be turned on, or turned off. Added lab 7 - Will transmit the temperature over
 * 	 bluetooth in celcius or in Farenheit depending on a private variable
 *
 ******************************************************************************/
void scheduled_si7021_done_evt (void){
	EFM_ASSERT(get_scheduled_events() & SI7021_READ_EVT);
	//clear the event
	remove_scheduled_event(SI7021_READ_EVT);
	float tempF = si7021_get_tempF();

	//String formatting process courtesy of Wade in Slack - Lab 5
	char temptransmit [20];
	double number = 0;
	if (is_temp_F){
		number = (double)tempF;
		sprintf(temptransmit, "\nTempF = %4.1f", number);
	}
	else{
		number = ((double)tempF - 32.0) * (5.0/9.0);
		sprintf(temptransmit, "\nTempC = %4.1f", number);
	}
	//transmit the temperature string
	ble_write(temptransmit);

	if (tempF >= 80.0)
		GPIO_PinOutSet(LED1_port,LED1_pin);
	else
		GPIO_PinOutClear(LED1_port,LED1_pin);
}

/***************************************************************************//**
 * @brief
 *   Handles a boot up that has been scheduled.
 *
 * @details
 * 	 Removes the scheduled event and then if BLE_TEST_ENABLED is defined,
 * 	 will attempt to change the name of the module. It will perform a TDD test to
 * 	 establish proper function of a circular buffer. Then, it will write the string
 * 	 "\nHello World\n", as well as two other string writes to the ble module. This
 * 	 will test if the application is compatible with back to back writes to the circular buffer.
 *
 ******************************************************************************/
void scheduled_boot_up_evt (void){
	//clear the event
 	remove_scheduled_event(BOOT_UP_EVT);

 	//used to have letimer start here

	//Unit Test For BLE Module (lab 5)
#ifdef BLE_TEST_ENABLED
	bool ble_success = ble_test("Malcolm Test");
	EFM_ASSERT(ble_success);
	//Test sending a basic string with delay
	for (int i = 0; i < 20000000; i++);
#endif

	leuart_rx_test(); //added in lab 7
	circular_buff_test();//added in lab 6
	ble_circ_init();
	ble_write("\nHello World\n");
	ble_write("Circular Buffer Lab\n"); //added lab 6
	ble_write("Malcolm\n"); //added lab 6


}

/***************************************************************************//**
 * @brief
 *   Handles a tx done evt that has been scheduled.
 *
 * @details
 * 	 Removes the scheduled event and then starts the LETIMER0 through
 * 	 a call to letimer_start(). Added Lab 6 -> will check and print a residual
 * 	 packet in the circular buffer.
 *
 ******************************************************************************/
void scheduled_tx_done_evt(void){
	//clear the event
	EFM_ASSERT(get_scheduled_events() & LEUART0_TX_DONE_EVT);
	remove_scheduled_event(LEUART0_TX_DONE_EVT);
	// Call to start the LETIMER operation
	letimer_start(LETIMER0, true); //moved here from bootup event
	//Added in lab 6
	ble_circ_pop(false);
}

/***************************************************************************//**
 * @brief
 *   Handles a rx done evt that has been scheduled.
 *
 * @details
 * 	 Removes the scheduled event and then processes a string received over LEUART.
 * 	 It will toggle the state of a private variable if a valid string has been received
 * 	 to decide whether to print temperature from the si7021 in F or C
 *
 ******************************************************************************/
void scheduled_rx_done_evt(void){
	//clear the event
	EFM_ASSERT(get_scheduled_events() & LEUART0_RX_DONE_EVT);
	remove_scheduled_event(LEUART0_RX_DONE_EVT);

	//Get the rx_buffer that was just transmitted and decode it to convert between F/C
	char temp_string[80];
	get_leuart_rx_buffer(temp_string);
	if (!strcmp(temp_string,LEUART_F_CODE)){
		is_temp_F = true;
	}
	else if (!strcmp(temp_string,LEUART_C_CODE)){
		is_temp_F = false;
	}
}
