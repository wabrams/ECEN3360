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

static BLE_CIRCULAR_BUF ble_cbuf;
static CIRC_TEST_STRUCT test_struct;
static char ble_tx_string[16];

static inline bool ble_circ_isEmpty(void)
{
	return (ble_cbuf.read_ptr == ble_cbuf.write_ptr);
}
static inline uint8_t ble_circ_space(void)
{
	return CSIZE - ((ble_cbuf.write_ptr - ble_cbuf.read_ptr) & ble_cbuf.size_mask);
}
static inline void update_circ_wrtindex(BLE_CIRCULAR_BUF * index_struct, uint32_t update_by)
{
	index_struct -> write_ptr = (index_struct -> write_ptr + update_by) & index_struct -> size_mask;
}
static inline void update_circ_readindex(BLE_CIRCULAR_BUF * index_struct, uint32_t update_by)
{
	index_struct -> read_ptr = (index_struct -> read_ptr + update_by) & index_struct -> size_mask;
}


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

	ble_circ_init();
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
	ble_circ_push(string);
	if (!leuart_tx_busy(HM10_LEUART0))
		ble_circ_pop(false);
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
void ble_circ_init(void)
{
	ble_cbuf.read_ptr = ble_cbuf.write_ptr = 0;
	ble_cbuf.size = CSIZE; //MUST BE POWER OF 2
	ble_cbuf.size_mask = CSIZE - 1;
}
void ble_circ_push(char * string)
{
	EFM_ASSERT(ble_circ_space());

	uint8_t len = strlen(string);
	//ROOM FOR PACKET?
	if ((len + 1) <= ble_circ_space())
	{
		//PACKET HEADER
		ble_cbuf.cbuf[ble_cbuf.write_ptr] = (char) len;
		update_circ_wrtindex(&ble_cbuf, 1);
		//PACKET BODY
		for (int i = 0; i < len; i++)
		{
			ble_cbuf.cbuf[ble_cbuf.write_ptr] = string[i];
			update_circ_wrtindex(&ble_cbuf, 1);
		}
	}
	else
	{
		EFM_ASSERT(false);
	}
}
void circular_buff_test(void)
{
	 bool buff_empty;
	 int test1_len = 50;
	 int test2_len = 25;
	 int test3_len = 5;

	 // Why this 0 initialize of read and write pointer?
	 // Student Response:
	 //
	 ble_cbuf.read_ptr = 0;
	 ble_cbuf.write_ptr = 0;

	 // Why do none of these test strings contain a 0?
	 // Student Response:
	 //
	 for (int i = 0;i < test1_len; i++){
		 test_struct.test_str[0][i] = i+1;
	 }
	 for (int i = 0;i < test2_len; i++){
		 test_struct.test_str[1][i] = i + 20;
	 }
	 for (int i = 0;i < test3_len; i++){
		 test_struct.test_str[2][i] = i +  35;
	 }

	 // Why is there only one push to the circular buffer at this stage of the test
	 // Student Response:
	 //
	 ble_circ_push(&test_struct.test_str[0][0]);

	 // Why is the expected buff_empty test = false?
	 // Student Response:
	 //

	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test1_len; i++){
		 EFM_ASSERT(test_struct.test_str[0][i] == test_struct.result_str[i]);
	 }

	 // What does this next push on the circular buffer test?
	 // Student Response:

	 ble_circ_push(&test_struct.test_str[1][0]);

	 // What does this next push on the circular buffer test?
	 // Student Response:
	 ble_circ_push(&test_struct.test_str[2][0]);


	 // Why is the expected buff_empty test = false?
	 // Student Response:
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test2_len; i++){
		 EFM_ASSERT(test_struct.test_str[1][i] == test_struct.result_str[i]);
	 }

	 // Why is the expected buff_empty test = false?
	 // Student Response:
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test3_len; i++){
		 EFM_ASSERT(test_struct.test_str[2][i] == test_struct.result_str[i]);
	 }

	 // Using these three writes and pops to the circular buffer, what other test
	 // could we develop to better test out the circular buffer?
	 // Student Response:


	 // Why is the expected buff_empty test = true?
	 // Student Response:
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == true);
	 ble_write("\nPassed Circular Buffer Test\n");

}

bool ble_circ_pop(bool test)
{
	if (!ble_circ_isEmpty())
	{
		if (test)
		{
			uint8_t len = (uint8_t)ble_cbuf.cbuf[ble_cbuf.read_ptr];
			update_circ_readindex(&ble_cbuf, 1);
			for (int i = 0; i < len; i++)
			{
				test_struct.result_str[i] = ble_cbuf.cbuf[ble_cbuf.read_ptr];
				update_circ_readindex(&ble_cbuf, 1);
			}
			return false;
		}
		else
		{
			uint8_t len = (uint8_t)ble_cbuf.cbuf[ble_cbuf.read_ptr];
			if (len + 1 <= BLE_STR_SIZE)
			{
				update_circ_readindex(&ble_cbuf, 1);
				for (int i = 0; i < len; i++)
				{
					ble_tx_string[i] = ble_cbuf.cbuf[ble_cbuf.read_ptr];
					update_circ_readindex(&ble_cbuf, 1);
				}
				ble_tx_string[len] = '\0';
				leuart_start(HM10_LEUART0, ble_tx_string, len);
				return false;
			}
			else
				EFM_ASSERT(false);
		}
	}
	return true;
}
