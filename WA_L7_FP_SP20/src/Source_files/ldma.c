#include "ldma.h"
#include "em_ldma.h"
#include <stdbool.h>

//TODO: support multiple LDMA transfers at a time (as they are independent of each other, maybe keep track of what each channel is being used for or scheduled events?)

void ldma_open(ldma_channel_t channel)
{
	LDMA_Init_t ldma_init = LDMA_INIT_DEFAULT;
	LDMA_Init(&ldma_init);
}
void ldma_start(ldma_channel_t channel, LDMA_TransferCfg_t * ldma_transfercfg, LDMA_Descriptor_t * ldma_descriptor)
{
	LDMA_StartTransfer(channel, ldma_transfercfg, ldma_descriptor);
}

void LDMA_IRQHandler(void)
{
	__disable_irq();

	uint32_t iflags = (LDMA -> IFC = LDMA -> IF) & LDMA -> IEN;
	//FIXME: generic for only LEUART TX but eh
	if (iflags & (1 << LDMA_CHANNEL_LEUART_TX))
	{
		LDMA -> IEN &= ~(1 << LDMA_CHANNEL_LEUART_TX);
	}

	__enable_irq();
}
