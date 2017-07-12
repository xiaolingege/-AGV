#include "key.h"
#include "taskCTRL.h"
#include "SPI.h"
#include "lcd.h"
#include "stm32f10x_gpio.h"
#include "usart.h"
#include "NVICconfig.h"
#include "ttllcd.h"
void initShowStrings(void);
void lcdShowElectricity(float e);
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
	xTaskCreate((TaskFunction_t)usartLcdTask,\
		(const char*)"UsartLcdTask",\
		(u16)_USART_LCD_STK,\
		(void *)NULL,\
		(UBaseType_t)_USART_LCD_PRIO,\
		(TaskHandle_t *)&UsartLcdTaskHandle);
	xTaskCreate((TaskFunction_t)usartIrdaTask,\
		(const char*)"UsartIrdaTask",\
		(u16)_USART_IRDA_STK,\
		(void *)NULL,\
		(UBaseType_t)_USART_IRDA_PRIO,\
		(TaskHandle_t *)&UsartIrdaTaskHandle);
	xTaskCreate((TaskFunction_t)canChargeTask,\
		(const char*)"CanChargeTask",\
		(u16)_CAN_CHARGE_STK,\
		(void *)NULL,\
		(UBaseType_t)_CAN_CHARGE_PRIO,\
		(TaskHandle_t *)&CanChargeTaskHandle);
	vTaskStartScheduler();
}

void usartLcdTask(void * pvParameter)
{
	float i = 10.2;
	vTaskDelay(2000);
	_TTL_LCD_CLR;
	lcdInit();
	vTaskDelay(500);
	_TTL_LCD_SHOW;
	vTaskDelay(2000);
	initShowStrings();
	while (1)
	{
		lcdShowNumber(X2_Y5, i);
		lcdShowNumber(X3_Y5, i);
		lcdShowElectricity(i);
		ttlLcdMsgSed(CHARGEVOL, 10);
		ttlLcdMsgSed(CHARGECUR, 0);
		ttlLcdMsgSed(CHARGETIME, 0);
		ttlLcdMsgSed(MACHINESTATUS, 0);
		ttlLcdMsgSed(BATTERY, TRUE);
		ttlLcdMsgSed(COOL, FALSE);
	}
}

void usartIrdaTask(void * pvParameter)
{
	pvParameter = (void *)pvParameter;
	while (1)
	{
		usart485Send((u8 *)"hello\r\n", 7);
		vTaskDelay(200);
	}
}

void canChargeTask(void *pvParameter)
{
	pvParameter = (void *)pvParameter;
	while (1)
	{
		canMsgTx(10, 20);
		vTaskDelay(200);
	}
}

void initShowStrings(void)
{
	lcdShowString(X1_Y1, "哈工大机器人集团");
	lcdShowString(X2_Y2, "电量：");  					lcdShowString(X2_Y8, "%");
	lcdShowString(X3_Y2, "电压：");						lcdShowString(X3_Y8, "V");
	lcdShowString(X4_Y2, "电流：");						lcdShowString(X4_Y8, "A");
}
void lcdShowElectricity(float e)
{
	lcdShowNumber(X4_Y5, e);
	if(e > 0)
	{
		lcdShowString(X4_Y5, "+");	
	}
	else
	{
		lcdShowString(X4_Y5, "-");	
	}

}

