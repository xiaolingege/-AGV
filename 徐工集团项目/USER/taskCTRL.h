#ifndef _TASK_CTRL_H
#define _TASK_CTRL_H
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

#define _USART_LCD_PRIO 1
#define _TTL_LCD_PRIO 2

#define _USART_LCD_STK 100
#define _TTL_LCD_STK 50

TaskHandle_t UsartLcdTaskHandle;
TaskHandle_t TtlLcdTaskHandle;

void usartLcdTask(void * pvParameter);


#endif // !_TASK_CTRL_H
