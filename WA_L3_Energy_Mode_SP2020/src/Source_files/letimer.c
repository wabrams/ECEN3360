/**
 * @file letimer.c
 * @author William Abrams
 * @date January 12th, 2020
 * @brief Contains all the LETIMER driver functions
 */

//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Libraries

//** Silicon Lab include files
#include "em_cmu.h"
#include "em_assert.h"

//** User/developer include files
#include "letimer.h"
#include "scheduler.h"
#include "sleep_routines.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// private variables
//***********************************************************************************
static uint32_t scheduled_comp0_evt;
static uint32_t scheduled_comp1_evt;
static uint32_t scheduled_uf_evt;

//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Driver to open an set an LETIMER peripheral in PWM mode
 *
 * @details
 * 	 This routine is a low level driver.  The application code calls this function
 * 	 to open one of the LETIMER peripherals for PWM operation to directly drive
 * 	 GPIO output pins of the device and/or create interrupts that can be used as
 * 	 a system "heart beat" or by a scheduler to determine whether any system
 * 	 functions need to be serviced.
 *
 * @note
 *   This function is normally called once to initialize the peripheral and the
 *   function letimer_start() is called to turn-on or turn-off the LETIMER PWM
 *   operation.
 *
 * @param[in] letimer
 *   Pointer to the base peripheral address of the LETIMER peripheral being opened
 *
 * @param[in] app_letimer_struct
 *   Is the STRUCT that the calling routine will use to set the parameters for PWM
 *   operation
 *
 ******************************************************************************/
void letimer_pwm_open(LETIMER_TypeDef * letimer, APP_LETIMER_PWM_TypeDef *app_letimer_struct)
{
	LETIMER_Init_TypeDef letimer_pwm_values;
	/*  Enable the routed clock to the LETIMER0 peripheral */
	if (letimer == LETIMER0)
		CMU_ClockEnable(cmuClock_LETIMER0, true);

	letimer_start(letimer, false);
	/* Use EFM_ASSERT statements to verify whether the LETIMER clock tree is properly
	 * configured and enabled
	 */
	letimer -> CMD = LETIMER_CMD_START;
	while (letimer -> SYNCBUSY);
	EFM_ASSERT(letimer -> STATUS & LETIMER_STATUS_RUNNING);
	letimer -> CMD = LETIMER_CMD_STOP;

	// Initialize letimer for PWM operation
	letimer_pwm_values.bufTop = false;	// Comp1 will not be used to load comp0, but used to create an on-time/duty cycle
	letimer_pwm_values.comp0Top = true;	// load comp0 into cnt register when count register underflows enabling continuous looping
	letimer_pwm_values.debugRun = app_letimer_struct -> debugRun;
	letimer_pwm_values.enable = app_letimer_struct -> enable;
	letimer_pwm_values.out0Pol = 0;	// While PWM is not active out, idle is DEASSERTED, 0
	letimer_pwm_values.out1Pol = 0;	// While PWM is not active out, idle is DEASSERTED, 0
	letimer_pwm_values.repMode = letimerRepeatFree;	// Setup letimer for free running for continuous looping
	letimer_pwm_values.ufoa0 = letimerUFOAPwm; //see enum LETIMER_UFOA_TypeDef, we want PWM
	letimer_pwm_values.ufoa1 = letimerUFOAPwm;

	LETIMER_Reset(letimer); // Reset letimer
	LETIMER_Init(letimer, &letimer_pwm_values);		// Initialize letimer
	while (letimer -> SYNCBUSY);
	/* Calculate the value of COMP0 and COMP1 and load these control registers
	 * with the calculated values
	 */
	letimer -> COMP0 = app_letimer_struct -> period * LETIMER_HZ;
	letimer -> COMP1 = app_letimer_struct -> active_period * LETIMER_HZ;
	/* Set the REP0 mode bits for PWM operation
	 * Use the values from app_letimer_struct input argument for ROUTELOC0 and ROUTEPEN enable
	 */
	letimer -> REP0 = LETIMER_REP_MAGIC_NUMBER; // any nonzero
	letimer -> REP1 = LETIMER_REP_MAGIC_NUMBER; // not needed, set it up so it is enabled
	letimer -> ROUTELOC0 = app_letimer_struct -> out_pin_route0 | app_letimer_struct -> out_pin_route1;
	letimer -> ROUTEPEN = (app_letimer_struct -> out_pin_0_en) | (app_letimer_struct -> out_pin_1_en << 1);

	/* We are enabling interrupts at this time. enable them now */
	letimer -> IFC = letimer -> IF; //clear interrupts
	letimer -> IEN = 0
					| (app_letimer_struct -> comp0_irq_enable * LETIMER_IEN_COMP0)
					| (app_letimer_struct -> comp1_irq_enable * LETIMER_IEN_COMP1)
					| (app_letimer_struct -> uf_irq_enable * LETIMER_IEN_UF);
	while (letimer -> SYNCBUSY);

	scheduled_comp0_evt = app_letimer_struct -> comp0_evt;
	scheduled_comp1_evt = app_letimer_struct -> comp1_evt;
	scheduled_uf_evt = app_letimer_struct -> uf_evt;
	/* We will not enable the LETIMER0 at this time */
	NVIC_EnableIRQ(LETIMER0_IRQn);

	if (letimer -> STATUS & LETIMER_STATUS_RUNNING)
		sleep_block_mode(LETIMER_EM);
}

