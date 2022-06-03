/**
 * @file cmu.c
 * @author Malcolm McKellips
 * @date 2 February 2020
 * @brief The cmu.c module is responsible in enabling all oscillators and routing the clock tree for the application.
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// prototypes
//***********************************************************************************


/***************************************************************************//**
 * @brief
 *	Enables all necessary oscillators and routes the clock tree for the application
 *
 * @details
 *	This function will enable the HFPER clock and disable the LFRCO oscillator and LFXO.
 *	It will also route the low frequency rc oscillator to the correct low frequency clock tree which will then be enabled.
 *
 * @note
 * 	As this function is void, it will always format the same clock tree. To modify the clock tree, the enums within the function must be changed.
 *
 ******************************************************************************/
void cmu_open(void){

		CMU_ClockEnable(cmuClock_HFPER, true);

		// By default, LFRCO is enabled, disable the LFRCO oscillator
		CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);	// using LFXO or ULFRCO

		// Route LF clock to the LF clock tree
		// No requirement to enable the ULFRCO oscillator.  It is always enabled in EM0-4H

		//Lab 5 enabling and routing
		CMU_OscillatorEnable(cmuOsc_LFXO, true, true);		// ENABLE LFXO (Lab 5)
		CMU_ClockSelectSet(cmuClock_LFB , cmuSelect_LFXO);	// route LFXOO to LFB (Lab 5)


		CMU_ClockSelectSet(cmuClock_LFA , cmuSelect_ULFRCO);	// route ULFRCO to proper Low Freq clock tree

		CMU_ClockEnable(cmuClock_CORELE, true);					// Enable the Low Freq clock tree

		// Enabling the High Frequency Peripheral Clock Tree
		CMU_ClockEnable(cmuClock_HFPER, true);			// Enable the High Frequency Peripheral clock

}

