#ifndef _TASK_CTRL_H
#define _TASK_CTRL_H
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

#define _SPI_LCD_PRIO 2
#define _USART_IRDA_PRIO 2
#define _CAN_CHARGE_PRIO 1
#define _TTL_LCD_PRIO 2

#define _SPI_LCD_STK 100
#define _USART_IRDA_STK 500
#define _CAN_CHARGE_STK 300
#define _TTL_LCD_STK 50

TaskHandle_t SPILcdTaskHandle;
TaskHandle_t UsartIrdaTaskHandle;
TaskHandle_t CanChargeTaskHandle;
TaskHandle_t TtlLcdTaskHandle;

void spiLcdTask(void * pvParameter);
void usartIrdaTask(void * pvParameter);
void canChargeTask(void *pvParameter);
void ttlLcdTask(void *pvParameter);



#endif // !_TASK_CTRL_H
