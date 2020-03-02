/**
 * @file gpio.h
 **/

//#pragma GCC warning "outside the include guards"

#ifndef GPIO_H
#define GPIO_H

//#pragma GCC warning "inside the include guards"
//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_gpio.h"

//***********************************************************************************
// defined files
//***********************************************************************************
	// LED0
	#define	LED0_port		gpioPortF			/**< LED0's GPIO Port     **/
	#define LED0_pin		04u					/**< LED0's GPIO Pin      **/
	#define LED0_default	false				/**< LED0's Default State **/
	// LED1
	#define LED1_port		gpioPortF			/**< LED1's GPIO Port     **/
	#define LED1_pin		05u					/**< LED1's GPIO Pin      **/
	#define LED1_default	false				/**< LED1's Default State **/
	// SI7021
	#define SI7021_SCL_PORT			gpioPortC	/**< Si7021's SCL GPIO Port **/
	#define SI7021_SCL_PIN			11			/**< Si7021's SCL GPIO Pin  **/
	#define SI7021_SDA_PORT			gpioPortC	/**< Si7021's SDA GPIO Port **/
	#define SI7021_SDA_PIN			10			/**< Si7021's SDA GPIO Pin  **/
	#define SI7021_SENSOR_EN_PORT	gpioPortB	/**< Si7021's EN GPIO Port  **/
	#define SI7021_SENSOR_EN_PIN	10			/**< Si7021's EN GPIO Pin   **/

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void gpio_open(void);

#endif /* GPIO_H */
