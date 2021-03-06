/**
 * @file ble.c
 * @author William Abrams
 * @date March 8th, 2020
 * @brief Contains all the functions to interface the application with the HM-18
 *   BLE module and the LEUART driver
 **/

#include "ble.h"
#include "leuart.h"
#include <string.h>

/**
 * @brief
 *
 * @details
 *
 * @param[in] tx_event
 *
 * @param[in] rx_event
 *
 **/
void ble_open(uint32_t tx_event, uint32_t rx_event)
{
	LEUART_OPEN_STRUCT leuart_open_s;
	// LEUART INIT STRUCT fields
	leuart_open_s.baudrate = HM10_BAUDRATE;
	leuart_open_s.databits = HM10_DATABITS;
	leuart_open_s.enable = HM10_ENABLE;
	leuart_open_s.parity = HM10_PARITY;
	leuart_open_s.refFreq =  HM10_REFFREQ;
	leuart_open_s.stopbits = HM10_STOPBITS;
	// LEUART RX and TX RouteLoc
	leuart_open_s.rx_rloc = LEUART0_RX_RLOC;
	leuart_open_s.rx_rpen = LEUART0_RX_RPEN;
	leuart_open_s.tx_rloc = LEUART0_TX_RLOC;
	leuart_open_s.tx_rpen = LEUART0_TX_RPEN;
	// LEUART CMD SETUP
	leuart_open_s.rx_en   = true;
	leuart_open_s.tx_en   = true;
	leuart_open_s.rxblocken = false;
	// LEUART START AND SIG FRAMES
	leuart_open_s.sfubrx = false;
	leuart_open_s.startframe_en = false;
	leuart_open_s.startframe = ' ';
	leuart_open_s.sigframe_en = false;
	leuart_open_s.sigframe = ' ';
	// LEUART DMA
	leuart_open_s.tx_dma = LEUART_TX_DMA;
	// LEUART SCHEDULED EVENTS
	leuart_open_s.rx_done_evt = rx_event;
	leuart_open_s.tx_done_evt = tx_event;

	leuart_open(HM10_LEUART0, &leuart_open_s);
}

/**
 * @brief
 *	Starts a write to the BLE (HM-10) device
 * @details
 *	Uses input string to call leuart_start(), which begins transmitting the string if LEUART is not already transmitting
 * @param[in] string
 *	input string to be transmitted
 **/
void ble_write(char * string)
{
	//MAYBE: app should call this and this should copy and "store the string"
	leuart_start(HM10_LEUART0, string, strlen(string));
}

/**
 * @brief
 *   BLE Test performs two functions.  First, it is a Test Driven Development
 *   routine to verify that the LEUART is correctly configured to communicate
 *   with the BLE HM-18 module.  Second, the input argument passed to this
 *   function will be written into the BLE module and become the new name
 *   advertised by the module while it is looking to pair.
 *
 * @details
 * 	 This global function will use polling functions provided by the LEUART
 * 	 driver for both transmit and receive to validate communications with
 * 	 the HM-18 BLE module.  For the assignment, the communication with the
 * 	 BLE module must use low energy design principles of being an interrupt
 * 	 driven state machine.
 *
 * @note
 *   For this test to run to completion, the phone most not be paired with
 *   the BLE module.  In addition for the name to be stored into the module
 *   a breakpoint must be placed at the end of the test routine and stopped
 *   at this breakpoint while in the debugger for a minimum of 5 seconds.
 *
 * @param[in] *mod_name
 *   The name that will be written to the HM-18 BLE module to identify it
 *   while it is advertising over Bluetooth Low Energy.
 *
 * @return
 *   Returns bool true if successfully passed through the tests in this
 *   function.
 **/
bool ble_test(char * mod_name)
{
	uint32_t str_len;

	__disable_irq();

	// How is polling different than using interrupts?
	// 	ANSWER: it's a busy / wait instead of being trigged by an interrupt
	// How does interrupts benefit the system for low energy operation?
	// 	ANSWER: we can sleep while waiting for interrupts while waiting
	// How do interrupts benefit the system that has multiple tasks?
	// 	ANSWER: can work on other things while we wait for an interrupt

	char break_str[80] = "AT";
	char ok_str[80] = "OK";

	char output_str[80] = "AT+NAME";
	char result_str[80] = "OK+Set:";

	char reset_str[80] = "AT+RESET";
	char reset_result_str[80] = "OK+RESET";
	char return_str[80];

	bool rx_disabled, rx_en, tx_en;
	uint32_t status;

	// These are the routines that will build up the entire command and response
	// of programming the name into the BLE module.  Concatenating the command or
	// response with the input argument name
	strcat(output_str, mod_name);
	strcat(result_str, mod_name);

	status = leuart_status(HM10_LEUART0);
	if (status & LEUART_STATUS_RXBLOCK)
	{
		rx_disabled = true;
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXBLOCKDIS);
	}
	else
		rx_disabled = false;

	if (status & LEUART_STATUS_RXENS)
		rx_en = true;
	else
	{
		rx_en = false;
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXEN);
		while (!(leuart_status(HM10_LEUART0) & LEUART_STATUS_RXENS));
	}

	if (status & LEUART_STATUS_TXENS)
		tx_en = true;
	else
	{
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_TXEN);
		while (!(leuart_status(HM10_LEUART0) & LEUART_STATUS_TXENS));
		tx_en = false;
	}
	leuart_cmd_write(HM10_LEUART0, (LEUART_CMD_CLEARRX | LEUART_CMD_CLEARTX));

	// Why is this command required if you want to change the name of the DSD HM10 module?
	//  ANSWER: it won't listen to commands while connected to the device
	str_len = strlen(break_str);
	for (int i = 0; i < str_len; i++)
		leuart_app_transmit_byte(HM10_LEUART0, break_str[i]);

	// What will the ble module response back to this command if there is a current ble connection?
	// 	ANSWER: either OK or OK+LOST (if AT+NOTI = 1)
	str_len = strlen(ok_str);
	for (int i = 0; i < str_len; i++)
	{
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (ok_str[i] != return_str[i])
			EFM_ASSERT(false);
	}

	// This sequence of code will be writing or programming the name of
	// the module to the DSD HM10
	str_len = strlen(output_str);
	for (int i = 0; i < str_len; i++)
		leuart_app_transmit_byte(HM10_LEUART0, output_str[i]);

	// Here will be the check on the response back from the DSD HM10 on the
	// programming of its name
	str_len = strlen(result_str);
	for (int i = 0; i < str_len; i++)
	{
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (result_str[i] != return_str[i])
			EFM_ASSERT(false);
	}

	// It is now time to send the command to RESET the DSD HM10 module
	str_len = strlen(reset_str);
	for (int i = 0; i < str_len; i++)
		leuart_app_transmit_byte(HM10_LEUART0, reset_str[i]);

	// After sending the command to RESET, the DSD HM10 will send a response
	// back to the micro-controller
	str_len = strlen(reset_result_str);
	for (int i = 0; i < str_len; i++)
	{
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (reset_result_str[i] != return_str[i])
			EFM_ASSERT(false);
	}

	// After the test and programming have been completed, the original
	// state of the LEUART must be restored
	if (!rx_en) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXDIS);
	if (rx_disabled) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXBLOCKEN);
	if (!tx_en) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_TXDIS);
	leuart_if_reset(HM10_LEUART0);

	__enable_irq();

	return true;
}
