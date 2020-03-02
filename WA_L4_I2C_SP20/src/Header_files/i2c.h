/**
 * @file i2c.h
 */
#ifndef I2C_H
#define I2C_H

#include "sleep_routines.h"

#include "em_i2c.h"
#include "em_gpio.h"
#include <stdbool.h>

//defines
	#define I2C_MASTER_EM_BLOCK 	EM2		/**< I2C Master Block, used for restricting sleep in sleep_routines**/

	#define I2C_DIR_WRITE	0		/**< I2C direction write bit, to be transmitted in start byte as LSB (with device address) **/
	#define I2C_DIR_READ	1		/**< I2C direction read bit, to be transmitted in start byte as LSB (with device address) **/
	#define GENERAL_BYTE_SHIFT 8	/**< Generic definition of bits in a byte, used for shifting buffers with RXDATA **/
//enums
	/**
	 * @brief
	 * I2C State Machine Enumeration
	 **/
	enum I2C_STATEMACHINE_ENUM
	{
		I2C_STATE_IDLE,		/**< Idle state **/
		I2C_STATE_START,	/**< Send the start command **/
		I2C_STATE_CMDW,		/**< Send the write command **/
		I2C_STATE_CMDR,		/**< Send the read command **/
		I2C_STATE_RX_MSB,	/**< Receive the most significant byte from the device **/
		I2C_STATE_RX_LSB,	/**< Receive the least significant byte from the device **/
		I2C_STATE_DONE		/**< Done reading from the device **/
	};
//structs
	/**
	 * @brief
	 * Structure used for i2c_open to pass all relevant values
	 **/
	typedef struct
	{
		// I2C_Init_TypeDef
		I2C_ClockHLR_TypeDef 	clhr;			/**< Clock low/high ratio control **/
		bool					enable;			/**< Enable I2C peripheral when init completed **/
		uint32_t				freq;			/**< I2C bus frequency to use (if master enabled) **/
		bool					master;			/**< Set to master (true) or slave (false) **/
		uint32_t				refFreq;		/**< I2C reference clock assumed when configuring bus frequency setup **/
		// Route location for SCL and SDA
		uint32_t 				rloc_scl;		/**< GPIO routeloc information for I2C's SCL **/
		uint32_t				rloc_scl_en;	/**< GPIO routeloc enable for I2C's SCL **/
		uint32_t 				rloc_sda;		/**< GPIO routeloc information for I2C's SDA **/
		uint32_t				rloc_sda_en;	/**< GPIO routeloc enable for I2C's SDA **/
	} I2C_OPEN_STRUCT;
	/**
	 * @brief
	 * Structure used to pass GPIO information to I2C
	 **/
	typedef struct
	{
		GPIO_Port_TypeDef 	SCL_PORT;		/**< I2C's SCL GPIO Port **/
		int 				SCL_PIN;		/**< I2C's SCL GPIO Pin **/
		GPIO_Port_TypeDef 	SDA_PORT;		/**< I2C's SDA GPIO Port **/
		int 				SDA_PIN;		/**< I2C's SDA GPIO Pin **/
	} I2C_IO_STRUCT;
	/**
	 * @brief
	 * Structure used to retain information used for I2C
	 **/
	typedef struct
	{
		int_fast8_t	i2c_state;				/**< I2C's state machine **/
		I2C_TypeDef * i2c;					/**< I2C's i2c pointer, unused **/
		uint32_t dev_addr;					/**< Device address value **/
		uint32_t dev_cmd;					/**< Device register / command **/
		uint32_t dev_evt;					/**< Device scheduler event **/
		uint16_t * dev_buffer;				/**< Device buffer pointer **/
		bool read;							/**< I2C read or write operation **/
// 	ECEN 4593 IMPLEMENTATION
//		int_fast8_t rx_bytes;				< I2C's bytes received tracker (helps with state machine)
//		int_fast8_t rx_max;					< I2C's max bytes to receive
	} I2C_PAYLOAD_STRUCT;

// functions
	void i2c_open(I2C_TypeDef *, I2C_OPEN_STRUCT *, I2C_IO_STRUCT *);
	void i2c_bus_reset(I2C_TypeDef *, I2C_IO_STRUCT *);
	void i2c_start(I2C_TypeDef *, I2C_PAYLOAD_STRUCT *);

	void I2C0_IRQHandler(void);
	void I2C1_IRQHandler(void);

#endif /* I2C_H */
