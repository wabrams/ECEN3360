/**
 * @file leuart.h
 **/

#ifndef LEUART_H
#define LEUART_H

#include "em_leuart.h"
#include "sleep_routines.h"

#define LEUART_TX_EM_BLOCK EM3 	/**< lowest energy mode LEUART can TX in **/
#define LEUART_RX_EM_BLOCK EM3	/**< lowest energy mode LEUART can RX in **/

/**
 * @brief
 * LEUART TX State Machine Enumeration
 **/
typedef enum
{
	LEUART_STATE_TX_IDLE,			/**< idle state **/
	LEUART_STATE_TX_TRANSMIT,		/**< transmit state **/
	LEUART_STATE_TX_DONE,			/**< completed state **/
} leuart_txstate_t;

typedef enum
{
	LEUART_STATE_RX_IDLE,
	LEUART_STATE_RX_RECEIVE,
} leuart_rxstate_t;

/**
 * @brief
 * Structure used for leuart_open() to pass all relevant values
 **/
typedef struct
{
	// LEUART_Init_TypeDef
	uint32_t					baudrate;		/**< LEUART baud rate to be used **/
	LEUART_Databits_TypeDef		databits;		/**< LEUART data bits to be used **/
	LEUART_Enable_TypeDef		enable;			/**< LEUART enable post init **/
	LEUART_Parity_TypeDef 		parity;			/**< LEUART parity setting to be used **/
	uint32_t 					refFreq;		/**< LEUART reference frequency **/
	LEUART_Stopbits_TypeDef		stopbits;		/**< LEUART stop bits to be used **/
	// LEUART RX and TX RouteLoc
	uint32_t					rx_rloc;		/**< LEUART RX pin routing **/
	uint32_t					rx_rpen;		/**< LEUART RX pin enabling **/
	uint32_t					tx_rloc;		/**< LEUART TX pin routing **/
	uint32_t					tx_rpen;		/**< LEUART TX pin enabling **/
	// LEUART CMD Vars
	bool						rx_en;			/**< LEUART RX enable **/
	bool						tx_en;			/**< LEUART TX enable **/
	bool						rxblocken;		/**< LEUART RXBLOCK enable (for use with SIGF or STARTF) **/
	// START and SIG Variables
	bool						sfubrx;			/**< LEUART StartF UnBlock RX **/
	bool						startframe_en;	/**< LEUART STARTF enable **/
	char						startframe;		/**< LEUART STARTF character (8 bit only) **/
	bool						sigframe_en;	/**< LEUART SIGF enable **/
	char						sigframe;		/**< LEUART SIGF character (8 bit only) **/
	// RX
	int							rxlen;
	char *						rxstring;
	// DMA
	bool 						rx_dma;			/**< TODO: Unused. Enables RX DMA in EM2 **/
	bool 						tx_dma;			/**< TODO: Unused. Enables TX DMA in EM2 **/
	// Scheduler Event IDs
	uint32_t					rx_done_evt;	/**< Scheduler ID for RX Done event **/
	uint32_t					tx_done_evt;	/**< Scheduler ID for TX Done event **/
} LEUART_OPEN_STRUCT;

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
