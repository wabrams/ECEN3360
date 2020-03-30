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

#define HM10_LEUART0		LEUART0				/**< BLE LEUART peripheral to use **/
#define HM10_BAUDRATE		9600				/**< BLE baud rate **/
#define	HM10_DATABITS		leuartDatabits8		/**< BLE databits in each packet **/
#define HM10_ENABLE			leuartEnable		/**< BLE LEUART enable **/
#define HM10_PARITY			leuartNoParity		/**< BLE parity to use for LEUART **/
#define HM10_REFFREQ		0					/**< BLE LEUART reference frequency **/
#define HM10_STOPBITS		leuartStopbits1		/**< BLE stopbits to look for **/

#define LEUART_TX_DMA		true				/**< TODO: Unused, for future implementation of LDMA **/

#define LEUART0_TX_RLOC		LEUART_ROUTELOC0_TXLOC_LOC18	/**< LEUART route location for TX pin to HM-10 **/
#define LEUART0_RX_RLOC		LEUART_ROUTELOC0_RXLOC_LOC18	/**< LEUART route location for RX pin to HM-10 **/
#define LEUART0_TX_RPEN		LEUART_ROUTEPEN_TXPEN			/**< LEUART route pin enabling for TX pin **/
#define LEUART0_RX_RPEN		LEUART_ROUTEPEN_RXPEN			/**< LEUART route pin enabling for RX pin **/

void ble_open(uint32_t tx_event, uint32_t rx_event);
void ble_write(char *string);
bool ble_test(char *mod_name);

#endif
