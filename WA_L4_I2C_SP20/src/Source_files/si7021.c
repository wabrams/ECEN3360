#include "i2c.h"
#include "si7021.h"
#include "gpio.h"
#include <stdbool.h>

void si7021_i2c_open()
{
	I2C_IO_STRUCT i2c_io_s;
	i2c_io_s.SCL_PORT = SI7021_SCL_PORT;
	i2c_io_s.SCL_PIN  = SI7021_SCL_PIN;
	i2c_io_s.SDA_PORT = SI7021_SDA_PORT;
	i2c_io_s.SDA_PIN  = SI7021_SDA_PIN;

	I2C_OPEN_STRUCT i2c_open_s;
	i2c_open_s.clhr			= SI7021_I2C_CLK_RATIO;
	i2c_open_s.enable		= false;
	i2c_open_s.freq			= SI7021_I2C_FREQ;
	i2c_open_s.master		= true;
	i2c_open_s.refFreq		= 0;
	i2c_open_s.rloc_scl		= SI7021_SCL_LOC;
	i2c_open_s.rloc_scl_en	= SI7021_SCL_EN;
	i2c_open_s.rloc_sda		= SI7021_SDA_LOC;
	i2c_open_s.rloc_sda_en	= SI7021_SDA_EN;

	i2c_open(SI7021_I2C, &i2c_open_s, &i2c_io_s);
}
