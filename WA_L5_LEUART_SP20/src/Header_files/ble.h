#ifndef BLE_H
#define BLE_H

#include <stdbool.h>
#include <stdint.h>

#include "leuart.h"
#include "em_leuart.h"

#define HM10_LEUART0		LEUART0
#define HM10_BAUDRATE		9600
#define	HM10_DATABITS		leuartDatabits8
#define HM10_ENABLE			leuartEnable
#define HM10_PARITY			leuartNoParity
#define HM10_REFFREQ		0
#define HM10_STOPBITS		leuartStopbits1

#define LEUART0_TX_RLOC		LEUART_ROUTELOC0_TXLOC_LOC18
#define LEUART0_RX_RLOC		LEUART_ROUTELOC0_RXLOC_LOC18
#define LEUART0_TX_RPEN		LEUART_ROUTEPEN_TXPEN
#define LEUART0_RX_RPEN		LEUART_ROUTEPEN_RXPEN

void ble_open(uint32_t tx_event, uint32_t rx_event);
void ble_write(char *string);

bool ble_test(char *mod_name);

#endif
