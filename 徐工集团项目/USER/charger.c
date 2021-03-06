#include "charger.h"
#include "led.h"
#include "key.h"
#include "TIM.h"

#define _CLOSE_CUR 0x100ul//充电结束电流

#define _MSG_RCV_DELAY 100

#define clrChargerOver()\
		ChargerOverFlag = FALSE

#define isButtonPress()\
		Bit_RESET == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)

extern xQueueHandle CanMsgQueue;
extern xQueueHandle CloseTimeQueue;
static _CHARGER_STATUS_TYPE ChargerStatus = CLOSE;
static bool ChargerOverFlag = FALSE;
static bool ChargerGoodStatus = FALSE;
static CHARGER_MOUDLE_TYPE MoudleGoodFlag = {FALSE, FALSE };
static CHARGER_MOUDLE_TYPE MoudleOpenFlag = {FALSE, FALSE };
static u16 RcvCurr = 0x00;
static u16 RcvVola = 0x00;
static bool AgvSetChargerOpen = FALSE;
static bool AgvConnectFlag = FALSE;


/*主要控制循环*/
//************************************
// FullName:  changerCTRLLoop
// Returns:   extern u8
// Qualifier:完成充电机控制主要流程
// Parameter: void
//************************************
extern u8 chargerCTRLLoop(void)
{
	u8 controlBack = 0;
	if (isCloseDelay())
	{
		return 0x01;//正在延时关闭
	}
	else if (isEmmergency())
	{
		if (checkChangerStatusOpen() == OPEN)
		{
			while( 0 != closeCharger());
		}
		return 0x02;//按下急停
	}
	else if (isChargerNotGood())
	{
		return 0x04;//充电板异常(连接检测)
	}
	else if (!isOnConnect())
	{
		if (checkChangerStatusOpen() == OPEN)
		{
			while (0 != closeCharger());
		}
		return 0x03;//无车体连接
	}
	else if(!AgvSetChargerOpen)
	{
		if (checkChangerStatusOpen()== OPEN)
		{
			while (0 != closeCharger());
		}
		return 0x0a;
	}
	else if (!isBattryVolGood())
	{
		if (checkChangerStatusOpen() == OPEN)
		{
			while (0 != closeCharger());
		}
		return 0x05;//电池电压异常
	}
	else
	{
		controlBack = controlStrtagy();
		if (controlBack)
		{
			return controlBack;
		}
	}
	return 0x09; //正常充电中
}

//************************************
// FunctionName:  controlStrtagy
// Returns:   s8
// Qualifier:控制策略，前半小时双充电板充电，半小时后按照半小时，轮流交替两个充电板充电
// Parameter: void
//************************************
static s8 controlStrtagy(void)
{
	if (CLOSE == checkChangerStatusOpen())//充电板未打开，打开充电板
	{
		while (0 != setCharger());
		while (0 != openCharger());
		return 0x06;//正在打开充电桩
	}
	else if (!isCurGood())//充电板已经打开，查询参数
	{
		closeCharger();
		//while (0 != closeCharger());
		return 0x07;//电流异常关闭
	}
	else if (isOverCharge())
	{
		while (0 != closeCharger());//充电完成关闭
		return 0x08;
	}
	else if ((ChargerTimeCount >= 1800) && (ChargerTimeCount % 600 == 0))
	{
		if (MoudleOpenFlag.module0 == TRUE)
		{
			closeMoudle(module0);
			MoudleOpenFlag.module0 = FALSE;
			openMdoule(module2);
			MoudleOpenFlag.module2 = TRUE;
		}
		else if (MoudleOpenFlag.module2 == TRUE)
		{
			closeMoudle(module2);
			MoudleOpenFlag.module2 = FALSE;
			openMdoule(module0);
			MoudleOpenFlag.module0 = TRUE;
		}
        vTaskDelay(30000);
	}
	return 0x00;
}