/***************************************************************************//**
 * @brief
 *   Used to enable, turn-on, or disable, turn-off, the LETIMER peripheral
 *
 * @details
 * 	 This function allows the application code to initialize the LETIMER
 * 	 peripheral separately to enabling or disabling the LETIMER
 *
 * @note
 *   Application code should not directly access hardware resources.  The
 *   application program should access the peripherals through the driver
 *   functions
 *
 * @param[in] letimer
 *   Pointer to the base peripheral address of the LETIMER peripheral being
 *   enabled or disable
 *
 * @param[in] enable
 *   true enables the LETIMER to start operation while false disables the
 *   LETIMER
 *
 ******************************************************************************/
void letimer_start(LETIMER_TypeDef * letimer, bool enable)
{
	while (letimer -> SYNCBUSY);

	if (enable && !(letimer -> STATUS & LETIMER_STATUS_RUNNING))
		sleep_block_mode(LETIMER_EM);
	if (!enable && (letimer -> STATUS & LETIMER_STATUS_RUNNING))
		sleep_unblock_mode(LETIMER_EM);

	LETIMER_Enable(letimer, enable);
	while (letimer -> SYNCBUSY);
}
/***************************************************************************//**
 * @brief
 *	Interrupt Routine for LETIMER0
 *
 * @details
 *	Definition of LETIMER0_IRQHandler, clears all interrupts and handles defined interrupts
 *
 * @note
 *	This IRQHandler can only handle COMP0, COMP1, and UF events
 ******************************************************************************/
void LETIMER0_IRQHandler(void)
{
	__disable_irq();

	uint32_t int_flag = LETIMER0 -> IF & LETIMER0 -> IEN;
	LETIMER0 -> IFC = int_flag;

	if (int_flag & LETIMER_IF_COMP0)
	{
		add_scheduled_event(scheduled_comp0_evt);
		EFM_ASSERT(!(LETIMER0 -> IF & LETIMER_IF_COMP0));
	}
	if (int_flag & LETIMER_IF_COMP1)
	{
		add_scheduled_event(scheduled_comp1_evt);
		EFM_ASSERT(!(LETIMER0 -> IF & LETIMER_IF_COMP1));
	}
	if (int_flag & LETIMER_IF_UF)
	{
		add_scheduled_event(scheduled_uf_evt);
		EFM_ASSERT(!(LETIMER0 -> IF & LETIMER_IF_UF));
	}

	__enable_irq();
}
