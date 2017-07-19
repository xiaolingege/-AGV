#include "irda.h"



volatile u8 IrdaRxBufferCount = 0;
u8 IrdaRxBuffer[_IRDA_RX_SIZE] = { 0 };

IRDA_RX_TYPE rcvMsgFromIrda(void)
{
	IRDA_RX_TYPE cmdRteturn = NO_RX;
	if (IrdaRxBufferCount <= 5)
	{
		cmdRteturn = NO_RX;
	}
	else if(isCheckHead()&&isCheckEOF())
	{
		switch (IrdaRxBuffer[1])
		{
		case CHECK_STATUS:
            cmdRteturn = CHECK_STATUS;
			break;
		case REQUEST_CHARGE:
            cmdRteturn = REQUEST_CHARGE;
			break;
		case REQUEST_LEAVE:
            cmdRteturn = REQUEST_LEAVE;
			break;
		case QUERY:
            cmdRteturn = QUERY;
			break;
		default:
			break;
		}
        IrdaRxBufferCount = 0;
	}
    else 
    {
        IrdaRxBufferCount = 0;
    }
	return cmdRteturn;
}
bool isCheckHead(void)
{
	if (IrdaRxBuffer[0] == 0x55)
	{
		return TRUE;
	}
	return FALSE;
}
bool isCheckEOF(void)
{
	u16 crcCheck = 0;
	crcCheck = CRC16(IrdaRxBuffer, 4);
	if ((crcCheck&0xff) == IrdaRxBuffer[4] && ((crcCheck>>8) & 0xff) == IrdaRxBuffer[5])
	{
		return TRUE;
	}
    else
    {
        	return FALSE;
    }
}

void USART3_IRQHandler(void)
{//  unsigned int i;
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
		// 	if(!(USART_GetITStatus(USART2,USART_IT_RXNE))); 
	{
		IrdaRxBuffer[IrdaRxBufferCount++] = USART_ReceiveData(USART3);
        if(IrdaRxBuffer[0] != 0x55)
        {
            IrdaRxBufferCount = 0;
        }
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}

