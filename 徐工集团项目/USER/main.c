#include "key.h"
#include "taskCTRL.h"
#include "SPI.h"
#include "lcd.h"
#include "stm32f10x_gpio.h"
#include "usart.h"
#include "NVICconfig.h"
#include "ttllcd.h"
#include "charger.h" 
#include "led.h"
#include "stdio.h"

xQueueHandle CanMsgQueue;
u8 ChargerStatusBack;
void initShowStrings(void);
void lcdShowElectricity(float e);

//************************************
// FunctionName:  main
// Returns:   int
// Qualifier:主函数，本工程为徐工集团AGV辅助设备控制程序，主要包括2种LCD的驱动、485通信，CAN通信，
//485与红外透传模块连接，进行无线通信,CAN与充电板连接，完成对充电板的开关控制、参数配置等功能
// Parameter: void
//************************************
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
	CanMsgQueue = xQueueCreate(16, sizeof(u32));
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

//************************************
// FullName:  usartLcdTask
// Returns:   void
// Qualifier:LCD驱动任务，完成LCD的驱动与现实
// Parameter: void * pvParameter
//************************************
void usartLcdTask(void * pvParameter)
{
	u8 showbyte = 0;
	float i = 10.2;
 	vTaskDelay(200);
// 	_TTL_LCD_CLR;
 	lcdInit();
// 	vTaskDelay(500);
// 	_TTL_LCD_SHOW;
// 	vTaskDelay(2000);
	initShowStrings();
	while (1)
	{
		sprintf((u8 *)&showbyte, "%x", ChargerStatusBack);
		lcdShowString(X2_Y6, &showbyte);
		//lcdShowNumber(X2_Y5, i);
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

//************************************
// FullName:  usartIrdaTask
// Returns:   void
// Qualifier:完成通过RS485红外透传通信
// Parameter: void * pvParameter
//************************************
void usartIrdaTask(void * pvParameter)
{
	pvParameter = (void *)pvParameter;
	while (1)
	{
		LED2(0);
		usart485Send((u8 *)"Helloworld\r\n", 12);
		LED2(1);
		vTaskDelay(1000);
	}
}

//************************************
// FullName:  canChargeTask
// Returns:   void
// Qualifier:通过can通信控制充电板任务
// Parameter: void * pvParameter
//************************************
void canChargeTask(void *pvParameter)
{
	u8 led1Flag = 0;
	pvParameter = (void *)pvParameter;
	while (1)
	{
		led1Flag ^= 1;
		LED1(led1Flag);
			ChargerStatusBack = chargerCTRLLoop();

		vTaskDelay(500);
	}
}

//************************************
// FullName:  initShowStrings
// Returns:   void
// Qualifier:12864开机显示基本信息（lables）
// Parameter: void
//************************************
void initShowStrings(void)
{
	lcdShowString(X1_Y1,(u8 *)"哈工大机器人集团");
	lcdShowString(X2_Y2,(u8 *)"状态：");  					//lcdShowString(X2_Y8,(u8 *) "%");
	lcdShowString(X3_Y2,(u8 *)"电压：");						lcdShowString(X3_Y8,(u8 *)"V");
	lcdShowString(X4_Y2,(u8 *)"电流：");						lcdShowString(X4_Y8,(u8 *) "A");
}
//************************************
// FullName:  lcdShowElectricity
// Returns:   void
// Qualifier:电流显示增加电流方向，用来判断充电、放电（用在AGV上时正为放电，负为充电状态）
// Parameter: float e
//************************************
void lcdShowElectricity(float e)
{
	lcdShowNumber(X4_Y5, e);
	if(e > 0)
	{
		lcdShowString(X4_Y5, (u8 *)"+");	
	}
	else
	{
		lcdShowString(X4_Y5, (u8 *) "-");	
	}

}

