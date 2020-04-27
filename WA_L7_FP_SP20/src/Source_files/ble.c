#define ifn(x) if(!(x))			/**< macro if not definition **/
#define whilen(x) while(!(x))	/**< macro while not definition **/

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
#include <stdio.h>

static BLE_CIRCULAR_BUF ble_cbuf;				/**< circular buffer struct for ble.c **/
static CIRC_TEST_STRUCT test_struct;			/**< circular buffer test struct for the TDD routine **/
static char ble_tx_string[BLE_STR_SIZE];		/**< ble string currently being transmitted by LEUART **/
static char ble_rx_string[(BLE_STR_SIZE / 2)];  /**< ble string currently being received **/

static uint32_t ble_tx_done_evt;				/**< scheduled event id for ble tx done **/
static uint32_t ble_rx_done_evt;				/**< scheduled event id for ble rx done **/

/**
 * @brief
 * 	checks if the circular buffer ble_cbuf is empty
 * @details
 *	compares the read and write indeces, if they are equal the buffer is empty
**/
static inline bool ble_circ_isEmpty(void)
{
	return (ble_cbuf.read_ptr == ble_cbuf.write_ptr);
}
/**
 * @brief
 * 	checks the amount of free space in the circular buffer
 * @details
 *	gets the difference between write and read indeces, wraps them to a valid value (0, size), and returns size - occupied
 * @note
 *  does not account for the "bubble" due to the architecture limitation, so will always stop at 1.
**/
static inline uint32_t ble_circ_space(void)
{
	return CSIZE - ((ble_cbuf.write_ptr - ble_cbuf.read_ptr) & ble_cbuf.size_mask);
}
/**
 * @brief
 * 	updates the circular buffer's write index safely
 * @details
 * 	adds update_by to index_struct's write_ptr and then masks it to not oob
 * @note
 * 	bypasses modulo math by using bitwise and with the mask
**/
static inline void update_circ_wrtindex(BLE_CIRCULAR_BUF * index_struct, uint32_t update_by)
{
	index_struct -> write_ptr = (index_struct -> write_ptr + update_by) & index_struct -> size_mask;
}
/**
 * @brief
 * 	updates the circular buffer's read index safely
 * @details
 * 	adds update_by to index_struct's read_ptr and then masks it to not oob
 * @note
 * 	bypasses modulo math by using bitwise and with the mask
**/
static inline void update_circ_readindex(BLE_CIRCULAR_BUF * index_struct, uint32_t update_by)
{
	index_struct -> read_ptr = (index_struct -> read_ptr + update_by) & index_struct -> size_mask;
}


