/**
 * @file sleep_routines.c
 * @author Malcolm McKellips
 * @date 11 February 2020
 * @brief This file contains functions for managing the sleep mode
 *
 */



/************************************************************************* *
 *
 *
 /* @file sleep.c

**************************************************************************
*
* @section License
* <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>

**************************************************************************
*
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
* DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
* obligation to support this Software. Silicon Labs is providing the
* Software "AS IS", with no express or implied warranties of any kind,
* including, but not limited to, any implied warranties of merchantability
* or fitness for any particular purpose or warranties against infringement
* of any proprietary rights of a third party.
*
* Silicon Labs will not be liable for any consequential, incidental, or
* special damages, or any other relief, or for any claim by any third party,
* arising from your use of this Software.
*
**************************************************************************
 */


//***********************************************************************************
// Include files
//***********************************************************************************
#include "sleep_routines.h"

//***********************************************************************************
// defined files
//***********************************************************************************

//***********************************************************************************
// private variables
//***********************************************************************************
 //Array to maintain state of which state that the Pearl Gecko cannot go into based on its current active peripherals
static int lowest_energy_mode[MAX_ENERGY_MODES];

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Used to open sleep module
 *
 * @details
 * 	 initializes the lowest_energy_mode blocking array to 0 at every index
 *
 ******************************************************************************/
void sleep_open(void){
	for (int i = 0; i<MAX_ENERGY_MODES;i++) lowest_energy_mode[i] = 0;
}

/***************************************************************************//**
 * @brief
 *   Used to enter a sleep mode based on which energy modes are blocked by peripherals
 *
 * @details
 * 	 Puts the device in the deepest sleep currently allowed by peripherals.
 *	 Will check sleep modes starting from EM0 until it finds one that is blocked.
 * @note
 *	 If no blocked sleep mode is found, the device will enter EM3.
 *
 ******************************************************************************/
void enter_sleep(void){
	if (lowest_energy_mode[EM0]>0) return;
	else if (lowest_energy_mode[EM1]>0) return;
	else if (lowest_energy_mode[EM2]>0){
		EMU_EnterEM1();
		return;
	}
	else if (lowest_energy_mode[EM3]>0){
		EMU_EnterEM2(true);
		return;
	}
	else EMU_EnterEM3(true);
	return;
}

/***************************************************************************//**
 * @brief
 *   Returns the index of which energy mode is currently blocked by periphals
 *
 * @details
 * 	 Will search through the lowest_energy_mode array starting from EM0
 * 	 until a blocked energy mode is found.
 *
 * @note
 * 	 If no blocked energy mode is found, the device will return the lowest possible
 * 	 energy mode.
 *
 * @return
 *	 A 32 bit unsigned integer corresponding to the current blocked energy mode.
 ******************************************************************************/
uint32_t current_block_energy_mode(void){
	for (int i = 0; i < MAX_ENERGY_MODES; i++){
		if (lowest_energy_mode[i] != 0)
			return i;
	}
	return (MAX_ENERGY_MODES - 1); //Go into lowest energy mode possible. We found no blocks in the search
}

/***************************************************************************//**
 * @brief
 *   Blocks the specified energy mode while in use by peripheral.
 *
 * @details
 * 	 Incrememnts the value in the lowest_energy_mode array at the specified
 * 	 energy mode. Also makes sure that the mode has not been blocked too many times.
 *
 * @param[in] EM
 * 	 The energy mode that is to be blocked by an active peripheral.
 ******************************************************************************/
void sleep_block_mode(uint32_t EM){
	__disable_irq();
	lowest_energy_mode[EM]++;
	EFM_ASSERT (lowest_energy_mode[EM] < 10);
	__enable_irq();
}

/***************************************************************************//**
 * @brief
 *   Unblocks the specified energy mode after peripheral no longer needs it.
 *
 * @details
 * 	 Decrements the value in the lowest_energy_mode array at the specified
 * 	 energy mode. Also makes sure that the mode has not been unblocked too many times.
 *
 * @param[in] EM
 * 	 The energy mode that is to be unblocked after peripheral is finished.
 ******************************************************************************/
void sleep_unblock_mode(uint32_t EM){
	__disable_irq();
	lowest_energy_mode[EM]--;
	EFM_ASSERT (lowest_energy_mode[EM] >= 0);
	__enable_irq();
}

