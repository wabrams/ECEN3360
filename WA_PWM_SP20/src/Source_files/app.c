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


//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *	Set up the peripherals.
 *
 * @details
 *	Calls open functions for the following: CMU, GPIO, LETIMER (PWM).
 *
 * @note
 *	This function does call other app functions, used to open some of the peripherals.
 *
 ******************************************************************************/
void app_peripheral_setup(void)
{
	cmu_open();
	gpio_open();
	app_letimer_pwm_open(PWM_PER, PWM_ACT_PER);
}


/***************************************************************************//**
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
 ******************************************************************************/
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

	letimer_pwm_open(LETIMER0, &letimer_pwm_struct);
}


