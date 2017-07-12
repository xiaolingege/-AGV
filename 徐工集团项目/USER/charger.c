#include "charger.h"

extern xQueueHandle CanMsgQueue;
static u32 CloseDelay = 0;
static _CHANGER_STATUS ChangerStatus = CLOSE;

/*主要控制循环*/
//************************************
// FullName:  changerCTRLLoop
// Returns:   extern u8
// Qualifier:完成充电机控制主要流程
// Parameter: void
//************************************
extern u8 changerCTRLLoop(void)
{
	if (isCloseDelay())
	{
		return 0x01;//正在延时关闭
	}
	else if(isEmmergency())
	{
		return 0x02;//按下急停
	}
	else if (!isOnConnect())
	{
		return 0x03;//无车体连接
	}
	else if (isChangerNotGood())
	{
		return 0x04;//充电板异常
	}
	else if (!isBattryVolGood())
	{
		return 0x05;//电池电压异常
	}
	else 
	{
		if (CLOSE == checkChangerStatusOpen())
		{
			setChanger();
			openChanger();
			return 0x06;//正在打开充电桩
		}
		else if (!isCurGood())
		{
			closeChanger();
			return 0x07;//电流异常关闭
		}
		else if (isOverCharge())
		{
			closeChanger();//充电完成关闭
			return 0x08;
		}
	}
	return 0x09; //正常充电中
}

//************************************
// FullName:  setCloseDelay
// Returns:   void
// Qualifier:设置延时，延时时间为3s
// Parameter: void
//************************************
static void setCloseDelay(void)
{
	CloseDelay = 150000;
}


//************************************
// FullName:  isOnConnect
// Returns:   bool
// Qualifier:判断是否已经与AGV对接成功
// Parameter: void
//************************************
static bool isOnConnect(void)
{

	return TRUE;
}

//************************************
// FullName:  isCloseDelay
// Returns:   bool
// Qualifier:判断是否正在关机延时
// Parameter: void
//************************************
static bool isCloseDelay(void)
{
	if (0 == CloseDelay)
	{
		return FALSE;
	}
	else
	{
		CloseDelay--;
		return TRUE;
	}
}

/*充电板查询正常*/
//************************************
// FullName:  isChangerNotGood
// Returns:   bool
// Qualifier:有效充电板
// Parameter: void
//************************************
static bool isChangerNotGood(void)
{
	//发送查询命令
	//回馈命令等待并验证
	u32 RxMsg = 0;
	canMsgTx(0x01, 0x02, 0x03, 0x04);
	xQueueReceive(CanMsgQueue, &RxMsg, 10);
	if (RxMsg == 0x01)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//************************************
// FullName:  openChanger
// Returns:   void
// Qualifier:打开充电机，等待反馈命令
// Parameter: void
//************************************
static void openChanger(void)
{
	u32 RxMsg = 0;
	canMsgTx(0xf1, 0x02, 0x04, 0x87);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != 0xf1020487)
	{
		return;
	}
	canMsgTx(0xf1, 0x12, 0x00, 0x00);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != 0xf1120000)
	{
		return;
	}
	ChangerStatus = OPEN;
}

//************************************
// FunctionName:  closeChanger
// Returns:   void
// Qualifier:关闭充电机，设置延时
// Parameter: void
//************************************
static void closeChanger(void)
{
	u32 RxMsg = 0;
	canMsgTx(0xf1, 0x02, 0x04, 0x87);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != 0xf1020487)
	{
		return;
	}
	canMsgTx(0xf1, 0x12, 0x00, 0x01);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != 0xf1120001)
	{
		return;
	}
	setCloseDelay();
	ChangerStatus = CLOSE;
}
static void setChanger(void)
{
	//发送配置命令并等待反馈
	u32 RxMsg = 0;
	canMsgTx(0xf1, 0x02, 0x04, 0x21);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != 0xf1020421)
	{
		return;
	}
	canMsgTx(0xf1, 0x12, 0x0e, 0xb3);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != 0xf1120eb3)
	{
		return;
	}
	canMsgTx(0xf1, 0x02, 0x04, 0x23);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != 0xf1020423)
	{
		return;
	}
	canMsgTx(0xf1, 0x12, 0x0a, 0x00);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != 0xf1120a00)
	{
		return;
	}

}

static bool isBattryVolGood(void)
{
	u32 RxMsg = 0;
	canMsgTx(0xf1, 0x02, 0x04, 0x31);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != 0xf1020431)
	{
		return FALSE;
	}
	canMsgTx(0xf1, 0x12, 0x04, 0x31);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);

	return TRUE;
}

static bool isCurGood(void)
{
	u32 RxMsg = 0;
	canMsgTx(0xf1, 0x02, 0x04, 0x30);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != 0xf1020430)
	{
		return FALSE;
	}
	canMsgTx(0xf1, 0x12, 0x04, 0x30);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	return TRUE;
}

static bool isOverCharge(void)
{
	return FALSE;
}

static bool isEmmergency(void)
{
	return FALSE;
}

static _CHANGER_STATUS checkChangerStatusOpen(void)
{
	return ChangerStatus;
}

/* USB中断和CAN接收中断服务程序，USB跟CAN公用I/O，这里只用到CAN的中断。 */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;
	u32 RxData = 0;
	portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	RxMessage.StdId = 0x00;
	RxMessage.ExtId = 0x00;
	RxMessage.IDE = 0;
	RxMessage.DLC = 0;
	RxMessage.FMI = 0;
	RxMessage.Data[0] = 0x00;
	RxMessage.Data[1] = 0x00;
	RxMessage.Data[2] = 0x00;
	RxMessage.Data[3] = 0x00;
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage); //接收FIFO0中的数据  
	RxData = RxMessage.Data[0] << 24 | RxMessage.Data[1] << 16 | RxMessage.Data[2] << 8 | RxMessage.Data[0];
	if (RxMessage.StdId == 0x181ff502)
	{
		xQueueSendFromISR(CanMsgQueue,&RxData, &xHigherPriorityTaskWoken);
	}
}


