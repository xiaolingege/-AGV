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
	KeyInit();	  //按键管脚初始化
	LED_GPIO_Config();//LED管脚初始化
	CAN_GPIO_Config();//CAN管脚初始化
//	CAN_NVIC_Configuration(); //CAN中断初始化   
	CAN_INIT();//CA初始化N模块	
	usartConfig();
	spiInit();
	nvicConfig();
	xTaskCreate((TaskFunction_t)usartLcdTask,
		(const char*)"UsartLcdTask",
		(u16)_USART_LCD_STK,
		(void *)NULL,
		(UBaseType_t)_USART_LCD_PRIO,
		(TaskHandle_t *)&UsartLcdTaskHandle);
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
	//	USART_OUT(USART2, "DS32(174,168,'10.1V',1);\r\n", 26);
	//	usart485Send("hello\r\n", 7);
		can_tx(10,20);
		vTaskDelay(200);
	}
}


