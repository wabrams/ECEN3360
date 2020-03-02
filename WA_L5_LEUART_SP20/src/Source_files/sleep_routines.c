/**
 * @file sleep_routines.c
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

#include "sleep_routines.h"
#include <stdbool.h>

static int lowest_energy_mode[MAX_ENERGY_MODES]; /**< array tracking blocks to sleep modes EM0 - EM4 **/

/**
 * @brief
 *	Initializes the Sleep Manager
 * @details
 *	Zeroes private array lowest_energy_mode
 * @note
 *	Will overwrite any existing blocks, this should only be called once
 **/
void sleep_open(void)
{
	for (int i = 0; i < MAX_ENERGY_MODES; i++)
		lowest_energy_mode[i] = 0;
}
/**
 * @brief
 *	Adds sleep block at specified energy mode
 * @details
 *	Increments lowest_energy_mode[em]
 * @note
 *	This function does not check the input energy mode, so it is possible to access
 *	an out of bounds value in the array. Be sure to use the predefined constants from
 *	sleep_routines.h (EM0, EM1, EM2, EM3, EM4)
 * @param[in] em
 *	The energy mode to be blocked
 **/
void sleep_block_mode(uint32_t em)
{
	__disable_irq();

	lowest_energy_mode[em]++;
	EFM_ASSERT(lowest_energy_mode[em] < 10);

	__enable_irq();
}
/**
 * @brief
 *	Removes sleep block at specified energy mode
 * @details
 *	Decrements lowest_energy_mode[em]
 * @note
 *	This function does not check the input energy mode, so it is possible to access
 *	an out of bounds value in the array. Be sure to use the predefined constants from
 *	sleep_routines.h (EM0, EM1, EM2, EM3, EM4).
 *	If unblock is called on an energy mode that is not blocked, you will end up in an
 *	EFM_ASSERT(), be sure to watch where you block and unblock sleep.
 * @param[in] em
 *	The energy mode to be unblocked
 **/
void sleep_unblock_mode(uint32_t em)
{
	__disable_irq();

	lowest_energy_mode[em]--;
	EFM_ASSERT(lowest_energy_mode[em] >= 0);

	__enable_irq();
}
/**
 * @brief
 *	Puts the Pearl Gecko in the lowest available sleep mode, if possible
 * @details
 *	Based on the lowest_energy_mode blocks array, checks for blocks and then picks to sleep or not
 * @note
 *	EM4 is not enabled in this implementation, as the CPU context state is lost
 **/
void enter_sleep(void)
{
	if (lowest_energy_mode[EM0] > 0 || lowest_energy_mode[EM1] > 0)
		return;
	else if (lowest_energy_mode[EM2] > 0)
		EMU_EnterEM1();
	else if (lowest_energy_mode[EM3] > 0)
		EMU_EnterEM2(true);
	else
		EMU_EnterEM3(true);
}
/**
 * @brief
 *	Returns the current lowest accessible energy mode
 * @details
 *	Iterates through lowest_energy_mode, looking for blocks
 * @note
 *	It is possible for EM4 to be returned from this, even though it is not accessible at this time
 * @returns
 * 	lowest accessible energy mode, EM0 - EM4
 **/
uint32_t current_block_energy_mode(void)
{
	for (int i = 0; i < MAX_ENERGY_MODES; i++)
		if (lowest_energy_mode[i])
			return i;
	return MAX_ENERGY_MODES - 1;
}
