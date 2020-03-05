
#include <string.h>
#include <stdbool.h>
#include "em_gpio.h"
#include "em_cmu.h"
#include "leuart.h"
#include "scheduler.h"

uint32_t	rx_done_evt;
uint32_t	tx_done_evt;

uint32_t	str_ptr;
uint32_t	max_len;
char		output_str[80];
bool		leuart0_tx_busy;

void leuart_open(LEUART_TypeDef * leuart, LEUART_OPEN_STRUCT * leuart_settings)
{
	// Enable LEUART0 Clock
	CMU_ClockEnable(cmuClock_LEUART0, true);
	// Verify Clock Tree
	if ((leuart -> STARTFRAME & 0x01) == 0) //LSB not set
	{
		leuart -> STARTFRAME |= 0x01;
		while (leuart -> SYNCBUSY & LEUART_SYNCBUSY_STARTFRAME);
		EFM_ASSERT(leuart -> STARTFRAME & 0x01);
		leuart -> STARTFRAME &= ~0x01;
	}
	else //LSB set
	{
		leuart -> STARTFRAME &= ~0x01;
		while (leuart -> SYNCBUSY & LEUART_SYNCBUSY_STARTFRAME);
		EFM_ASSERT(!(leuart -> STARTFRAME & 0x01));
		leuart -> STARTFRAME |= 0x01;
	}
	// LEUART Initialization
	LEUART_Init_TypeDef leuart_init_s;
	leuart_init_s.baudrate = leuart_settings -> baudrate;
	leuart_init_s.databits = leuart_settings -> databits;
	leuart_init_s.enable   = leuart_settings -> enable;
	leuart_init_s.parity   = leuart_settings -> parity;
	leuart_init_s.refFreq  = leuart_settings -> refFreq;
	leuart_init_s.stopbits = leuart_settings -> stopbits;
	LEUART_Init(leuart, &leuart_init_s);
	//TODO: see if syncbusy wait is actually needed here (lab says to)
	while(leuart -> SYNCBUSY);

	// LEUART Routing Setup
	leuart -> ROUTELOC0 = leuart_settings -> rx_rloc | leuart_settings -> tx_rloc;
	leuart -> ROUTEPEN = leuart_settings -> rx_rpen | leuart_settings -> tx_rpen;

	// MISC Setup
	leuart -> CMD = (LEUART_CMD_RXBLOCKEN * leuart_settings -> rxblocken) | (LEUART_CMD_RXEN * leuart_settings -> rx_en) | (LEUART_CMD_TXEN * leuart_settings -> tx_en);
	leuart -> CTRL = leuart_settings -> sfubrx * LEUART_CTRL_SFUBRX;

	// Setup for Start Frame
	leuart -> STARTFRAME = leuart_settings -> startframe;

	// Setup for Signal Frame
	leuart -> SIGFRAME = leuart_settings -> sigframe;


	// Sync Up for CMD
	while (leuart -> SYNCBUSY & LEUART_SYNCBUSY_CMD);
	// Clear TX and RX Buffers
	leuart -> CMD = LEUART_CMD_CLEARRX | LEUART_CMD_CLEARTX;

	// Sync All
	while (leuart -> SYNCBUSY);

	// Verify RX and TX EN
	EFM_ASSERT(leuart -> STATUS & LEUART_STATUS_RXENS == leuart_settings -> rx_en * LEUART_STATUS_RXENS);
	EFM_ASSERT(leuart -> STATUS & LEUART_STATUS_TXENS == leuart_settings -> tx_en * LEUART_STATUS_TXENS);

	// Setup for Scheduler
	rx_done_evt = leuart_settings -> rx_done_evt;
	tx_done_evt = leuart_settings -> tx_done_evt;

	// Setup for Interrupts
	leuart -> IFC = leuart -> IF;
	leuart -> IEN = (LEUART_IEN_SIGF * leuart_settings -> sigframe_en) | (LEUART_IEN_STARTF * leuart_settings -> startframe_en) | LEUART_IEN_TXBL | LEUART_IEN_TXC; //TODO: TXC not needed?
	NVIC_EnableIRQ(LEUART0_IRQn);
}

