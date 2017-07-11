#include "key.h"
#include "taskCTRL.h"
#include "SPI.h"
#include "lcd.h"
#include "stm32f10x_gpio.h"
#include "usart.h"
#include "NVICconfig.h"

int main(void)
{
	SystemInit();
	keyInit();	  //按键管脚初始化
	ledGpioConfig();//LED管脚初始化
	canGpioConfig();//CAN管脚初始化
	canInit();//CAN初始化N模块	
	usartConfig();
	spiInit();
	nvicConfig();
	xTaskCreate((TaskFunction_t)usartLcdTask,
		(const char*)"UsartLcdTask",
		(u16)_USART_LCD_STK,
		(void *)NULL,
		(UBaseType_t)_USART_LCD_PRIO,
		(TaskHandle_t *)&UsartLcdTaskHandle);
	xTaskCreate((TaskFunction_t)usartIrdaTask,
		(const char*)"UsartIrdaTask",
		(u16)_USART_IRDA_STK,
		(void *)NULL,
		(UBaseType_t)_USART_IRDA_PRIO,
		(TaskHandle_t *)&UsartIrdaTaskHandle);
	xTaskCreate((TaskFunction_t)canChargeTask,
		(const char*)"CanChargeTask",
		(u16)_CAN_CHARGE_STK,
		(void *)NULL,
		(UBaseType_t)_CAN_CHARGE_PRIO,
		(TaskHandle_t *)&CanChargeTaskHandle);
	vTaskStartScheduler();
}

void usartLcdTask(void * pvParameter)
{
	vTaskDelay(2000);
	USART_OUT(USART2, "SPG(2)\r\n", 8);
	vTaskDelay(500);
	USART_OUT(USART2, "SPG(3)\r\n", 8);
	vTaskDelay(2000);
	while (1)
	{
		USART_OUT(USART2, "DS32(174,168,'10.1V',1);\r\n", 26);
		vTaskDelay(200);
	}
}

void usartIrdaTask(void * pvParameter)
{
	pvParameter = (void *)pvParameter;
	while (1)
	{
		usart485Send("hello\r\n", 7);
		vTaskDelay(200);
	}
}

void canChargeTask(void *pvParameter)
{
	pvParameter = (void *)pvParameter;
	while (1)
	{
		can_tx(10, 20);
		vTaskDelay(200);
	}
}
