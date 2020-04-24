/**
 * @file scheduler.h
 **/
//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef SCHEDULER_H
#define	SCHEDULER_H

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

#endif /* SCHEDULER_H */