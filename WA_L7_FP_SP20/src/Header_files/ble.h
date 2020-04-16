/**
 * @file ble.h
 */

#ifndef BLE_H
#define BLE_H
//#define BLE_TEST_ENABLED

#include <stdbool.h>
#include <stdint.h>

#include "leuart.h"
#include "em_leuart.h"

#define CIRC_TEST true			/**< boolean value indicating if the call to pop is for TDD or not **/
#define CIRC_TEST_SIZE 3		/**< number of test strings, used for the CIRC_TEST_STRUCT **/
#define CSIZE 128				/**< number of characters in the circular buffer (usable is 1 less) **/
#define BLE_STR_SIZE 32			/**< size of the ble tx string for LEUART (LEUART receives a pointer of this) **/

/**
 * @brief
 * Structure used to hold information for BLE (FIFO of strings to write over UART)
 **/
typedef struct
{
	char	cbuf[CSIZE];		/**< char array for the circular buffer **/
	uint8_t	size_mask;			/**< mask, based on the size (relies on power of 2 math) **/
	uint32_t size;				/**< size of the circular buffer, MUST BE POWER OF 2 **/
	uint32_t read_ptr;			/**< read index between 0 and size **/
	uint32_t write_ptr;			/**< write index between 0 and size **/
} BLE_CIRCULAR_BUF;

/**
 * @brief
 * Structure used to test BLE CB
 **/
typedef struct
{
	char test_str[CIRC_TEST_SIZE][CSIZE];	/**< array of strings for test struct **/
	char result_str[CSIZE];					/**< result string used for test struct **/
} CIRC_TEST_STRUCT;

void ble_circ_init(void);
void ble_circ_push(char *);
void circular_buff_test(void);
bool ble_circ_pop(bool);

#define HM10_LEUART0		LEUART0				/**< BLE LEUART peripheral to use **/
#define HM10_BAUDRATE		9600				/**< BLE baud rate **/
#define	HM10_DATABITS		leuartDatabits8		/**< BLE databits in each packet **/
#define HM10_ENABLE			leuartEnable		/**< BLE LEUART enable **/
#define HM10_PARITY			leuartNoParity		/**< BLE parity to use for LEUART **/
#define HM10_REFFREQ		0					/**< BLE LEUART reference frequency **/
#define HM10_STOPBITS		leuartStopbits1		/**< BLE stopbits to look for **/

//command would look like: <tempC> or <tempF>
#define HM10_STARTF		'<'
#define HM10_SIGF		'>'

#define LEUART_TX_DMA		true				/**< TODO: Unused, for future implementation of LDMA **/

#define LEUART0_TX_RLOC		LEUART_ROUTELOC0_TXLOC_LOC18	/**< LEUART route location for TX pin to HM-10 **/
#define LEUART0_RX_RLOC		LEUART_ROUTELOC0_RXLOC_LOC18	/**< LEUART route location for RX pin to HM-10 **/
#define LEUART0_TX_RPEN		LEUART_ROUTEPEN_TXPEN			/**< LEUART route pin enabling for TX pin **/
#define LEUART0_RX_RPEN		LEUART_ROUTEPEN_RXPEN			/**< LEUART route pin enabling for RX pin **/

void ble_open(uint32_t tx_event, uint32_t rx_event);
void ble_write(char *string);
bool ble_test(char *mod_name);
char * ble_getCMD();

#endif
