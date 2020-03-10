
#ifndef LEUART_H
#define LEUART_H

#include "em_leuart.h"
#include "sleep_routines.h"

#define LEUART_TX_EM EM2
#define LEUART_RX_EM EM4

typedef enum
{
	LEUART_STATE_TX_IDLE,
	LEUART_STATE_TX_START,
	LEUART_STATE_TX_TRANSMIT,
	LEUART_STATE_TX_DONE,
	LEUART_STATE_TX_STOP
} leuart_txstate_t;

/***************************************************************************//**
 * @addtogroup leuart
 * @{
 ******************************************************************************/

typedef struct
{
	// LEUART_Init_TypeDef
	uint32_t					baudrate;
	LEUART_Databits_TypeDef		databits;
	LEUART_Enable_TypeDef		enable;
	LEUART_Parity_TypeDef 		parity;
	uint32_t 					refFreq;
	LEUART_Stopbits_TypeDef		stopbits;
	// LEUART RX and TX RouteLoc
	uint32_t					rx_rloc;
	uint32_t					rx_rpen;
	uint32_t					tx_rloc;
	uint32_t					tx_rpen;
	// LEUART CMD Vars
	bool						rx_en;
	bool						tx_en;
	bool						rxblocken;
	// START and SIG Variables
	bool						sfubrx;
	bool						startframe_en;
	char						startframe;		/**< With 8 data-bit frame, only the 8 least significant bits of LEUARTn_STARTFRAME are compared. **/
	bool						sigframe_en;
	char						sigframe;		/**< With 8 data-bit frame, only the 8 least significant bits of LEUARTn_SIGFRAME are compared. **/
	// Scheduler Event IDs
	uint32_t					rx_done_evt;
	uint32_t					tx_done_evt;
} LEUART_OPEN_STRUCT;
/** @} (end addtogroup leuart) */

void leuart_open(LEUART_TypeDef *leuart, LEUART_OPEN_STRUCT * leuart_settings);
void LEUART0_IRQHandler(void);
void leuart_start(LEUART_TypeDef *leuart, char *string, uint32_t string_len);
bool leuart_tx_busy(LEUART_TypeDef *leuart);

uint32_t leuart_status(LEUART_TypeDef *leuart);
void leuart_cmd_write(LEUART_TypeDef *leuart, uint32_t cmd_update);
void leuart_if_reset(LEUART_TypeDef *leuart);
void leuart_app_transmit_byte(LEUART_TypeDef *leuart, uint8_t data_out);
uint8_t leuart_app_receive_byte(LEUART_TypeDef *leuart);

#endif /* LEUART_H */
