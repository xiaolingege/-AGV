#ifndef _IRDA_H
#define _IRDA_H

#include "stm32f10x.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

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
static bool isCheckHead(void);
static bool isCheckEOF(void);
void msgFeedBackToIrda(u8 status, u8 cmd);
static void chargerStatusBack(u8 status);
static void chargeRequestBack(u8 status);
static void agvLeaveBack(u8 status);
static void chargeOverCheckBack(u8 status);
#endif // !_IRDA_H