//************************************
// FullName:  isOnConnect
// Returns:   bool
// Qualifier:判断是否已经与AGV对接成功
// Parameter: void
//************************************
static bool isOnConnect(void)
{
	return AgvConnectFlag;
}

//************************************
// FullName:  isCloseDelay
// Returns:   bool
// Qualifier:判断是否正在关机延时
// Parameter: void
//************************************
static bool isCloseDelay(void)
{
	if (ChargerStatus == OPEN)
	{
		return FALSE;
	}
	else if (0 == ChargerCloseCount)
	{
		STOP_CLOSE;
		return FALSE;
	}
	else
	{
		vTaskDelay(500);
		return TRUE;
	}
}

//************************************
// FunctionName:  isModuleConnect
// Returns:   bool
// Qualifier:查询两个模块的连接情况，判断有几个充电板可用
// Parameter: _CHANGER_MODULE moudle
//************************************
static bool isModuleConnect(_CHANGER_MODULE moudle)
{
	u8 i = 0;
    u32 RxMsg = 0;
	sendCmdToCharger(moudle, _CONNECT_CMD);
	while (pdTRUE == xQueueReceive(CanMsgQueue, &RxMsg, 100))
	{
		i++;
	}
	if (i == 0)
	{
		return FALSE;
	}
	else
	{
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
bool isChargerNotGood(void)
{
	//发送查询命令
	//回馈命令等待并验证
	if (ChargerGoodStatus == TRUE)
	{
		return FALSE;
	}

	if (isModuleConnect(module0))//判断模块1是否连接正常
	{
		MoudleGoodFlag.module0 = TRUE;
	}
	if (isModuleConnect(module2))//判断模块2是否连接正常
	{
		MoudleGoodFlag.module2 = TRUE;
	}
	if ((MoudleGoodFlag.module0 | MoudleGoodFlag.module2) == FALSE)
	{
		ChargerGoodStatus = FALSE;
		return TRUE;
	}
	ChargerGoodStatus = TRUE;
	return FALSE;
}

//************************************
// FunctionName:  openMdoule
// Returns:   s8
// Qualifier:根据参数打开对应的充电板模块
// Parameter: _CHANGER_MODULE module
//************************************
static s8 openMdoule(_CHANGER_MODULE module)
{
	u32 RxMsg = 0;
	canMsgTx(module, _OPEN_CMD);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _OPEN_CMD_BACK)
	{
		return -1;
	}
	vTaskDelay(10);
	canMsgTx(module, _OPEN_DATA);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _OPEN_DATA_BACK)
	{
		return -2;
	}
    return 0;
}

//************************************
// FullName:  openChanger
// Returns:   void
// Qualifier:打开充电机，等待反馈命令
// Parameter: void
//************************************
static s8 openCharger(void)
{
	if (MoudleGoodFlag.module0 == TRUE)
	{
		openMdoule(module0);
		MoudleOpenFlag.module0 = TRUE;
	}
	if (MoudleGoodFlag.module2 == TRUE)
	{
		openMdoule(module2);
		MoudleOpenFlag.module2 = TRUE;
	}

	clrChargerOver();
	ChargerStatus = OPEN;
	START_TIME;
	return 0;
}

//************************************
// FunctionName:  closeMoudle
// Returns:   s8
// Qualifier:根据参数关闭对应的模块
// Parameter: _CHANGER_MODULE module
//************************************
static s8 closeMoudle(_CHANGER_MODULE module)
{
	u32 RxMsg = 0;
	canMsgTx(module, _CLOSE_CMD);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _CLODE_CMD_BACK)
	{
		return -1;
	}
	vTaskDelay(10);
	canMsgTx(module, _CLOSE_DATA);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _CLOSE_DATA_BACK)
	{
		return -2;
	}
	return 0;
}

