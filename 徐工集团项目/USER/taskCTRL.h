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
#define _MSG_AGV_PRIO 1

#define _SPI_LCD_STK 400
#define _USART_IRDA_STK 400
#define _CAN_CHARGE_STK 800
#define _TTL_LCD_STK 400
#define _MSG_AGV_STK 400

TaskHandle_t SPILcdTaskHandle;
TaskHandle_t UsartIrdaTaskHandle;
TaskHandle_t CanChargeTaskHandle;
TaskHandle_t TtlLcdTaskHandle;
TaskHandle_t MsgAgvTaskHandle;

void spiLcdTask(void * pvParameter);
void usartIrdaTask(void * pvParameter);
void canChargeTask(void *pvParameter);
void ttlLcdTask(void *pvParameter);
void msgAgvTask(void *pvParameter);




#endif // !_TASK_CTRL_H
