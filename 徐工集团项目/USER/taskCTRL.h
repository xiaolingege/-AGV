#ifndef _TASK_CTRL_H
#define _TASK_CTRL_H
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

#define _USART_LCD_PRIO 2
#define _USART_IRDA_PRIO 2
#define _CAN_CHARGE_PRIO 1
#define _TTL_LCD_PRIO 2

#define _USART_LCD_STK 100
#define _USART_IRDA_STK 100
#define _CAN_CHARGE_STK 100
#define _TTL_LCD_STK 50

TaskHandle_t UsartLcdTaskHandle;
TaskHandle_t UsartIrdaTaskHandle;
TaskHandle_t CanChargeTaskHandle;
TaskHandle_t TtlLcdTaskHandle;

void usartLcdTask(void * pvParameter);
void usartIrdaTask(void * pvParameter);
void canChargeTask(void *pvParameter);


#endif // !_TASK_CTRL_H