//************************************
// FunctionName:  closeChanger
// Returns:   void
// Qualifier:关闭充电机，设置延时
// Parameter: void
//************************************
static s8 closeCharger(void)
{
	if (MoudleOpenFlag.module0 == TRUE)
	{
		if (0 == closeMoudle(module0))
		{
			MoudleOpenFlag.module0 = FALSE;
		}
	}
	if (MoudleOpenFlag.module2 == TRUE)
	{
		if (0 == closeMoudle(module2))
		{
			MoudleOpenFlag.module2 = FALSE;
		}
		MoudleOpenFlag.module2 = FALSE;
	}
	if ((MoudleOpenFlag.module0 | MoudleOpenFlag.module2) == FALSE)
	{
		ChargerStatus = CLOSE;
	}
    RcvCurr = 0;
	STOP_TIME;
	START_CLOSE;
	agvOpenResetCmd();
	return 0;
}
//************************************
// FunctionName:  setChanger
// Returns:   void
// Qualifier:发送命令配置当前充电板参数
// Parameter: void
//************************************
static s8 setCharger(void)
{
	//发送配置命令并等待反馈
	u32 RxMsg = 0;
	canMsgTx(0,_SET_VOLATE_CMD);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _SET_VOLATE_CMD_BACK)
	{
		return -1;
	}
	vTaskDelay(10);
	canMsgTx(0,_SET_VOLATE_DATA);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _SET_VOLATE_DATA_BACK)
	{
		return -2;
	}
	vTaskDelay(10);
	canMsgTx(0,_SET_CURR_CMD);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _SET_CURR_CMD_BACK)
	{
		return -3;
	}
	vTaskDelay(10);
	canMsgTx(0,_SET_CURR_DATA);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _SET_CURR_DATA_BACK)
	{
		return -4;
	}
	canMsgTx(2, _SET_VOLATE_CMD);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _SET_VOLATE_CMD_BACK)
	{
		return -1;
	}
	vTaskDelay(10);
	canMsgTx(2, _SET_VOLATE_DATA);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _SET_VOLATE_DATA_BACK)
	{
		return -2;
	}
	vTaskDelay(10);
	canMsgTx(2, _SET_CURR_CMD);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _SET_CURR_CMD_BACK)
	{
		return -3;
	}
	vTaskDelay(10);
	canMsgTx(2, _SET_CURR_DATA);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _SET_CURR_DATA_BACK)
	{
		return -4;
	}
	return 0;
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
	canMsgTx(0,_CHECK_BATTRY_CMD);
	xQueueReceive(CanMsgQueue, &RxMsg, 80);
	if (RxMsg != _CHECK_BATTRY_CMD_BACK)
	{
		return FALSE;
	}
	vTaskDelay(10);
	canMsgTx(0,_CHECK_BATTY_DATA);
	if (pdFALSE == xQueueReceive(CanMsgQueue, &RxMsg, 50))
	{
		return FALSE;
	}
	RcvVola = RxMsg & 0xffff;
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
	u32 Cur0 = 0;
	u32 Cur2 = 0;
	canMsgTx(module0,_CHECK_CURR_CMD);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _CHECK_CURR_CMD_BACK)
	{
		return FALSE;
	}
	vTaskDelay(10);
	canMsgTx(module0,_CHECK_CURR_DATA);
	if (pdFALSE == xQueueReceive(CanMsgQueue, &Cur0, 50))
	{
		return FALSE;
	}

	canMsgTx(module2, _CHECK_CURR_CMD);
	xQueueReceive(CanMsgQueue, &RxMsg, 50);
	if (RxMsg != _CHECK_CURR_CMD_BACK)
	{
		return FALSE;
	}
	vTaskDelay(10);
	canMsgTx(module2, _CHECK_CURR_DATA);
	if (pdFALSE == xQueueReceive(CanMsgQueue, &Cur2, 50))
	{
		return FALSE;
	}
	RcvCurr = (Cur0 & 0xffff) + (Cur2 & 0xffff);
	if (ChargerTimeCount < 60)
	{
		return TRUE;
	}
	if (RcvCurr < _CLOSE_CUR)//设定小于某值时，充电结束，充电开始时设定充电未结束
	{
		setChangerOver();
	}
	return TRUE;
}

