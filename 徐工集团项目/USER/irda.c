#include "irda.h"

extern volatile u32 ChargerCloseCount;
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

void msgFeedBackToIrda(u8 status, u8 cmd)
{
	switch (cmd)
	{
	case NO_RX:
		break;
	case CHECK_STATUS:
		chargerStatusBack(status);
		break;
	case REQUEST_CHARGE:
		chargeRequestBack(status);
		break;
	case REQUEST_LEAVE:
		agvLeaveBack(status);
		break;
	case QUERY:
		chargeOverCheckBack(status);
		break;
	default:
		break;
	}
}

static void chargerStatusBack(u8 status)
{
	u8 statusBackBuffer[6] = { 0xAA, 0x01, 0x00, 0xFF };
	switch (status)
	{
	case 0x01:
        statusBackBuffer[2] = ChargerCloseCount/10;
		//延时中
		statusBackBuffer[3] = 0x03;
		break;
	case 0x02:
		statusBackBuffer[3] = 0x01;
		//急停中
		break;
	case 0x03:
		statusBackBuffer[3] = 0x00;
		//等待连接中
		break;
	case 0x04:
		statusBackBuffer[3] = 0x02;
		//充电板异常
		break;
	case 0x05:
	case 0x06:
	case 0x07:
	case 0x08:
	case 0x09:
		//充电桩忙碌
		break;
	default:
		break;
	}
	statusBackBuffer[4] = (CRC16(statusBackBuffer, 4) & 0xff);
	statusBackBuffer[5] = (CRC16(statusBackBuffer, 4) >> 8);
	usart485Send(statusBackBuffer, 6);
}

static void chargeRequestBack(u8 status)
{
	u8 chargerRequestBackBuffer[6] = { 0xAA, 0x02, 0x00, 0xFF };
	if (status == 0x0a)
	{
		chargerRequestBackBuffer[3] = 0x01;
		chargerRequestBackBuffer[4] = (CRC16(chargerRequestBackBuffer, 4) & 0xff);
		chargerRequestBackBuffer[5] = (CRC16(chargerRequestBackBuffer, 4) >> 8);
		usart485Send(chargerRequestBackBuffer, 6);
	}
}

static void agvLeaveBack(u8 status)
{
	u8 agvLeaveBackBuffer[6] = { 0xAA, 0x03, 0x00, 0xFF };

    if (status == 0x09 || status == 0x01)
    {
        agvLeaveBackBuffer[3] = 0x01;
        agvLeaveBackBuffer[4] = (CRC16(agvLeaveBackBuffer, 4) & 0xff);
        agvLeaveBackBuffer[5] = (CRC16(agvLeaveBackBuffer, 4) >> 8);
        usart485Send(agvLeaveBackBuffer, 6);
    }
	
}

static void chargeOverCheckBack(u8 status)
{
	u8 chargeOverBackBuffer[6] = { 0xAA, 0x04, 0x00, 0xFF };
	if (status == 0x08 || status == 0x01)
	{
		chargeOverBackBuffer[3] = 0x01;
	}
	else
	{
		chargeOverBackBuffer[3] = 0x00;
	}
	chargeOverBackBuffer[4] = (CRC16(chargeOverBackBuffer, 4) & 0xff);
	chargeOverBackBuffer[5] = (CRC16(chargeOverBackBuffer, 4) >> 8);
	usart485Send(chargeOverBackBuffer, 6);
}



