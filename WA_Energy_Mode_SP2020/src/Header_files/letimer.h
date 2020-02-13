//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_letimer.h"
#include "sleep_routines.h"
//***********************************************************************************
// defined files
//***********************************************************************************
#define LETIMER_HZ	1000				// Utilizing ULFRCO oscillator for LETIMERs
#define	LETIMER_REP_MAGIC_NUMBER 7
#define LETIMER_EM EM4
//***********************************************************************************
// global variables
//***********************************************************************************
typedef struct {
	bool 			debugRun;			// True = keep LETIMER running will halted
	bool 			enable;				// enable the LETIMER upon completion of open
	uint8_t			out_pin_route0;		// out 0 route to gpio port/pin
	uint8_t			out_pin_route1;		// out 1 route to gpio port/pin
	bool			out_pin_0_en;		// enable out 0 route
	bool			out_pin_1_en;		// enable out 1 route
	float			period;				// seconds
	float			active_period;		// seconds
	bool			comp0_irq_enable;
	uint32_t		comp0_evt;
	bool			comp1_irq_enable;
	uint32_t		comp1_evt;
	bool			uf_irq_enable;
	uint32_t		uf_evt;
} APP_LETIMER_PWM_TypeDef ;


//***********************************************************************************
// function prototypes
//***********************************************************************************
void letimer_pwm_open(LETIMER_TypeDef *letimer, APP_LETIMER_PWM_TypeDef *app_letimer_struct);
void letimer_start(LETIMER_TypeDef *letimer, bool enable);
