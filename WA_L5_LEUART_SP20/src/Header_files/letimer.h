/**
 * @file letimer.h
 **/
#ifndef LETIMER_H
#define LETIMER_H

//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_letimer.h"
#include "sleep_routines.h"
//***********************************************************************************
// defined files
//***********************************************************************************
#define LETIMER_HZ	1000				/**< frequency for LETIMER in Hz, set to once every second **/
#define	LETIMER_REP_MAGIC_NUMBER 7		/**< generic non-zero value used to set REP registers **/
#define LETIMER_EM EM4					/**< energy block for LETIMER, keeps PG12 out of EM4 **/
//***********************************************************************************
// global variables
//***********************************************************************************
/**
 * @brief
 *  LETIMER PWM Setup Struct
 */
typedef struct {
	bool 			debugRun;			/**< keep the LETIMER running while paused in Debugger **/
	bool 			enable;				/**< enables LETIMER upon completion of open **/
	uint8_t			out_pin_route0;		/**< out 0 route to gpio port / pin **/
	uint8_t			out_pin_route1;		/**< out 1 route to gpio port / pin **/
	bool			out_pin_0_en;		/**< enable out 0 route **/
	bool			out_pin_1_en;		/**< enable out 1 route **/
	float			period;				/**< period, in seconds **/
	float			active_period;		/**< active period, in seconds **/
	bool			comp0_irq_enable;	/**< enable COMP0 interrupts **/
	uint32_t		comp0_evt;			/**< scheduler event id for COMP0 **/
	bool			comp1_irq_enable;	/**< enable COMP1 interrupts **/
	uint32_t		comp1_evt;			/**< scheduler event id for COMP1 **/
	bool			uf_irq_enable;		/**< enable UF interrupts **/
	uint32_t		uf_evt;				/**< scheduler event id for UF **/
} APP_LETIMER_PWM_TypeDef ;


//***********************************************************************************
// function prototypes
//***********************************************************************************
void letimer_pwm_open(LETIMER_TypeDef *letimer, APP_LETIMER_PWM_TypeDef *app_letimer_struct);
void letimer_start(LETIMER_TypeDef *letimer, bool enable);

#endif /* LETIMER_H */
