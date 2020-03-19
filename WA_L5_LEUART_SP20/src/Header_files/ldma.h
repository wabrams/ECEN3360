#ifndef LDMA_H
#define LDMA_H

#define LDMA_CHANNEL_LEUART_RX	0
#define LDMA_CHANNEL_LEUART_TX	1

typedef enum
{
	LDMA_CHANNEL0,
	LDMA_CHANNEL1,
//	LDMA_CHANNEL2,
//	LDMA_CHANNEL3,
//	LDMA_CHANNEL4,
//	LDMA_CHANNEL5,
//	LDMA_CHANNEL6,
//	LDMA_CHANNEL7,
	LDMA_CHANNELS
} ldma_channel_t;

void ldma_open(ldma_channel_t);
void LDMA_IRQHandler(void);

#endif /* LDMA_H */
