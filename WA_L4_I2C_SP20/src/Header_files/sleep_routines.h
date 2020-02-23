/**
 * @file sleep_routines.h
 * @brief EM Manager for Application
 ***************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
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
**/

#ifndef SLEEP_ROUTINES_H
#define SLEEP_ROUTINES_H

#include "em_emu.h"
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
