/**
 *
 * @file gpio.c
 * @author William Abrams
 * @date 28th Jan. 2020
 * @brief GPIO Controller File
 *
**/

//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"
#include "em_cmu.h"
#include <stdbool.h>

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************


//***********************************************************************************
// functions
//***********************************************************************************
/**
 * @brief
 *	Initialization of the GPIO for any required peripherals / components
 * @details
 *	Enables needed pins and sets the drive strength
 * @note
 *	GPIO = General Purpose Input / Output
 **/
void gpio_open(void)
{
	CMU_ClockEnable(cmuClock_GPIO, true);

	// LED 0
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);
	// LED 1
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);
	// SI7021 EN
	GPIO_DriveStrengthSet(SI7021_SENSOR_EN_PORT, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(SI7021_SENSOR_EN_PORT, SI7021_SENSOR_EN_PIN, gpioModePushPull, true);
	// SI7021 SCL and SDA
	GPIO_PinModeSet(SI7021_SCL_PORT, SI7021_SCL_PIN, gpioModeWiredAnd, true);
	GPIO_PinModeSet(SI7021_SDA_PORT, SI7021_SDA_PIN, gpioModeWiredAnd, true);
}
