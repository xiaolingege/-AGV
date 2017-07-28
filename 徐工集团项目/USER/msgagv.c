#include "msgagv.h"
#define _PROJECT_AGV_MSG 1
#define _MSG_AGV_SIZE 20
static u8 AgvMsgRxBuf[_MSG_AGV_SIZE] = { 0 };
static u8 AgvMsgRxCount = 0;

#if _PROJECT_AGV_MSG
void USART3_IRQHandler(void)
{//  unsigned int i;
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
		// 	if(!(USART_GetITStatus(USART2,USART_IT_RXNE))); 
	{
		AgvMsgRxBuf[AgvMsgRxCount++] = USART_ReceiveData(USART3);
		if (AgvMsgRxBuf[0] != 0x55)
		{
			AgvMsgRxCount = 0;
		}
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}
#endif

static AGV_SHOW_MSG_STRUCT msgReturn;

AGV_SHOW_MSG_STRUCT checkAgvMsg(void)
{
	u8 i = 0;
    u8 backMsg[5] = {0x55, 0xAA, 0xFF};
	union _msgUnion
	{
		u8 msgArr[12];
		AGV_SHOW_MSG_STRUCT msg;
	}msgUnion;
	if (AgvMsgRxCount > 15 )
	{
        if(AgvMsgRxBuf[1] == 0xAA && isCheckTail())
        {
            for (i = 0; i < 12; i++)
            {
                msgUnion.msgArr[i] = AgvMsgRxBuf[i + 2];
            }
        msgReturn = msgUnion.msg;
        backMsg[3] = CRC16(backMsg, 3) & 0xff;
        backMsg[4] = (CRC16(backMsg, 3)>>8) & 0xff;
        usart485Send(backMsg, 5);
        }
		AgvMsgRxCount = 0;
	}

	return msgReturn;
}

static bool isCheckTail(void)
{
	u16 crcCheck = 0;
	crcCheck = CRC16(AgvMsgRxBuf, 14);
	if ((crcCheck & 0xff) == AgvMsgRxBuf[14] && (((crcCheck >> 8) & 0xff) == AgvMsgRxBuf[15]))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

