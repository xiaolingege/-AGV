#include "charger.h"

extern xQueueHandle CanMsgQueue;
static u32 CloseDelay = 0;
static _CHANGER_STATUS ChangerStatus = CLOSE;


/*主要控制循环*/
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

static void setCloseDelay(void)
{
	CloseDelay = 150000;
}


static bool isOnConnect(void)
{
	return FALSE;
}

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
static bool isChangerNotGood(void)
{
	u32 RxMsg = 0;
	canMsgTx(0x01, 0x02, 0x03, 0x04);
	//发送查询命令
	//回馈命令等待并验证
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

static void openChanger(void)
{
	ChangerStatus = OPEN;
}
static void closeChanger(void)
{
	setCloseDelay();
	ChangerStatus = CLOSE;
}
static void setChanger(void)
{
  //发送配置命令并等待反馈
}

static bool isBattryVolGood(void)
{
	return FALSE;
}

static bool isCurGood(void)
{
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


