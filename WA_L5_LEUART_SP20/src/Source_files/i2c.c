/**
 * @file i2c.c
 * @author William Abrams
**/

#include "em_cmu.h"
#include "i2c.h"
#include "si7021.h"
#include "sleep_routines.h"
#include "scheduler.h"

static I2C_PAYLOAD_STRUCT * i2c_payload_s;	/**< Pointer to I2C Payload Struct for the current operation **/

/**
 * @brief
 *	Opener function for the I2C Peripheral
 * @details
 *	Sets up i2c, routing, and interrupts
 * @param[in] i2c
 *	pointer to I2C0 or I2C1
 * @param[in] i2c_open_s
 *	pointer to I2C Opener Struct, used to configure I2C
 * @param[in] i2c_io_s
 *	pointer to I2C IO Struct, used for the i2c_bus_reset() function
 **/
void i2c_open(I2C_TypeDef * i2c, I2C_OPEN_STRUCT * i2c_open_s, I2C_IO_STRUCT * i2c_io_s)
{
	if (i2c == I2C0)
		CMU_ClockEnable(cmuClock_I2C0, true);
	else if (i2c == I2C1)
		CMU_ClockEnable(cmuClock_I2C1, true);

	if (i2c -> IF & 0x1)
	{
		i2c -> IFC = 0x1;
		EFM_ASSERT(!(i2c -> IF & 0x1));
	}
	else
	{
		i2c -> IFS = 0x1;
		EFM_ASSERT(i2c -> IF & 0x1);
		i2c -> IFC = 0x1;
	}

	I2C_Init_TypeDef i2c_init;
	i2c_init.clhr 		= i2c_open_s -> clhr;
	i2c_init.enable 	= i2c_open_s -> enable;
	i2c_init.freq 		= i2c_open_s -> freq;
	i2c_init.master 	= i2c_open_s -> master;
	i2c_init.refFreq 	= i2c_open_s -> refFreq;
	I2C_Init(i2c, &i2c_init);

	i2c -> ROUTELOC0 = i2c_open_s -> rloc_scl | i2c_open_s -> rloc_sda;
	i2c -> ROUTEPEN  = i2c_open_s -> rloc_scl_en | i2c_open_s -> rloc_sda_en;
	i2c_bus_reset(i2c, i2c_io_s);

	i2c -> IEN = I2C_IEN_ACK | I2C_IEN_NACK | I2C_IEN_RXDATAV | I2C_IEN_MSTOP;

	if (i2c == I2C0)
		NVIC_EnableIRQ(I2C0_IRQn);
	else if (i2c == I2C1)
		NVIC_EnableIRQ(I2C1_IRQn);

	__enable_irq(); //TODO: might want this in start instead
}

/**
 * @brief
 * 	I2C Bus Reset function
 * @details
 *  Clears the TX Buffer, Interrupt Flags, Resets the Bus, Sends Abort Command
 * @param[in] i2c
 *  Pointer to the I2C peripheral
 * @param[in] i2c_io_s
 * 	GPIO Struct for I2C, used for toggling the pins
 **/
void i2c_bus_reset(I2C_TypeDef * i2c, I2C_IO_STRUCT * i2c_io_s)
{
	EFM_ASSERT(GPIO_PinInGet(i2c_io_s -> SCL_PORT, i2c_io_s -> SCL_PIN));
	EFM_ASSERT(GPIO_PinInGet(i2c_io_s -> SDA_PORT, i2c_io_s -> SDA_PIN));

	// OPTIONAL:
	i2c -> CMD = I2C_CMD_CLEARTX; 	// Clear the TX Buffer
	i2c -> IFC = i2c -> IF;			// Clear Interrupt Flags

	// Toggle SCL 9 times, while SDA is held high
	GPIO_PinOutSet(i2c_io_s -> SDA_PORT, i2c_io_s -> SDA_PIN);

	for (int i = 0; i < 9; i++) //TODO: pragma GCC unroll n
	{
		GPIO_PinOutClear(i2c_io_s -> SCL_PORT, i2c_io_s -> SCL_PIN);
		GPIO_PinOutSet(i2c_io_s -> SCL_PORT, i2c_io_s -> SCL_PIN);
	}

	i2c -> CMD = I2C_CMD_ABORT;		// Send the I2C Abort Command
}

/**
 * @brief
 *	Start function for I2C
 * @details
 *	Sends the start command, and device address with the appropriate read / write bit
 * @param[in] i2c
 *	Pointer to the I2C Peripheral
 * @param[in] i2c_pl_s
 *	Pointer to the I2C Payload Struct, containing all necessary info for protocol
 * @note
 * 	contains a call to sleep_block_mode()
 **/
void i2c_start(I2C_TypeDef * i2c, I2C_PAYLOAD_STRUCT * i2c_pl_s)
{
	EFM_ASSERT((i2c -> STATE & _I2C_STATE_STATE_MASK) == I2C_STATE_STATE_IDLE);
	sleep_block_mode(I2C_MASTER_EM_BLOCK);

	i2c_payload_s = i2c_pl_s;
	i2c_payload_s -> i2c_state = I2C_STATE_START;
	i2c -> CMD = I2C_CMD_START;
	i2c -> TXDATA = (i2c_payload_s -> dev_addr << 1) | I2C_DIR_WRITE;
}

/**
 * @brief
 *	ACK Handler function for I2Cn IRQHandler
 * @details
 *	called by I2Cn IRQHandler, and will perform actions based on the I2C State in the payload struct
 * @param[in] i2c
 * 	pointer to I2C0 or I2C1
 **/
