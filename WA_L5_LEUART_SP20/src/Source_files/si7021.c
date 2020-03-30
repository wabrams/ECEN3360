/**
 * @file si7021.c
 * @brief Si7021 Device Driver
 **/

#include "i2c.h"
#include "si7021.h"
#include "gpio.h"
#include "app.h"
#include <stdbool.h>

static uint16_t rx_buffer;				/**< Si7021's receiving buffer, holds a single raw temperature reading **/
static I2C_PAYLOAD_STRUCT i2c_pl_s;		/**< Si7021's I2C payload struct, to be passed and used by I2C **/

/**
 * @brief
 *	Opener function for I2C, to configure it for Si7021
 * @details
 *	creates instances of I2C_IO_STRUCT and I2C_OPEN_STRUCT and passes them to i2c_open()
 **/
void si7021_i2c_open()
{
	I2C_IO_STRUCT i2c_io_s;
	i2c_io_s.SCL_PORT = SI7021_SCL_PORT;
	i2c_io_s.SCL_PIN  = SI7021_SCL_PIN;
	i2c_io_s.SDA_PORT = SI7021_SDA_PORT;
	i2c_io_s.SDA_PIN  = SI7021_SDA_PIN;

	I2C_OPEN_STRUCT i2c_open_s;
	i2c_open_s.clhr			= SI7021_I2C_CLK_RATIO;
	i2c_open_s.enable		= true;
	i2c_open_s.freq			= SI7021_I2C_FREQ;
	i2c_open_s.master		= true;
	i2c_open_s.refFreq		= 0;
	i2c_open_s.rloc_scl		= SI7021_SCL_LOC;
	i2c_open_s.rloc_scl_en	= SI7021_SCL_EN;
	i2c_open_s.rloc_sda		= SI7021_SDA_LOC;
	i2c_open_s.rloc_sda_en	= SI7021_SDA_EN;

	i2c_open(SI7021_I2C, &i2c_open_s, &i2c_io_s);
}

/**
 * @brief
 * 	Start function for I2C, to use it for the Si7021
 * @details
 * 	configures the static I2C_PAYLOAD_STRUCT, and passes it to i2c_start()
 * @note
 * 	si7021_i2c_open() must be called before using this function
 **/
void si7021_i2c_start()
{
	i2c_pl_s.dev_addr = SI7021_DEV_ADDR;
	i2c_pl_s.dev_buffer = &rx_buffer;
	i2c_pl_s.dev_cmd = SI7021_TEMP_NO_HOLD;
	i2c_pl_s.dev_evt = I2C_SI7021_EVT;
	i2c_pl_s.i2c = SI7021_I2C;
	i2c_pl_s.i2c_state = I2C_STATE_IDLE;
	i2c_pl_s.read = true;

	i2c_start(SI7021_I2C, &i2c_pl_s);
}

void si7021_lpm_enable()
{
	// Turn On Power
	GPIO_PinModeSet(SI7021_SENSOR_EN_PORT, SI7021_SENSOR_EN_PIN, gpioModePushPull, true);
	//TODO: wait for bootup delay
	// Engage GPIO
	GPIO_PinModeSet(SI7021_SCL_PORT, SI7021_SCL_PIN, gpioModeWiredAnd, true);
	GPIO_PinModeSet(SI7021_SDA_PORT, SI7021_SDA_PIN, gpioModeWiredAnd, true);
	// Engage Peripheral
	i2c_enable_bussigs(SI7021_I2C);
//	i2c_enable_interrupts(SI7021_I2C);
}

void si7021_lpm_disable()
{
	// Disengage Peripheral
	i2c_disable_interrupts(SI7021_I2C);
	i2c_disable_bussigs(SI7021_I2C);
	// Disengage GPIO
	GPIO_PinModeSet(SI7021_SCL_PORT, SI7021_SCL_PIN, gpioModeDisabled, false);
	GPIO_PinModeSet(SI7021_SDA_PORT, SI7021_SDA_PIN, gpioModeDisabled, false);
	// Turn Off Power
	GPIO_PinModeSet(SI7021_SENSOR_EN_PORT, SI7021_SENSOR_EN_PIN, gpioModeDisabled, false);
}

/**
 * @brief
 *	Getter for the Si7021's temperature reading, in Celsius
 * @details
 *	converts the raw rx_data to Celsius
 * @returns
 *	temperature in Celsius, to the tenth of a degree
 **/
float si7021_temp_C()
{
	float tempC = (175.72 * (float)rx_buffer / 65536) - 46.85;
	return (float)((int)(tempC*10))/10;
}

/**
 * @brief
 *	Getter for the Si7021's temperature reading, in Fahrenheit
 * @details
 *	converts the raw rx_data to Fahrenheit
 * @returns
 *	temperature in Fahrenheit, to the tenth of a degree
 **/
float si7021_temp_F()
{
	float tempF = (316.296 * (float)rx_buffer / 65536) - 52.33;
	return (float)((int)(tempF*10))/10;
}

