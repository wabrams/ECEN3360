/**
 * @file sleep_routines.h
 **/

#ifndef SLEEP_ROUTINES_H
#define SLEEP_ROUTINES_H

#include "em_emu.h"
#include "em_int.h"
#include "em_assert.h"
#include "em_core.h"

#define EM0 0 /**< Energy Mode 0 - Awake **/
#define EM1 1 /**< Energy Mode 1 - Sleep **/
#define EM2 2 /**< Energy Mode 2 - Sleep **/
#define EM3 3 /**< Energy Mode 3 - Sleep **/
#define EM4 4 /**< Energy Mode 4 - Hibernation **/
#define MAX_ENERGY_MODES 5 /**< Total Number of Energy Modes **/

//better implementation:
//enum energy_modes
//{
//	em0 = 0,
//	em1 = 1,
//	em2 = 2,
//	em3 = 3,
//	em4 = 4,
//	em_count
//};

void sleep_open(void);
void sleep_block_mode(uint32_t);
void sleep_unblock_mode(uint32_t);
void enter_sleep(void);
uint32_t current_block_energy_mode(void);

#endif /* SLEEP_ROUTINES_H_ */
