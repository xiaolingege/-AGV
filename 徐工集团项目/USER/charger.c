#include "charger.h"
#include "led.h"
extern xQueueHandle CanMsgQueue;
static u32 CloseDelay = 0;
static _CHANGER_STATUS ChangerStatus = CLOSE;

static bool ChangerOverFlag = FALSE;

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
		return 0x04;//充电板异常(连接检测)
	}
	else if (!isBattryVolGood())
	{
		return 0x05;//电池电压异常
	}
	else 
	{
		if (CLOSE == checkChangerStatusOpen())//充电板未打开，打开充电板
		{
			setChanger();
			openChanger();
			return 0x06;//正在打开充电桩
		}
		else if (!isCurGood())//充电板已经打开，查询参数
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
	u8 i = 0;
	u32 RxMsg = 0;
	canMsgTx(_CONNECT_CMD);
	for ( i = 0; i < 16; i++)
	{
		if (pdFALSE == xQueueReceive(CanMsgQueue, &RxMsg, 50))
		{

			return TRUE;
		}
	}
	return FALSE;
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
	canMsgTx(_OPEN_CMD);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _OPEN_CMD_BACK)
	{
		return;
	}
	canMsgTx(_OPEN_DATA);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _OPEN_DATA_BACK)
	{
		return;
	}
	clrChangerOver();
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
	canMsgTx(_CLOSE_CMD);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _CLODE_CMD_BACK)
	{
		return;
	}
	canMsgTx(_CLOSE_DATA);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _CLOSE_DATA_BACK)
	{
		return;
	}
	setCloseDelay();
	ChangerStatus = CLOSE;
}
//************************************
// FunctionName:  setChanger
// Returns:   void
// Qualifier:发送命令配置当前充电板参数
// Parameter: void
//************************************
static void setChanger(void)
{
	//发送配置命令并等待反馈
	u32 RxMsg = 0;
	canMsgTx(_SET_VOLATE_CMD);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _SET_VOLATE_CMD_BACK)
	{
		return;
	}
	canMsgTx(_SET_VOLATE_DATA);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _SET_VOLATE_DATA_BACK)
	{
		return;
	}
	canMsgTx(_SET_CURR_CMD);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _SET_CURR_CMD_BACK)
	{
		return;
	}
	canMsgTx(_SET_CURR_DATA);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _SET_CURR_DATA_BACK)
	{
		return;
	}

}

//************************************
// FunctionName:  isBattryVolGood
// Returns:   bool
// Qualifier:检查电池电压是否满足充电要求
// Parameter: void
//************************************
static bool isBattryVolGood(void)
{
	u32 RxMsg = 0;
	canMsgTx(_CHECK_BATTRY_CMD);
	LED2(0);
	xQueueReceive(CanMsgQueue, &RxMsg, 80);
	if (RxMsg != _CHECK_BATTRY_CMD_BACK)
	{
		LED2(1);
		return FALSE;
	}
	canMsgTx(_CHECK_BATTY_DATA);
	if (pdFALSE == xQueueReceive(CanMsgQueue, &RxMsg, 50))
	{
		return FALSE;
	}
	return TRUE;
}

//************************************
// FunctionName:  isCurGood
// Returns:   bool
// Qualifier:检查当前充电电流有没有异常现象，并检查充电电流是否满足充电结束要求
// Parameter: void
//************************************
static bool isCurGood(void)
{
	u32 RxMsg = 0;
	canMsgTx(_CHECK_CURR_CMD);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _CHECK_CURR_CMD_BACK)
	{
		return FALSE;
	}
	canMsgTx(_CHECK_CURR_DATA);
	if (pdFALSE == xQueueReceive(CanMsgQueue, &RxMsg, 50))
	{
		return FALSE;
	}
	else
	{
		if (RxMsg < 0xf13100ff)//设定小于某值时，充电结束，充电开始时设定充电未结束
		{
			setChangerOver();
		}
		return TRUE;
	}
}

//************************************
// FunctionName:  isOverCharge
// Returns:   bool
// Qualifier:检查是否已经充电完成
// Parameter: void
//************************************
static bool isOverCharge(void)
{
	return ChangerOverFlag;
}

//************************************
// FunctionName:  isEmmergency
// Returns:   bool
// Qualifier:检测急停按钮是否按下
// Parameter: void
//************************************
static bool isEmmergency(void)
{
	return FALSE;
}

//************************************
// FunctionName:  checkChangerStatusOpen
// Returns:   _CHANGER_STATUS
// Qualifier:查询当前充电机状态
// Parameter: void
//************************************
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

//************************************
// FunctionName:  setChangerOver
// Returns:   void
// Qualifier:调用该函数设定充电结束，此时充电电流较小
// Parameter: void
//************************************
static void setChangerOver(void)
{
	ChangerOverFlag = TRUE;
}

//************************************
// FunctionName:  clrChangerOver
// Returns:   void
// Qualifier:开始充电，如果上次充电正常结束，本次充电刷新标志位，显示为未充电结束
// Parameter: void
//************************************
static void clrChangerOver(void)
{
	ChangerOverFlag = FALSE;
}