//************************************
// FunctionName:  isOverCharge
// Returns:   bool
// Qualifier:检查是否已经充电完成
// Parameter: void
//************************************
static bool isOverCharge(void)
{
	return ChargerOverFlag;
}

//************************************
// FunctionName:  isEmmergency
// Returns:   bool
// Qualifier:检测急停按钮是否按下
// Parameter: void
//************************************
static bool isEmmergency(void)
{
	if (isButtonPress())
	{
		vTaskDelay(5);
		if (isButtonPress())
		{
			return TRUE;
		}
	}
	return FALSE;
}

//************************************
// FunctionName:  checkChangerStatusOpen
// Returns:   _CHANGER_STATUS
// Qualifier:查询当前充电机状态
// Parameter: void
//************************************
static _CHARGER_STATUS_TYPE checkChangerStatusOpen(void)
{
	return ChargerStatus;
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
	RxData = RxMessage.Data[0] << 24 | RxMessage.Data[1] << 16 | RxMessage.Data[2] << 8 | RxMessage.Data[3];
	if (RxMessage.ExtId == 0x181ff502 || RxMessage.ExtId == 0x181ff512 || RxMessage.ExtId == 0x181ff522)
	{
		xQueueSendFromISR(CanMsgQueue, &RxData, &xHigherPriorityTaskWoken);
	}
}

//************************************
// FunctionName:  setChangerOver
// Returns:   void
// Qualifier:调用该函数设定充电结束标志，此时充电电流较小
// Parameter: void
//************************************
static void setChangerOver(void)
{
	ChargerOverFlag = TRUE;
}

extern u16 getCurr(void)
{
	return RcvCurr;
}
extern u16 getVola(void)
{
    if(RcvVola > 0x78)
    {
        return (RcvVola-0x78);
    }
    else
    {
        return 0;
    }
}

//************************************
// FunctionName:  agvOpenSetCmd
// Returns:   extern void
// Qualifier:通过AGV的信息打开充电桩
// Parameter: void
//************************************
extern void agvOpenSetCmd(void)
{
	AgvSetChargerOpen = TRUE;
}
//************************************
// FunctionName:  agvOpenResetCmd
// Returns:   extern void
// Qualifier:根据AGV的信息关闭充电桩
// Parameter: void
//************************************
extern void agvOpenResetCmd(void)
{
	AgvSetChargerOpen = FALSE;
}
//************************************
// FunctionName:  isAgvOpenCmd
// Returns:   extern bool
// Qualifier:判断当前AGV是否打开充电桩
// Parameter: void
//************************************
extern bool isAgvOpenCmd(void)
{
	return AgvSetChargerOpen;
}

//************************************
// FunctionName:  agvConnectSetCmd
// Returns:   extern void
// Qualifier:AGV开始对接充电板
// Parameter: void
//************************************
extern void agvConnectSetCmd(void)
{
	AgvConnectFlag = TRUE;
}
//************************************
// FunctionName:  agvConnectResetCmd
// Returns:   extern void
// Qualifier:AGV离开对接
// Parameter: void
//************************************
extern void agvConnectResetCmd(void)
{
	AgvConnectFlag = FALSE;
}


//************************************
// FunctionName:  sendCmdToCharger
// Returns:   void
// Qualifier:根据参数向对应的充电板发送数据
// Parameter: u8 moudle
// Parameter: u8 data1
// Parameter: u8 data2
// Parameter: u8 data3
// Parameter: u8 data4
//************************************
static void sendCmdToCharger(u8 moudle, u8 data1, u8 data2, u8 data3, u8 data4)
{
	switch (moudle)
	{
	case 0:
		canMsgTx(0, data1, data2, data3, data4);
		break;
	case 2:
		canMsgTx(2, data1, data2, data3, data4);
		break;
	case 3:
		canMsgTx(0, data1, data2, data3, data4);
		vTaskDelay(10);
		canMsgTx(2, data1, data2, data3, data4);
		break;
	default:
		break;
	}
}


