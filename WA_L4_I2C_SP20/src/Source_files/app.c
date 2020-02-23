/**
 *
 * @file app.c
 * @author William Abrams
 * @date 28th Jan. 2020
 * @brief Application controller file, for what we're trying to do
 *
**/


//***********************************************************************************
// Include files
//***********************************************************************************
#include "app.h"
#include "letimer.h"
#include "scheduler.h"
#include "sleep_routines.h"
#include "si7021.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function
//***********************************************************************************

/**
 * @brief
 *	Set up the peripherals.
 *
 * @details
 *	Calls open functions for the following: CMU, GPIO, LETIMER (PWM).
 *
 * @note
 *	This function does call other app functions, used to open some of the peripherals.
 *
 **/
void app_peripheral_setup(void)
{
	sleep_open();
	scheduler_open();
	cmu_open();
	gpio_open();
	app_letimer_pwm_open(PWM_PER, PWM_ACT_PER);
	si7021_i2c_open();
}


/**
 * @brief
 *	Set up the LETIMER for PWM
 *
 * @details
 *	Creates an APP_LETIMER_PWM_TypeDef struct used to set up LETIMER0.
 *	Passes the struct to letimer_pwm_open() .
 *
 * @note
 *	This function should only be called from app_peripheral_setup() .
 *
 * @param[in] period
 *	The period of the off duty cycle (PWM HIGH).
 *
 * @param[in] act_period
 *	The period of the on duty cycle (PWM LOW).
 **/
void app_letimer_pwm_open(float period, float act_period)
{
	// Initializing LETIMER0 for PWM operation by creating the
	// letimer_pwm_struct and initializing all of its elements
	APP_LETIMER_PWM_TypeDef letimer_pwm_struct;
	letimer_pwm_struct.active_period = act_period;
	letimer_pwm_struct.debugRun = false;
	letimer_pwm_struct.enable = false;
	letimer_pwm_struct.out_pin_0_en = LETIMER0_OUT0_EN;
	letimer_pwm_struct.out_pin_1_en = LETIMER0_OUT1_EN;
	letimer_pwm_struct.out_pin_route0 = LETIMER0_ROUTE_OUT0;
	letimer_pwm_struct.out_pin_route1 = LETIMER0_ROUTE_OUT1;
	letimer_pwm_struct.period = period;
	// setup for scheduler / interrupts
	letimer_pwm_struct.comp0_irq_enable = letimer_pwm_struct.comp1_irq_enable = false;
	letimer_pwm_struct.uf_irq_enable = true;
	letimer_pwm_struct.comp0_evt = LETIMER0_COMP0_EVT;
	letimer_pwm_struct.comp1_evt = LETIMER0_COMP1_EVT;
	letimer_pwm_struct.uf_evt = LETIMER0_UF_EVT;
	// aaaaaand open
	letimer_pwm_open(LETIMER0, &letimer_pwm_struct);
}
/**
 * @brief
 *	Scheduled Event Handler for LETIMER0 UF
 * @details
 *	Tests energy modes EM0, EM1, EM2, and EM3 (and our sleep routines)
 * @note
 *	Does not enter EM4, as the CPU context state would be lost (hibernation vs sleep)
 **/
void scheduled_letimer0_uf_evt(void)
{
	EFM_ASSERT(get_scheduled_events() & LETIMER0_UF_EVT);
	remove_scheduled_event(LETIMER0_UF_EVT);

	uint32_t cen = current_block_energy_mode();
	sleep_unblock_mode(cen);

	if (cen < EM4)
		sleep_block_mode(cen + 1);
	else
		sleep_block_mode(EM0);
}
/**
 * @brief
 *	Scheduled Event Handler for LETIMER0 COMP0
 * @details
 *	Removes event from scheduler, asserts false
 * @note
 *	Contains EFM_ASSERT(false), as we shouldn't end up in this method
 **/
void scheduled_letimer0_comp0_evt(void)
{
//	EFM_ASSERT(get_scheduled_events() & LETIMER0_COMP0_EVT);
	remove_scheduled_event(LETIMER0_COMP0_EVT);
	EFM_ASSERT(false);
}
/**
 * @brief
 *	Scheduled Event Handler for LETIMER0 COMP1
 * @details
 *	Removes event from scheduler, asserts false
 * @note
 *	Contains EFM_ASSERT(false), as we shouldn't end up in this method
 **/
void scheduled_letimer0_comp1_evt(void)
{
//	EFM_ASSERT(get_scheduled_events() & LETIMER0_COMP1_EVT);
	remove_scheduled_event(LETIMER0_COMP1_EVT);
	EFM_ASSERT(false);
}
