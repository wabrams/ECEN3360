#include "em_cmu.h"
#include "i2c.h"

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
}

static void i2c_ack(I2C_TypeDef * i2c)
{
	EFM_ASSERT(false);
}

static void i2c_nack(I2C_TypeDef * i2c)
{

}

static void i2c_rxdatav(I2C_TypeDef * i2c)
{

}

void I2C0_IRQHandler(void)
{
	__disable_irq();

	uint32_t iflags = I2C0 -> IF;
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
	{

	}
	__enable_irq();
}

void I2C1_IRQHandler(void)
{
	uint32_t iflags = I2C0 -> IF;
	I2C0 -> IFC = iflags;

	//TODO: copy I2C0_IRQHandler code here

	__enable_irq();
}