/**
 * @brief
 *	sets up BLE to control LEUART for the HM10 module
 * @details
 *	initializes circular buffer and HM10_LEUART
 * @param[in] tx_event
 *	scheduler event ID for transmit complete
 * @param[in] rx_event
 *	scheduler event ID for receive complete
**/
void ble_open(uint32_t tx_event, uint32_t rx_event)
{
	ble_rx_done_evt = rx_event;
	ble_tx_done_evt = tx_event;

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
	leuart_open_s.rxblocken = true;
	// LEUART START AND SIG FRAMES
	leuart_open_s.sfubrx = true;
	leuart_open_s.startframe_en = true;
	leuart_open_s.startframe = HM10_STARTF;
	leuart_open_s.rxdatav_en = true;
	leuart_open_s.sigframe = HM10_SIGF;
	// LEUART RX STRING
	leuart_open_s.rxlen = BLE_STR_SIZE / 2;
	leuart_open_s.rxstring = ble_rx_string;
	// LEUART DMA
	leuart_open_s.tx_dma = LEUART_TX_DMA;
	// LEUART SCHEDULED EVENTS
	leuart_open_s.rx_done_evt = &ble_rx_done_evt;
	leuart_open_s.tx_done_evt = &ble_tx_done_evt;
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
/**
 * @brief
 * 	initializes the private circular buffer for ble.c
 * @details
 * 	sets the read and write pointers to 0, sets the size, creates the mask
**/
void ble_circ_init(void)
{
	ble_cbuf.read_ptr = ble_cbuf.write_ptr = 0;
	ble_cbuf.size = CSIZE; //MUST BE POWER OF 2
	ble_cbuf.size_mask = CSIZE - 1;
}
/**
 * @brief
 *	pushes a string onto the circular buffer
 * @details
 * 	checks if there is room for the packet, then copies the data into the circular buffer
 * @param[in] string
 * 	the string to be pushed onto the buffer
**/
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
/**
 * @brief
 * 	TDD routine for the circular buffer
 * @details
 * 	uses test_struct and ble_cbuf, this should be called before ble is used for reads / writes
**/
void circular_buff_test(void)
{
	 bool buff_empty;
	 int test1_len = 50;
	 int test2_len = 25;
	 int test3_len = 5;

	 // Why this 0 initialize of read and write pointer?
	 // Student Response: because we want the buffer to be empty (ridx = widx)
	 //
	 ble_cbuf.read_ptr = 0;
	 ble_cbuf.write_ptr = 0;

	 // Why do none of these test strings contain a 0?
	 // Student Response: two reasons: 1) none of the for loops allow 0 to be a value (+1, +20, +35) but the real reason is 2) the ASCII character 00000000 is a NULL character and we never want to see this
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
	 // Student Response: because we're just testing one string
	 ble_circ_push(&test_struct.test_str[0][0]);

	 // Why is the expected buff_empty test = false?
	 // Student Response: because we are popping off a string and the buffer is NOT empty, we should expect a value of false

	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test1_len; i++){
		 EFM_ASSERT(test_struct.test_str[0][i] == test_struct.result_str[i]);
	 }

	 // What does this next push on the circular buffer test?
	 // Student Response: we're testing the second string (with a different length)

	 ble_circ_push(&test_struct.test_str[1][0]);

	 // What does this next push on the circular buffer test?
	 // Student Response: we're testing the third string (with yet another different length)
	 ble_circ_push(&test_struct.test_str[2][0]);


	 // Why is the expected buff_empty test = false?
	 // Student Response: because the circular buffer is not empty
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test2_len; i++){
		 EFM_ASSERT(test_struct.test_str[1][i] == test_struct.result_str[i]);
	 }

	 // Why is the expected buff_empty test = false?
	 // Student Response: because the circular buffer is not empty
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test3_len; i++){
		 EFM_ASSERT(test_struct.test_str[2][i] == test_struct.result_str[i]);
	 }

	 // Using these three writes and pops to the circular buffer, what other test
	 // could we develop to better test out the circular buffer?
	 // Student Response: we could develop an overflow test (overfill the buffer), or an underflow test (empty string on, length 0), or an overwrite test (if our structure supports it)

	 // Why is the expected buff_empty test = true?
	 // Student Response: because the circular buffer is empty at this point, it will not be able to pop
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == true);
	 ble_write("\nPassed Circular Buffer Test\n");
}
/**
 * @brief
 *	pops a string off of the circular buffer, either to the test_struct or to LEUART (via ble_tx_string)
 * @details
 *	checks if the buffer is empty, attempts to pop (depending on the size of the packet and if there is room)
 * @param[in] test
 *	boolean indicating if we are calling a test pop (for TDD) or not
**/
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
		else if (!leuart_tx_busy(HM10_LEUART0))
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
/**
 * @brief
 *  TDD routine for BLE RX
 * @details
 *  verifies the app specific setup and operation of BLE RX
 * @note
 * 	this function contains EFM_ASSERT(false) statements, it is possible to be stuck here
**/
void ble_rx_test()
{
	//wait for idle
	while(leuart_tx_busy(HM10_LEUART0) | leuart_rx_busy());
//PRE (SETUP CHECKS)
	// TEST  1:
	//	verify that receiving is enabled
	ifn (HM10_LEUART0 -> STATUS & LEUART_STATUS_RXENS)
		EFM_ASSERT(false);

	// TEST  2:
	//	verify that interrupt is enabled for STARTF
	ifn (HM10_LEUART0 -> IEN & LEUART_IEN_STARTF)
		EFM_ASSERT(false);

	// TEST  3:
	//	verify that interrupt is NOT enabled for SIGF
	if (HM10_LEUART0 -> IEN & LEUART_IEN_SIGF)
		EFM_ASSERT(false);

	// TEST  4:
	//	verify that interrupt is enabled for RXDATAV
	ifn (HM10_LEUART0 -> IEN & LEUART_IEN_RXDATAV)
		EFM_ASSERT(false);

	// TEST  5:
	//	verify that RXBLOCK is enabled
	ifn (HM10_LEUART0 -> STATUS & LEUART_STATUS_RXBLOCK)
		EFM_ASSERT(false);

	// TEST  6:
	//	verify that SFUBRX is enabled
	ifn (HM10_LEUART0 -> CTRL & LEUART_CTRL_SFUBRX)
		EFM_ASSERT(false);

	// TEST  7:
	// verify STARTF != SIGF
	if (HM10_LEUART0 -> STARTFRAME == HM10_LEUART0 -> SIGFRAME)
		EFM_ASSERT(false);
//DURING (TESTS)
	//LOOPBK mode (links RX to TX)
	HM10_LEUART0 -> CTRL |=  LEUART_CTRL_LOOPBK;
	while (HM10_LEUART0 -> SYNCBUSY & LEUART_SYNCBUSY_CTRL);
	__disable_irq();
	//CLEAR IEN REGISTER
//	uint32_t backup_ien = HM10_LEUART0 -> IEN;
//	HM10_LEUART0 -> IEN = 0;

	// TEST  8
	//	test character (blocked)
	HM10_LEUART0 -> IFC = HM10_LEUART0 -> IF; //clear all interrupts
	HM10_LEUART0 -> TXDATA = 'a';
	whilen (HM10_LEUART0 -> IF & LEUART_IF_TXC);
	if (HM10_LEUART0 -> IF & LEUART_IF_RXDATAV)
		EFM_ASSERT(false);

	// TEST 9, 10:
	//	test signal frame (blocked, but still appears)
	HM10_LEUART0 -> IFC = HM10_LEUART0 -> IF; //clear any leftover interrupts from above
	HM10_LEUART0 -> TXDATA = HM10_SIGF;
	whilen (HM10_LEUART0 -> IF & (LEUART_IF_TXC | LEUART_IF_TXBL));
	// should still get signal frame interrupt
	ifn (HM10_LEUART0 -> IF & LEUART_IF_SIGF)
		EFM_ASSERT(false);
	// should not get rxdatav interrupt (rxblocken)
	if (HM10_LEUART0 -> IF & LEUART_IF_RXDATAV)
		EFM_ASSERT(false);


	// TEST 11, 12, 13:
	//	test start frame
	HM10_LEUART0 -> IFC = HM10_LEUART0 -> IF;
	HM10_LEUART0 -> TXDATA = HM10_STARTF;
	whilen (HM10_LEUART0 -> IF & (LEUART_IF_TXC | LEUART_IF_TXBL));
	// should get start frame interrupt
	ifn (HM10_LEUART0 -> IF & LEUART_IF_STARTF)
		HM10_LEUART0 -> CTRL &= ~LEUART_CTRL_LOOPBK;
	// rxblock should be cleared
	if (HM10_LEUART0 -> STATUS & LEUART_STATUS_RXBLOCK)
		EFM_ASSERT(false);
	// should get rxdatav interrupt
	ifn (HM10_LEUART0 -> IF & LEUART_IF_RXDATAV)
		EFM_ASSERT(false);

	HM10_LEUART0 -> CMD |= LEUART_CMD_RXBLOCKEN | LEUART_CMD_CLEARRX;
	HM10_LEUART0 -> IFC = HM10_LEUART0 -> IF;
	while (HM10_LEUART0 -> SYNCBUSY & LEUART_SYNCBUSY_CMD);
	uint32_t backup_ble_rx_done_evt = ble_rx_done_evt;
	uint32_t backup_ble_tx_done_evt = ble_tx_done_evt;
	ble_rx_done_evt = (ble_tx_done_evt = 0);

	__enable_irq();
	char testString[32];
	// TEST 14:
	//	test empty command (start, sig)
	sprintf(testString, "<>");
	leuart_start(HM10_LEUART0, testString, strlen(testString));
	while(leuart_tx_busy(HM10_LEUART0) || leuart_rx_busy());
	if (strcmp(testString, ble_rx_string))
		EFM_ASSERT(false);

	// TEST 15:
	//	test command that fits in rxstring
	sprintf(testString, "<tempQ>");
	leuart_start(HM10_LEUART0, testString, strlen(testString));
	while(leuart_tx_busy(HM10_LEUART0) || leuart_rx_busy());
	if (strcmp(testString, ble_rx_string))
		EFM_ASSERT(false);

	// TEST 16:
	//	test repeated start, full overwrite
	sprintf(testString, "<tempR<tempQ>");
	leuart_start(HM10_LEUART0, testString, strlen(testString));
	sprintf(testString, "<tempQ>");
	while(leuart_tx_busy(HM10_LEUART0) || leuart_rx_busy());
	if (strcmp(testString, ble_rx_string))
		EFM_ASSERT(false);

	// TEST 17:
	//	test repeated start, only partial overwrite
	sprintf(testString, "<tempS<tempR<tempQ>");
	leuart_start(HM10_LEUART0, testString, strlen(testString));
	sprintf(testString, "<tempQ>");
	while(leuart_tx_busy(HM10_LEUART0) || leuart_rx_busy());
	if (strcmp(testString, ble_rx_string))
		EFM_ASSERT(false);

//POST (RESTORE)
	// TEST PASSED:
	__disable_irq();
	HM10_LEUART0 -> IFC = HM10_LEUART0 -> IF; //clear all pending interrupts
	HM10_LEUART0 -> CTRL &= ~LEUART_CTRL_LOOPBK;
	ble_rx_done_evt = backup_ble_rx_done_evt;
	ble_tx_done_evt = backup_ble_tx_done_evt;
	while (HM10_LEUART0 -> SYNCBUSY & LEUART_SYNCBUSY_CTRL);
	__enable_irq();
}
/**
 * @brief
 * 	generic getter for ble_rx_string
 * @details
 *	to be used by app.c in the scheduled_leuart_rx event
 * @return
 * 	ble_rx_string (char *)
**/
char * ble_getCMD()
{
	return ble_rx_string;
}
