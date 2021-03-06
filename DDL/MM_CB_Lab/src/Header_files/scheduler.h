#ifndef SRC_HEADER_FILES_SCHEDULER_H_
#define SRC_HEADER_FILES_SCHEDULER_H_

//***********************************************************************************
// Include files
//***********************************************************************************
#include <stdint.h>
#include "em_assert.h"



//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void scheduler_open(void);
void add_scheduled_event(uint32_t event);
void remove_scheduled_event(uint32_t event);
uint32_t get_scheduled_events(void);



#endif /* SRC_HEADER_FILES_scheduler_H_ */
