#ifndef _MSG_AGV_H
#define _MSG_AGV_H

#include "stm32f10x.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

#include "usart.h"

typedef struct _AGV_SHOW_MSG_STRUCT
{
	float curr;
	float vola;
	float batteryLevel;

}AGV_SHOW_MSG_STRUCT;
AGV_SHOW_MSG_STRUCT checkAgvMsg(void);


static bool isCheckTail(void);
#endif // !_MSG_AGV_H

