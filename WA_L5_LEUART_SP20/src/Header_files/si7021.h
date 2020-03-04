#ifndef SI7021_H
#define SI7021_H

#include "em_i2c.h"

#define SI7021_DEV_ADDR				0x40						/**< Si7021 Device Address **/
#define SI7021_TEMP_NO_HOLD			0xF3						/**< Register Address for measure temp, no hold **/
#define SI7021_I2C_FREQ				I2C_FREQ_STANDARD_MAX		/**< Si7021 I2C frequency **/
#define SI7021_I2C_CLK_RATIO		I2C_CTRL_CLHR_STANDARD		/** Clock Ratio, same as i2cClockHLRStandard **/
#define SI7021_SCL_EN				I2C_ROUTEPEN_SCLPEN			/**< I2C SCL enable **/
#define SI7021_SDA_EN				I2C_ROUTEPEN_SDAPEN			/**< I2C SDA enable **/
#define SI7021_I2Cn					1							/**< Preprocessor MUX Control for I2C0 I2C1 selection **/
#if SI7021_I2Cn == 0
	#define SI7021_I2C 				I2C0						/**< Si7021 set to use I2C0 **/
	#define SI7021_SCL_LOC			I2C_ROUTELOC0_SCLLOC_LOC15	/**< I2C0 SCL route location info **/
	#define SI7021_SDA_LOC			I2C_ROUTELOC0_SDALOC_LOC15	/**< I2C0 SDA route location info **/
#elif SI7021_I2Cn == 1
	#define SI7021_I2C 				I2C1						/**< Si7021 set to use I2C1 **/
	#define SI7021_SCL_LOC			I2C_ROUTELOC0_SCLLOC_LOC19	/**< I2C1 SCL route location info **/
	#define SI7021_SDA_LOC			I2C_ROUTELOC0_SDALOC_LOC19	/**< I2C1 SDA route location info **/
#endif

//function prototypes
void si7021_i2c_open();
void si7021_i2c_start();
float si7021_temp_C();
float si7021_temp_F();

#endif /* SI7021_H */
