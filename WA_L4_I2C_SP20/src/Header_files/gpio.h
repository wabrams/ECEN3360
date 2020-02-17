/**
 * @file gpio.h
 **/

//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_gpio.h"

//***********************************************************************************
// defined files
//***********************************************************************************
	// LED0
	#define	LED0_port		gpioPortF	/**< LED0's GPIO Port     **/
	#define LED0_pin		04u			/**< LED0's GPIO Pin      **/
	#define LED0_default	false		/**< LED0's Default State **/
	// LED1
	#define LED1_port		gpioPortF	/**< LED1's GPIO Port     **/
	#define LED1_pin		05u			/**< LED1's GPIO Pin      **/
	#define LED1_default	false		/**< LED1's Default State **/
	// SI7021
	#define SI7021_SCL_PORT			gpioPortC
	#define SI7021_SCL_PIN			11
	#define SI7021_SDA_PORT			gpioPortC
	#define SI7021_SDA_PIN			10
	#define SI7021_SENSOR_EN_PORT	gpioPortB
	#define SI7021_SENSOR_EN_PIN	10

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void gpio_open(void);

