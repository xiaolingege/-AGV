#ifndef _IRDA_H
#define _IRDA_H
#include "stm32f10x.h"
#include "usart.h"
#define _IRDA_RX_SIZE 6
typedef enum _IRDA_RX_TYPE
{
	NO_RX = 0,
	CHECK_STATUS = 1,
	REQUEST_CHARGE = 2,
	REQUEST_LEAVE = 3,
	QUERY = 4
}IRDA_RX_TYPE;
IRDA_RX_TYPE rcvMsgFromIrda(void);
bool isCheckHead(void);
bool isCheckEOF(void);
#endif // !_IRDA_H


