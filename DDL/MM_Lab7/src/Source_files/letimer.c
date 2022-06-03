/**
 * @file letimer.c
 * @author Malcolm McKellips
 * @date 2 February 2020
 * @brief Contains all the LETIMER driver functions
 *
 */


//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Libraries

//** Silicon Lab include files
#include "em_cmu.h"
#include "em_assert.h"

//** User/developer include files
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
static uint32_t scheduled_comp0_evt;
static uint32_t scheduled_comp1_evt;
static uint32_t scheduled_uf_evt;
//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Driver to open an set an LETIMER peripheral in PWM mode
 *
 * @details
 * 	 This routine is a low level driver.  The application code calls this function
 * 	 to open one of the LETIMER peripherals for PWM operation to directly drive
 * 	 GPIO output pins of the device and/or create interrupts that can be used as
 * 	 a system "heart beat" or by a scheduler to determine whether any system
 * 	 functions need to be serviced. This function will also setup the interrupts
 * 	 for an letimer. Finally, this function will block the desired sleep mode if
 * 	 necessary.
 *
 * @note
 *   This function is normally called once to initialize the peripheral and the
 *   function letimer_start() is called to turn-on or turn-off the LETIMER PWM
 *   operation.
 *
 * @param[in] letimer
 *   Pointer to the base peripheral address of the LETIMER peripheral being opened
 *
 * @param[in] app_letimer_struct
 *   Is the STRUCT that the calling routine will use to set the parameters for PWM
 *   operation
 *
 ******************************************************************************/

void letimer_pwm_open(LETIMER_TypeDef *letimer, APP_LETIMER_PWM_TypeDef *app_letimer_struct){
	LETIMER_Init_TypeDef letimer_pwm_values;


	/*  Enable the routed clock to the LETIMER0 peripheral */
	if (letimer == LETIMER0)CMU_ClockEnable(cmuClock_LETIMER0, true); //NOTE:CMU_LFACLKEN0.LETIMER0

	letimer_start(letimer,false);
	/* Use EFM_ASSERT statements to verify whether the LETIMER clock tree is properly
	 * configured and enabled
	 */
	letimer->CMD = LETIMER_CMD_START;
	while (letimer->SYNCBUSY);
	EFM_ASSERT(letimer->STATUS & LETIMER_STATUS_RUNNING);
	letimer->CMD = LETIMER_CMD_STOP;


	// Initialize letimer for PWM operation
	letimer_pwm_values.bufTop = false;		// Comp1 will not be used to load comp0, but used to create an on-time/duty cycle
	letimer_pwm_values.comp0Top = true;		// load comp0 into cnt register when count register underflows enabling continuous looping
	letimer_pwm_values.debugRun = app_letimer_struct->debugRun;
	letimer_pwm_values.enable = app_letimer_struct->enable;
	letimer_pwm_values.out0Pol = 0;			// While PWM is not active out, idle is DEASSERTED, 0
	letimer_pwm_values.out1Pol = 0;			// While PWM is not active out, idle is DEASSERTED, 0
	letimer_pwm_values.repMode = letimerRepeatFree;	// Setup letimer for free running for continuous looping
	letimer_pwm_values.ufoa0 = letimerUFOAPwm;		// No action of outputs on underflow
	letimer_pwm_values.ufoa1 = letimerUFOAPwm;		// No action of outputs on underflow

	LETIMER_Init(letimer, &letimer_pwm_values);		// Initialize letimer
	//Stall for synchronization
	while(letimer->SYNCBUSY);

	/* Calculate the value of COMP0 and COMP1 and load these control registers
	 * with the calculated values
	 */
	LETIMER_CompareSet(letimer,0,app_letimer_struct->period*LETIMER_HZ);
	letimer->CNT = 0;
	LETIMER_CompareSet(letimer,1, app_letimer_struct->active_period*LETIMER_HZ); //might want to check math...

	/* Set the REP0 mode bits for PWM operation
	 *
	 * Use the values from app_letimer_struct input argument for ROUTELOC0 and ROUTEPEN enable
	 */
	 letimer->REP0 = 1; //LETIMER_RepeatSet(letimer,0,1);
	 letimer->ROUTEPEN  = app_letimer_struct->out_pin_0_en;
	 letimer->ROUTELOC0 = app_letimer_struct->out_pin_route0;
	 //we only did stuff with 0, note that we also have 1 in the struct^^^^

	 //Configure Interrupts (Lab 3):
	 //Clear and disable all letimer interrupts
	 letimer->IEN = 0;
	 letimer->IFC = app_letimer_struct->comp0_evt | app_letimer_struct->comp1_evt | app_letimer_struct->uf_evt;

	 //Enable interrupts enabled in app.c
	 if (app_letimer_struct->comp0_irq_enable)
		 letimer->IEN |= app_letimer_struct->comp0_evt;
	 if (app_letimer_struct->comp1_irq_enable)
		 letimer->IEN |= app_letimer_struct->comp1_evt;
	 if (app_letimer_struct->uf_irq_enable)
		 letimer->IEN |= app_letimer_struct->uf_evt;

	 //enable these interrupts to the Pearl Gecko ARM Cortex CPU
	 NVIC_EnableIRQ(LETIMER0_IRQn);

	 //Configure event scheduling private variables. **CHECK THIS MIGHT BE ENABLES NOT EVENTS**
	 scheduled_comp0_evt = app_letimer_struct->comp0_evt;
	 scheduled_comp1_evt = app_letimer_struct->comp1_evt;
	 scheduled_uf_evt 	 = app_letimer_struct->uf_evt;

	/* We will not able the LETIMER0 at this time */

	 //If the letimer is running and enabled, add sleep block (Lab 3)
	 if (letimer->STATUS & LETIMER_STATUS_RUNNING){
		 sleep_block_mode(LETIMER_EM);
	 }
}

