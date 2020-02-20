#include "em_i2c.h"
#include "em_gpio.h"
#include <stdbool.h>

#ifndef I2C_H
#define I2C_H

//structs
	typedef struct
	{
		// I2C_Init_TypeDef
		I2C_ClockHLR_TypeDef 	clhr;			/**< Clock low/high ratio control **/
		bool					enable;			/**< Enable I2C peripheral when init completed **/
		uint32_t				freq;			/**< I2C bus frequency to use (if master enabled) **/
		bool					master;			/**< Set to master (true) or slave (false) **/
		uint32_t				refFreq;		/**< I2C reference clock assumed when configuring bus frequency setup **/
		// Routeloc for SCL and SDA
		uint32_t 				rloc_scl;		/**< GPIO routeloc information for I2C's SCL **/
		bool					rloc_scl_en;	/**< GPIO routeloc enable for I2C's SCL **/
		uint32_t 				rloc_sda;		/**< GPIO routeloc information for I2C's SDA **/
		bool					rloc_sda_en;	/**< GPIO routeloc enable for I2C's SDA **/
	} I2C_OPEN_STRUCT;

	typedef struct
	{
		GPIO_Port_TypeDef 	SCL_PORT;		/**< I2C's SCL GPIO Port **/
		int 				SCL_PIN;		/**< I2C's SCL GPIO Pin **/
		GPIO_Port_TypeDef 	SDA_PORT;		/**< I2C's SDA GPIO Port **/
		int 				SDA_PIN;		/**< I2C's SDA GPIO Pin **/
	} I2C_IO_STRUCT;

	typedef struct
	{
	//	varType 	varName;		/**< doxygen comment **/
	} I2C_PAYLOAD_STRUCT;

// functions
	void i2c_open(I2C_TypeDef *, I2C_OPEN_STRUCT *, I2C_IO_STRUCT *);
	void i2c_bus_reset(I2C_TypeDef *, I2C_IO_STRUCT *);

#endif /* I2C_H */
