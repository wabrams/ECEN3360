/**
 * @file scheduler.c
 * @author William Abrams
 * @date 28th Jan. 2020
 * @brief Scheduler Controller, for Application Events
**/

#include "scheduler.h"
#include "em_emu.h"
#include "em_assert.h"

static unsigned int event_scheduled;	/**< Scheduler Integer, each bit represents a different event **/

/**
 * @brief
 *	Initializes the scheduler
 * @details
 *	Sets event_scheduled to 0 (thus clearing any event bits)
 **/
void scheduler_open(void)
{
	event_scheduled = 0;
}

/**
 * @brief
 *	Adds event into the scheduler
 * @details
 *	Performs an OR operation to add event into the scheduler
 * @note
 *	Temporarily disables Interrupts, to prevent nested interrupts (keep things atomic)
 * @param[in] event
 *	The event to be set
 **/
void add_scheduled_event(uint32_t event)
{
	__disable_irq();
	event_scheduled |= event;
	__enable_irq();
}

/**
 * @brief
 *	Removes event from the scheduler
 * @details
 *	Performs a negated AND operation to remove event from the scheduler
 * @note
 *	Temporarily disables Interrupts, to prevent nested interrupts (keep things atomic)
 * @param[in] event
 *	The event to be set
 **/
void remove_scheduled_event(uint32_t event)
{
	__disable_irq();
	event_scheduled &= ~event;
	__enable_irq();
}

/**
 * @brief
 *	Returns scheduled events
 * @details
 * 	returns event_scheduled as a uint32_t
 * 	remember, each bit corresponds to a different event
 * @returns
 *	event_scheduled, the integer keeping track of our current events
 **/
uint32_t get_scheduled_events(void)
{
	return event_scheduled;
}
