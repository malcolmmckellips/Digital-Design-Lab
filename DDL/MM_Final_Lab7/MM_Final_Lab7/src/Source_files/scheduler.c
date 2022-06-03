/**
 * @file scheduler.c
 * @author Malcolm McKellips
 * @date 2 February 2020
 * @brief Contains functions for opening scheduler and getting/setting events
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Libraries

//** Silicon Lab include files
#include "em_emu.h"
#include "em_assert.h"

//** User/developer include files
#include "scheduler.h"


//***********************************************************************************
// private variables
//***********************************************************************************
static unsigned int event_scheduled;


/***************************************************************************//**
 * @brief
 *   Used to open scheduler module
 *
 * @details
 * 	 Opens the scheduler functionality by resetting the private variable event_scheduled to 0.
 *
 *
 ******************************************************************************/
void scheduler_open(void){
	event_scheduled = 0;
}

/***************************************************************************//**
 * @brief
 *   Used to add an event to the scheduler
 *
 * @details
 * 	 Adds an event setting the correct bits in the private event_scheduled variable
 *
 * @param[in] event
 *   32 bit unsigned integer specifying the bits to set in event_scheduled to
 *   schedule an event.
 *
 ******************************************************************************/
void add_scheduled_event(uint32_t event) {
	__disable_irq();
	event_scheduled |= event;
	__enable_irq();
}

/***************************************************************************//**
 * @brief
 *   Used to remove an event from the scheduler
 *
 * @details
 * 	 Revmoves an event by clearing the correct bits in the private variable
 *
 * @param[in] event
 *   32 bit unsigned integer specifying the bits to clear in event_scheduled to
 *   clear a scheduled event.
 ******************************************************************************/
void remove_scheduled_event(uint32_t event){
	__disable_irq();
	event_scheduled &= ~event;
	__enable_irq();
}

/***************************************************************************//**
 * @brief
 *   Returns which events are currently scheduled for handling.
 *
 * @details
 * 	 Accesses and returns the private event_scheduled variable containing all
 * 	 scheduled events
 *
 * @return
 *	 A 32 bit unsigned integer containing the bits set for all scheduled events.
 ******************************************************************************/
uint32_t get_scheduled_events(void){
	return event_scheduled;
}
