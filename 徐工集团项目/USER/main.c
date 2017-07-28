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
#include "TIM.h"
#include "irda.h"
#include "msgagv.h"

#define _PROJECT_AGV 1//定义了工程内容，当定义为1后，该工程使用在AGV上，通过485通信与AGV主控系统进行信息交互，显示实时的AGV信息
//当定义为0时，该工程使用在充电桩上，可以实现充电桩控制、红外通信、LCD显示充电桩信息的功能
//IRDA、msgagv中也有相关定义

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
	spiInit();
	canInit();//CAN初始化N模块	
	usartConfig();
	nvicConfig();
	TIM2_Configuration();
    TIM3_Configuration();

	CanMsgQueue = xQueueCreate(17, sizeof(u32));
#if _PROJECT_AGV
	xTaskCreate((TaskFunction_t)spiLcdTask, \
		(const char*)"SPILcdTask", \
		(u16)_SPI_LCD_STK, \
		(void *)NULL, \
		(UBaseType_t)_SPI_LCD_PRIO, \
		(TaskHandle_t *)&SPILcdTaskHandle);
#endif
#if !_PROJECT_AGV
	xTaskCreate((TaskFunction_t)usartIrdaTask, \
		(const char*)"UsartIrdaTask", \
		(u16)_USART_IRDA_STK, \
		(void *)NULL, \
		(UBaseType_t)_USART_IRDA_PRIO, \
		(TaskHandle_t *)&UsartIrdaTaskHandle);
#endif
#if !_PROJECT_AGV
	xTaskCreate((TaskFunction_t)canChargeTask, \
		(const char*)"CanChargeTask", \
		(u16)_CAN_CHARGE_STK, \
		(void *)NULL, \
		(UBaseType_t)_CAN_CHARGE_PRIO, \
		(TaskHandle_t *)&CanChargeTaskHandle);
#endif
#if !_PROJECT_AGV
	xTaskCreate((TaskFunction_t)ttlLcdTask, \
		(const char*)"TtlLcdTask", \
		(u16)_TTL_LCD_STK, \
		(void *)NULL, \
		(UBaseType_t)_TTL_LCD_PRIO, \
		(TaskHandle_t *)&TtlLcdTaskHandle);
#endif
	vTaskStartScheduler();
}

//************************************
// FullName:  usartLcdTask
// Returns:   void
// Qualifier:LCD驱动任务，完成LCD的驱动与现实
// Parameter: void * pvParameter
//************************************
void spiLcdTask(void * pvParameter)
{
    AGV_SHOW_MSG_STRUCT msg;
	u8 showbyte = 0;
	float i = -10.2;
	vTaskDelay(200);
	lcdInit();
	initShowStrings();
	while (1)
	{
		msg = checkAgvMsg();
        lcdShowNumber(X2_Y5, msg.batteryLevel);
		lcdShowNumber(X3_Y5, msg.vola);
		lcdShowElectricity(msg.curr);
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
	u8 cmdRcv = 0;
	pvParameter = (void *)pvParameter;
	while (1)
	{
		cmdRcv = rcvMsgFromIrda();
        if(cmdRcv != NO_RX)
        {
			msgFeedBackToIrda(ChargerStatusBack, cmdRcv);
        }
		if (cmdRcv == CHECK_STATUS && ChargerStatusBack == 0x03)
		{
			agvConnectSetCmd();
		}
		else if (cmdRcv == REQUEST_CHARGE && ChargerStatusBack == 0x0a)
		{
			agvOpenSetCmd();
		}
		else if(cmdRcv == REQUEST_LEAVE)
		{
			agvConnectResetCmd();
			agvOpenResetCmd();
		}
		vTaskDelay(20);
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
	lcdShowString(X1_Y1, "哈工大机器人集团");
	lcdShowString(X2_Y2, "电量：");  					lcdShowString(X2_Y8, "%");
	lcdShowString(X3_Y2, "电压：");						lcdShowString(X3_Y8, "V");
	lcdShowString(X4_Y2, "电流：");						lcdShowString(X4_Y8, "A");
}
//************************************
// FullName:  lcdShowElectricity
// Returns:   void
// Qualifier:电流显示增加电流方向，用来判断充电、放电（用在AGV上时正为放电，负为充电状态）----------------------不同
// Parameter: float e
//************************************
void lcdShowElectricity(float e)
{
	lcdShowNumber(X4_Y5, e);
	if(e > 0)
	{
		lcdShowString(X4_Y1, "充");	
	}
	else
	{
		lcdShowString(X4_Y1, "放");	
	}

}

//************************************
// FunctionName:  ttlLcdTask
// Returns:   void
// Qualifier:TTL电平驱动LCD，该LCD会显示充电桩的实时信息
// Parameter: void * pvParameter
//************************************
void ttlLcdTask(void *pvParameter)
{
    float i = 0;
	vTaskDelay(2000);
	_TTL_LCD_CLR;
	vTaskDelay(500);
	_TTL_LCD_SHOW;
	vTaskDelay(2000);
	while (1)
	{
 		ttlLcdMsgSed(CHARGEVOL, (float)getVola()/128.0f);
 		ttlLcdMsgSed(CHARGECUR, (float)getCurr()/210.0f);
		ttlLcdMsgSed(CHARGETIME,  ChargerTimeCount/60.0f);
		ttlLcdMsgSed(MACHINESTATUS, ChargerStatusBack);
		ttlLcdMsgSed(BATTERY, isAgvOpenCmd());
		ttlLcdMsgSed(COOL, ChargerCloseCount);
        vTaskDelay(100);
	}
}
void msgAgvTask(void *pvParameter)
{
	while (1)
	{
		vTaskDelay(50);
	}
}