void LEUART0_IRQHandler(void)
{
	__disable_irq();

	uint32_t iflags = LEUART0 -> IF & LEUART0 -> IEN;
	LEUART0 -> IFC = LEUART0 -> IF;

	if (iflags & LEUART_IF_SIGF)
	{

	}
	if (iflags & LEUART_IF_STARTF)
	{

	}
	if (iflags & LEUART_IF_TXBL)
	{

	}
	if (iflags & LEUART_IF_TXC)
	{

	}

	__enable_irq();
}

void leuart_start(LEUART_TypeDef * leuart, char * string, uint32_t string_len)
{

}

bool leuart_tx_busy(LEUART_TypeDef * leuart)
{
	if (leuart == LEUART0)
		return leuart0_tx_busy;
	return true;
}

/**
 * @brief
 *   LEUART STATUS function returns the STATUS of the peripheral for the
 *   TDD test
 *
 * @details
 * 	 This function enables the LEUART STATUS register to be provided to
 * 	 a function outside this .c module.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @return
 * 	 Returns the STATUS register value as an uint32_t value
 **/
uint32_t leuart_status(LEUART_TypeDef * leuart)
{
	uint32_t	status_reg;
	status_reg = leuart->STATUS;
	return status_reg;
}

/**
 * @brief
 *   LEUART CMD Write sends a command to the CMD register
 *
 * @details
 * 	 This function is used by the TDD test function to program the LEUART
 * 	 for the TDD tests.
 *
 * @note
 *   Before exiting this function to update  the CMD register, it must
 *   perform a SYNCBUSY while loop to ensure that the CMD has by synchronized
 *   to the lower frequency LEUART domain.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @param[in] cmd_update
 * 	 The value to write into the CMD register
 **/
void leuart_cmd_write(LEUART_TypeDef *leuart, uint32_t cmd_update)
{

	leuart->CMD = cmd_update;
	while(leuart->SYNCBUSY);
}

/**
 * @brief
 *   LEUART IF Reset resets all interrupt flag bits that can be cleared
 *   through the Interrupt Flag Clear register
 *
 * @details
 * 	 This function is used by the TDD test program to clear interrupts before
 * 	 the TDD tests and to reset the LEUART interrupts before the TDD
 * 	 exits
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 **/
void leuart_if_reset(LEUART_TypeDef *leuart)
{
	leuart->IFC = 0xffffffff;
}

/**
 * @brief
 *   LEUART App Transmit Byte transmits a byte for the LEUART TDD test
 *
 * @details
 * 	 The BLE module will respond to AT commands if the BLE module is not
 * 	 connected to the phone app.  To validate the minimal functionality
 * 	 of the LEUART peripheral, write and reads to the LEUART will be
 * 	 performed by polling and not interrupts.
 *
 * @note
 *   In polling a transmit byte, a while statement checking for the TXBL
 *   bit in the Interrupt Flag register is required before writing the
 *   TXDATA register.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @param[in] data_out
 *   Byte to be transmitted by the LEUART peripheral
 **/
void leuart_app_transmit_byte(LEUART_TypeDef *leuart, uint8_t data_out)
{
	while (!(leuart->IF & LEUART_IF_TXBL));
	leuart->TXDATA = data_out;
}


/**
 * @brief
 *   LEUART App Receive Byte polls a receive byte for the LEUART TDD test
 *
 * @details
 * 	 The BLE module will respond to AT commands if the BLE module is not
 * 	 connected to the phone app.  To validate the minimal functionality
 * 	 of the LEUART peripheral, write and reads to the LEUART will be
 * 	 performed by polling and not interrupts.
 *
 * @note
 *   In polling a receive byte, a while statement checking for the RXDATAV
 *   bit in the Interrupt Flag register is required before reading the
 *   RXDATA register.
 *
 * @param[in] leuart
 *   Defines the LEUART peripheral to access.
 *
 * @return
 * 	 Returns the byte read from the LEUART peripheral
 **/
uint8_t leuart_app_receive_byte(LEUART_TypeDef *leuart)
{
	uint8_t leuart_data;
	while (!(leuart->IF & LEUART_IF_RXDATAV));
	leuart_data = leuart->RXDATA;
	return leuart_data;
}