static void i2c_ack(I2C_TypeDef * i2c)
{
	switch(i2c_payload_s -> i2c_state)
	{
		case I2C_STATE_START:
			//device is open and ready for measurement
			i2c_payload_s -> i2c_state = I2C_STATE_CMDW; //TODO: i2c_state++ instead of set?
			i2c -> TXDATA = i2c_payload_s -> dev_cmd;
			break;
		case I2C_STATE_CMDW:
			//device measurement command received, ask if ready to tx
			i2c_payload_s -> i2c_state = I2C_STATE_CMDR;
			i2c -> CMD = I2C_CMD_START;
			i2c -> TXDATA = (i2c_payload_s -> dev_addr << 1) | I2C_DIR_READ;
			break;
		case I2C_STATE_CMDR:
			//device is sending MSByte
			i2c_payload_s -> i2c_state = I2C_STATE_RX_MSB;
			break;
		case I2C_STATE_RX_MSB:
			//device is sending LSByte
			i2c_payload_s -> i2c_state = I2C_STATE_RX_LSB;
			break;
		default:
			EFM_ASSERT(false);
			break;
	}
}

/**
 * @brief
 *	NACK Handler function for I2Cn IRQHandler
 * @details
 *	called by I2Cn IRQHandler, and will perform actions based on the I2C State in the payload struct
 * @param[in] i2c
 * 	pointer to I2C0 or I2C1
 **/
static void i2c_nack(I2C_TypeDef * i2c)
{
	switch(i2c_payload_s -> i2c_state)
	{
		case I2C_STATE_CMDR:
			//conversion not complete, ask again
			i2c -> CMD = I2C_CMD_START;
			i2c -> TXDATA = (i2c_payload_s -> dev_addr << 1) | I2C_DIR_READ;
			break;
		default:
			EFM_ASSERT(false);
			break;
	}
}

/**
 * @brief
 *	RXDATAV Handler function for I2Cn IRQHandler
 * @details
 *	called by I2Cn IRQHandler, and will perform actions based on the I2C State in the payload struct
 * @param[in] i2c
 * 	pointer to I2C0 or I2C1
 **/
static void i2c_rxdatav(I2C_TypeDef * i2c)
{
	switch(i2c_payload_s -> i2c_state)
	{
		case I2C_STATE_RX_MSB:
			*(i2c_payload_s -> dev_buffer) <<= GENERAL_BYTE_SHIFT;
			*(i2c_payload_s -> dev_buffer) |= i2c -> RXDATA;
			i2c_payload_s -> i2c_state = I2C_STATE_RX_LSB;
			i2c -> CMD = I2C_CMD_ACK;
			break;
		case I2C_STATE_RX_LSB:
			*(i2c_payload_s -> dev_buffer) <<= GENERAL_BYTE_SHIFT;
			*(i2c_payload_s -> dev_buffer) |= i2c -> RXDATA;
			i2c_payload_s -> i2c_state = I2C_STATE_DONE;
			i2c -> CMD = I2C_CMD_NACK | I2C_CMD_STOP;
			break;
		default:
			EFM_ASSERT(false);
	}
}

/**
 * @brief
 *	MSTOP Handler function for I2Cn IRQHandler
 * @details
 *	called by I2Cn IRQHandler, and will perform actions based on the I2C State in the payload struct
 * @param[in] i2c
 * 	pointer to I2C0 or I2C1
 **/
static void i2c_mstop(I2C_TypeDef * i2c)
{
	switch(i2c_payload_s -> i2c_state)
	{
		case I2C_STATE_DONE:
			i2c_payload_s -> i2c_state = I2C_STATE_IDLE;
			add_scheduled_event(i2c_payload_s -> dev_evt);
			sleep_unblock_mode(I2C_MASTER_EM_BLOCK);
			break;
		default:
			EFM_ASSERT(false);
			break;
	}
}

/**
 * @brief
 * 	I2C0's IRQ Handler
 * @details
 * 	Clears interrupt flags, handles enabled interrupts
 **/
void I2C0_IRQHandler(void)
{
	__disable_irq();

	uint32_t iflags = I2C0 -> IF & I2C0 -> IEN;
	I2C0 -> IFC = I2C0 -> IF;

	if (iflags & I2C_IF_ACK)
		i2c_ack(I2C0);
	if (iflags & I2C_IF_NACK)
		i2c_nack(I2C0);
	if (iflags & I2C_IF_RXDATAV)
		i2c_rxdatav(I2C0);
	if (iflags & I2C_IF_MSTOP)
		i2c_mstop(I2C0);

	__enable_irq();
}

/**
 * @brief
 * 	I2C1's IRQ Handler
 * @details
 * 	Clears interrupt flags, handles enabled interrupts
 **/
void I2C1_IRQHandler(void)
{
	__disable_irq();
	//TODO: change to concurrent assignment to flex on DDL peers and TAs
	uint32_t iflags = I2C1 -> IF & I2C1 -> IEN;
	I2C1 -> IFC = I2C1 -> IF;

	if (iflags & I2C_IF_ACK)
		i2c_ack(I2C1);
	if (iflags & I2C_IF_NACK)
		i2c_nack(I2C1);
	if (iflags & I2C_IF_RXDATAV)
		i2c_rxdatav(I2C1);
	if (iflags & I2C_IF_MSTOP)
		i2c_mstop(I2C1);

	__enable_irq();
}
