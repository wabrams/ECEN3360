/**
 * @file leuart.c
 * @author William Abrams
 * @date March 8th, 2020
 * @brief Contains all the functions needed to control LEUART
 **/

#include <string.h>
#include <stdbool.h>
#include "em_gpio.h"
#include "em_cmu.h"
#include "leuart.h"
#include "scheduler.h"

static uint32_t * rx_done_evt;							/**< Scheduler event ID for RX Done event **/
static uint32_t	* tx_done_evt;							/**< Scheduler event ID for TX Done event **/

static leuart_txstate_t txstate = LEUART_STATE_TX_IDLE; /**< State Machine state variable for transmitting **/
static volatile bool leuart0_txbusy = false;			/**< Status boolean, acts as weak mutex **/
static char * txstring;									/**< Pointer, to next char to be transmitted **/
static uint32_t txcnt = 0;								/**< Counter variable, of characters left to transmit **/


static leuart_rxstate_t rxstate = LEUART_STATE_RX_IDLE; /**< State Machine state variable for receiving **/
static char * rxstring;									/**< Receiving String, where we write RXDATA to **/
static uint32_t rxlen;									/**< Counter helper, length of rxstring **/
static uint32_t rxcnt = 0;								/**< Counter variable, of characters received so far **/


static bool leuart_tx_dma = false;						/**< TODO: Unused, for future implementation using LDMA **/
static bool leuart_rx_dma = false;						/**< TODO: Unused, for future implementation using LDMA **/

/**
 * @brief
 *	Opener function for LEUART
 * @details
 *	Sets up leuart based on leuart_settings, clears TX and RX buffers, enables interrupt handler but not TXBL or TXC
 * @param[in] leuart
 *  Pointer to an LEUART peripheral
 * @param[in] leuart_settings
 * 	Structure used to pass in all configuration values, see the documentation of LEUART_OPEN_STRUCT for more
 **/
void leuart_open(LEUART_TypeDef * leuart, LEUART_OPEN_STRUCT * leuart_settings)
{
	// Enable LEUART0 Clock
	if (leuart == LEUART0)
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
	while(leuart -> SYNCBUSY);

	// LEUART Routing Setup
	leuart -> ROUTELOC0 = leuart_settings -> rx_rloc | leuart_settings -> tx_rloc;
	leuart -> ROUTEPEN = leuart_settings -> rx_rpen | leuart_settings -> tx_rpen;

	// RX STRING
	sleep_block_mode(LEUART_RX_EM_BLOCK); //FIXME: this should not be done here
	rxstring = leuart_settings -> rxstring;
	rxlen = leuart_settings -> rxlen;

	// MISC Setup
	leuart -> CMD = (LEUART_CMD_RXBLOCKEN * leuart_settings -> rxblocken);
	while (leuart -> SYNCBUSY);
	leuart -> CTRL |= leuart_settings -> sfubrx * LEUART_CTRL_SFUBRX;

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
	EFM_ASSERT((leuart -> STATUS & LEUART_STATUS_RXENS) == (leuart_settings -> rx_en * LEUART_STATUS_RXENS));
	EFM_ASSERT((leuart -> STATUS & LEUART_STATUS_TXENS) == (leuart_settings -> tx_en * LEUART_STATUS_TXENS));

	// Setup for Scheduler
	rx_done_evt = leuart_settings -> rx_done_evt;
	tx_done_evt = leuart_settings -> tx_done_evt;

	// Set State Machine
	txstate = LEUART_STATE_TX_IDLE;
	rxstate = LEUART_STATE_RX_IDLE;

	// Pass off to DMA
	leuart_rx_dma = leuart_settings -> rx_dma;
	leuart_tx_dma = leuart_settings -> tx_dma;
//TODO:	LEUART_RxDmaInEM2Enable(leuart, leuart_rx_dma);
//	LEUART_TxDmaInEM2Enable(leuart, leuart_tx_dma);

	// Setup for Interrupts
	leuart -> IFC = leuart -> IF; //TODO: no sigf interrupt until startf
	leuart -> IEN = (LEUART_IEN_STARTF * leuart_settings -> startframe_en) | (LEUART_IEN_RXDATAV * leuart_settings -> rxdatav_en);
	if (leuart == LEUART0)
		NVIC_EnableIRQ(LEUART0_IRQn);
}
/**
 * @brief
 * 	LEUART0's IRQ Handler
 * @details
 * 	Clears interrupt flags, handles enabled interrupts
 * @note
 * 	Despite containing options for SIGF and STARTF, these are NYI
 **/
