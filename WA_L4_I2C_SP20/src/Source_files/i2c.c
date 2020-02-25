/**
 * @file i2c.c
 * @author William Abrams
**/

#include "em_cmu.h"
#include "i2c.h"
#include "si7021.h"
#include "sleep_routines.h"

static I2C_PAYLOAD_STRUCT i2c_payload_s;

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
}

void i2c_bus_reset(I2C_TypeDef * i2c, I2C_IO_STRUCT * i2c_io_s)
{
	EFM_ASSERT(GPIO_PinInGet(i2c_io_s -> SCL_PORT, i2c_io_s -> SCL_PIN));
	EFM_ASSERT(GPIO_PinInGet(i2c_io_s -> SDA_PORT, i2c_io_s -> SDA_PIN));

	// OPTIONAL:
	i2c -> CMD = I2C_CMD_CLEARTX; 	// Clear the TX Buffer
	i2c -> IFC = i2c -> IF;			// Clear Interrupt Flags

	// Toggle SCL 9 times, while SDA is held high
	GPIO_PinOutSet(i2c_io_s -> SDA_PORT, i2c_io_s -> SDA_PIN);
	int i = 0;
	for (i = 0; i < 9; i++)
	{
		GPIO_PinOutClear(i2c_io_s -> SCL_PORT, i2c_io_s -> SCL_PIN);
		GPIO_PinOutSet(i2c_io_s -> SCL_PORT, i2c_io_s -> SCL_PIN);
	}

	i2c -> CMD = I2C_CMD_ABORT;		// Send the I2C Abort Command

	i2c_payload_s.i2c_state = I2C_STATE_IDLE;
//	i2c_payload_s.rx_bytes = 0;
	i2c_payload_s.rx_buffer = 0;
}

static void i2c_ack(I2C_TypeDef * i2c)
{
	switch(i2c_payload_s.i2c_state)
	{
		case I2C_STATE_START:
			//device is open and ready for measurement
			i2c_payload_s.i2c_state = I2C_STATE_CMDW;
			i2c -> TXDATA = SI7021_TEMP_NO_HOLD;
			break;
		case I2C_STATE_CMDW:
			//device measurement command received, ask if ready to tx
			i2c_payload_s.i2c_state = I2C_STATE_CMDR;
			i2c -> CMD = I2C_CMD_START;
			i2c -> TXDATA = (SI7021_DEV_ADDR << 1) | I2C_DIR_READ;
			break;
		case I2C_STATE_CMDR:
			//device is sending MSByte
			i2c_payload_s.i2c_state = I2C_STATE_RX_MSB;
			break;
		case I2C_STATE_RX_MSB:
			//device is sending LSByte
			i2c_payload_s.i2c_state = I2C_STATE_RX_LSB;
			break;
		case I2C_STATE_RX_LSB:
			//we shouldn't be here
			break;
		case I2C_STATE_DONE:
			//set to idle, unblock sleep
			break;

	}
}

static void i2c_nack(I2C_TypeDef * i2c)
{
	switch(i2c_payload_s.i2c_state)
	{
		case I2C_STATE_START:
			//major fatal error
			EFM_ASSERT(false);
			break;
		case I2C_STATE_CMDW:
			//device could be busy, send again
			i2c -> TXDATA = SI7021_TEMP_NO_HOLD;
			break;
		case I2C_STATE_CMDR:
			//conversion not complete, ask again
			i2c -> CMD = I2C_CMD_START;
			i2c -> TXDATA = (SI7021_DEV_ADDR << 1) | I2C_DIR_READ;
			break;
		case I2C_STATE_RX_MSB:
			//critical error
			EFM_ASSERT(false);
			break;
		case I2C_STATE_RX_LSB:
			//critical error
			EFM_ASSERT(false);
			break;
		case I2C_STATE_DONE:
			break;
	}
}

static void i2c_rxdatav(I2C_TypeDef * i2c)
{
	switch(i2c_payload_s.i2c_state)
	{
		case I2C_STATE_RX_MSB:
			i2c_payload_s.rx_buffer <<= GENERAL_BYTE_SHIFT;
			i2c_payload_s.rx_buffer |= GENERAL_BYTE_MASK & i2c -> RXDATA;
			i2c_payload_s.i2c_state = I2C_STATE_RX_LSB;
			i2c -> CMD = I2C_CMD_ACK;
			break;
		case I2C_STATE_RX_LSB:
			i2c_payload_s.rx_buffer <<= GENERAL_BYTE_SHIFT;
			i2c_payload_s.rx_buffer |= GENERAL_BYTE_MASK & i2c -> RXDATA;
			i2c_payload_s.i2c_state = I2C_STATE_RX_LSB;
			i2c_payload_s.i2c_state = I2C_STATE_DONE;
			i2c -> CMD = I2C_CMD_NACK | I2C_CMD_STOP;
			break;
		//TODO: shouldn't be here...
//		default:
//			EFM_ASSERT(false);
	}
}

void I2C0_IRQHandler(void)
{
	__disable_irq();

	uint32_t iflags = I2C0 -> IF & I2C0 -> IEN;
	I2C0 -> IFC = iflags;

	if (iflags & I2C_IF_ACK)
	{
		i2c_ack(I2C0);
	}
	if (iflags & I2C_IF_NACK)
	{
		i2c_nack(I2C0);
	}
	if (iflags & I2C_IF_RXDATAV)
	{
		i2c_rxdatav(I2C0);
	}
	if (iflags & I2C_IF_MSTOP)
	{	//we can be sneaky and send this to the ack handler if we so desire

	}
	__enable_irq();
}

void I2C1_IRQHandler(void)
{
	uint32_t iflags = I2C1 -> IF & I2C1 -> IEN;
	I2C0 -> IFC = iflags;

	//TODO: copy I2C0_IRQHandler code here, replace I2C0 with I2C1

	__enable_irq();
}