/***************************************************************************//**
 * @brief
 *   Used to enable, turn-on, or disable, turn-off, the LETIMER peripheral
 *
 * @details
 * 	 This function allows the application code to initialize the LETIMER
 * 	 peripheral separately to enabling or disabling the LETIMER. On the rising
 * 	 edge of an letimer enable signal, this function will also set the necessary
 * 	 sleep block mode. Similarly, on a falling edge it will unblock that mode.
 *
 * @note
 *   Application code should not directly access hardware resources.  The
 *   application program should access the peripherals through the driver
 *   functions
 *
 * @param[in] letimer
 *   Pointer to the base peripheral address of the LETIMER peripheral being
 *   enabled or disable
 *
 * @param[in] enable
 *   true enables the LETIMER to start operation while false disables the
 *   LETIMER
 *
 ******************************************************************************/
void letimer_start(LETIMER_TypeDef *letimer, bool enable){
	//LETIMER_Enable(letimer,enable); //old (Changed lab 5 from Poorn's suggestion)
	//Energy mode blocking (Lab 3)
	//Check for rising edge of letimer getting enabled
	if (enable && !(letimer->STATUS & LETIMER_STATUS_RUNNING) ){
			LETIMER_Enable(letimer,enable); //new (Changed lab 5 from Poorn's suggestion)
			while(letimer->SYNCBUSY); //new (Changed lab 5 from Poorn's suggestion)
			sleep_block_mode(LETIMER_EM);
	}
	//Check for falling edge of letimer getting disabled
	if (!enable && (letimer->STATUS & LETIMER_STATUS_RUNNING)){
		LETIMER_Enable(letimer,enable); //new (Changed lab 5 from Poorn's suggestion)
		while(letimer->SYNCBUSY); //new (Changed lab 5 from Poorn's suggestion)
		sleep_unblock_mode(LETIMER_EM);
	}
	//Stall
	//while(letimer->SYNCBUSY); //old (Changed lab 5 from Poorn's suggestion)
}

/***************************************************************************//**
 * @brief
 *   Used to schedule required events based on LETIMER0 interrupt
 *
 * @details
 * 	 On an LETIMER0 interrupt, this function will clear the interrupt flag and
 * 	 schedule the necessary type of event based on the type of interrupt.
 *
 * @note
 *   The application checks that interrupts have been properly cleared. If
 *   stuck in an EFM assert statement from this function, that is why.
 *
 *
 ******************************************************************************/
void LETIMER0_IRQHandler(void){
	//Interrupt flag variable
	uint32_t int_flag;

	int_flag = LETIMER0->IF & LETIMER0->IEN; //Isolate only the interrupts of interest
	LETIMER0->IFC = int_flag;	//Clear the interrupt flag reg to allow same interrupt to occur again
	//
	if (int_flag & LETIMER_IF_COMP0){
		//Make sure that interrupt is cleared
		EFM_ASSERT(!(LETIMER0->IF & LETIMER_IF_COMP0));
		//Schedule the proper event
		add_scheduled_event(scheduled_comp0_evt);
	}
	if (int_flag & LETIMER_IF_COMP1){
		//Make sure that interrupt is cleared
		EFM_ASSERT(!(LETIMER0->IF & LETIMER_IF_COMP1));
		//Schedule the proper event
		add_scheduled_event(scheduled_comp1_evt);
	}
	if (int_flag & LETIMER_IF_UF){
		//Make sure that interrupt is cleared
		EFM_ASSERT(!(LETIMER0->IF & LETIMER_IF_UF));
		//Schedule the proper event
		add_scheduled_event(scheduled_uf_evt);
	}

}

