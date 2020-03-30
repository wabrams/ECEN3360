/**
 * @file app.h
 **/
#ifndef APP_H
#define APP_H
//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_cmu.h"
#include "em_prs.h"
#include "cmu.h"
#include "gpio.h"

//***********************************************************************************
// Macro Definitions
//***********************************************************************************
	// LETIMER0 PWM LED Setup
		#define	PWM_PER				10.0								/**< PWM period in seconds **/
		#define	PWM_ACT_PER			0.10							/**< PWM active period in seconds **/
		#define	LETIMER0_ROUTE_OUT0	LETIMER_ROUTELOC0_OUT0LOC_LOC28	/**< Routing for LETIMER ROUTE OUT0 (to LED0) **/
		#define	LETIMER0_OUT0_EN	false							/**< set to true for LED0 to blink, false for off **/
		#define	LETIMER0_ROUTE_OUT1	0								/**< Routing for  LETIMER ROUTE OUT1 (unused) **/
		#define	LETIMER0_OUT1_EN	false							/**< unused, ignore value **/
	// I2C Definitions
		#define TEMP_THRESHOLD		85.0							/**< Temperature Threshold, to either turn on or off LED1 as with the scheduled_i2c_si7021_evt() **/
	// Scheduler Event IDs
		#define LETIMER0_COMP0_EVT		0x00000001 /**< Scheduler Event ID for LETIMER0_COMP0_EVT  **/
		#define LETIMER0_COMP1_EVT		0x00000002 /**< Scheduler Event ID for LETIMER0_COMP1_EVT  **/
		#define LETIMER0_UF_EVT			0x00000004 /**< Scheduler Event ID for LETIMER0_UF_EVT     **/
		#define I2C_SI7021_EVT			0x00000008 /**< Scheduler Event ID for I2C_DONE_EVT        **/
		#define LEUART_RX_DONE_EVT		0x00000010 /**< Scheduler Event ID for LEUART0_RX_DONE_EVT **/
		#define LEUART_TX_DONE_EVT		0x00000020 /**< Scheduler Event ID for LEUART0_TX_DONE_EVT **/
		#define BOOT_UP_EVT				0x80000000 /**< Scheduler Event ID for BOOT_UP_EVT (MAX)   **/

//	typedef enum
//	{
//		EVT_LETIMER0_COMP0	= 1 << 0,
//		EVT_LETIMER0_COMP1	= 1 << 1,
//		EVT_LETIMER0_UF		= 1 << 2,
//		EVT_I2C_SI7021		= 1 << 3,
//		EVT_LEUART_RX_DONE	= 1 << 4,
//		EVT_LEUART_TX_DONE	= 1 << 5,
//		EVT_BOOT_UP_EVT		= 1 << 6
//	} scheduler_t;
//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void app_peripheral_setup(void);
void app_letimer_pwm_open(float period, float act_period);
void scheduled_letimer0_uf_evt(void);
void scheduled_letimer0_comp0_evt(void);
void scheduled_letimer0_comp1_evt(void);
void scheduled_i2c_si7021_evt(void);
void scheduled_leuart_rx_done_evt(void);
void scheduled_leuart_tx_done_evt(void);
void scheduled_boot_up_evt(void);

#endif /* APP_H */