void LEUART0_IRQHandler(void)
{
	__disable_irq();

	uint32_t iflags = (LEUART0 -> IFC = LEUART0 -> IF) & LEUART0 -> IEN;

	if (iflags & LEUART_IF_STARTF)
	{
		switch (rxstate)
		{
			case LEUART_STATE_RX_IDLE:
				memset(rxstring,0,rxlen); //purge all rx data
				rxcnt = 0;
				rxstate = LEUART_STATE_RX_RECEIVE;
				LEUART0 -> IEN |= LEUART_IEN_SIGF;
				break;
			case LEUART_STATE_RX_RECEIVE:
				memset(rxstring,0,rxlen); //purge all rx data
				rxcnt = 0;
				break;
			default:
				EFM_ASSERT(false);
				break;
		}
	}
	if (iflags & LEUART_IF_RXDATAV)
	{
		switch (rxstate)
		{
			case LEUART_STATE_RX_IDLE:
				EFM_ASSERT(false);
				break;
			case LEUART_STATE_RX_RECEIVE:
				rxstring[rxcnt] = LEUART0 -> RXDATA;
				rxcnt++;
				//rxcnt is valid from 0 to rxlen - 1
				if (rxcnt >= rxlen)
					rxcnt = 1;
				break;
			default:
				EFM_ASSERT(false);
		}
	}
	if (iflags & LEUART_IF_SIGF)
	{
		switch (rxstate)
		{
			case LEUART_STATE_RX_IDLE:
				EFM_ASSERT(false);
				break;
			case LEUART_STATE_RX_RECEIVE:
				//done reading:
				LEUART0 -> CMD = LEUART_CMD_RXBLOCKEN | LEUART_CMD_CLEARRX;
				LEUART0 -> IEN &= ~LEUART_IEN_SIGF;
				rxstate = LEUART_STATE_RX_IDLE;
				add_scheduled_event(*rx_done_evt);
				break;
			default:
				EFM_ASSERT(false);
		}
	}
	if (iflags & LEUART_IF_TXBL)
	{
		switch (txstate)
		{
			case LEUART_STATE_TX_TRANSMIT:
				if (txcnt > 0)
				{
					LEUART0 -> TXDATA = *txstring;
					txstring++; //slide pointer over
					txcnt--; //one less char to send
				}
				else
				{
					txstate = LEUART_STATE_TX_DONE;
					LEUART0 -> IEN &= ~LEUART_IEN_TXBL;
					LEUART0 -> IEN |= LEUART_IEN_TXC;
				}
				break;
			case LEUART_STATE_TX_IDLE:
			case LEUART_STATE_TX_DONE:
				EFM_ASSERT(false);
				break;
			default: //should never end up here
				EFM_ASSERT(false);
				break;
		}
	}
	if (iflags & LEUART_IF_TXC)
	{
		switch (txstate)
		{
			case LEUART_STATE_TX_DONE:
				txstate = LEUART_STATE_TX_IDLE;
				LEUART0 -> IEN &= ~LEUART_IEN_TXC;
				leuart0_txbusy = false;
				add_scheduled_event(*tx_done_evt);
				sleep_unblock_mode(LEUART_TX_EM_BLOCK);
				break;
			case LEUART_STATE_TX_IDLE:
			case LEUART_STATE_TX_TRANSMIT:
				EFM_ASSERT(false);
				break;
			default: //should never end up here
				EFM_ASSERT(false);
				break;
		}
	}

	__enable_irq();
}
/**
 * @brief
 *	Starts a transmission over LEUART
 * @details
 *  Blocks sleep, sets the mutex, and begins transmission.
 * @param[in] leuart
 *  LEUART peripheral to transmit over
 * @param[in] string
 * 	Pointer to the input string (to be transmitted)
 * @param[in] string_len
 * 	Length of the input string
 * @details
 * 	If string is declared on the stack and not the heap, it will disappear and cause leaurt to transmit incorrectly
 */
void leuart_start(LEUART_TypeDef * leuart, char * string, uint32_t string_len)
{
	//wait till not busy
	while(leuart_tx_busy(leuart));
	leuart0_txbusy = true;
	//block sleep
	sleep_block_mode(LEUART_TX_EM_BLOCK);
	//copy over tx information
	txstring = string;
	txcnt = string_len;
	txstate = LEUART_STATE_TX_TRANSMIT;
	//enable TXBL (should hit immediately)
	leuart -> IEN |= LEUART_IEN_TXBL;
}

/**
 * @brief
 *	Simple mutex to check if LEUART peripheral is busy in TX operation
 * @param[in] leuart
 * 	Pointer to the LEUART peripheral
 * @returns
 * 	Returns true if TX operation already in progress
 * 	Returns false if TX is idle (not in use)
 **/
bool leuart_tx_busy(LEUART_TypeDef * leuart)
{
	if (leuart == LEUART0)
		return leuart0_txbusy;
	return true;
}
/**
 * @brief
 *	Simple mutex to check if LEUART peripheral is busy in RX operation
 * @returns
 * 	Returns true if RX operation already in progress
 * 	Returns false if RX is idle (not in use)
 **/
bool leuart_rx_busy()
{
	return !(rxstate == LEUART_STATE_RX_IDLE);
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
