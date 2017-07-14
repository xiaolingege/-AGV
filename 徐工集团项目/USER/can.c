#include "can.h"
#include "led.h"
#include "stdio.h"

/* 在中断处理函数中返回 */
__IO uint32_t ret = 0;

/*CAN GPIO 和时钟配置 */
void canGpioConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
}

/*	CAN初始化 */
void canInit(void)
{
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	CAN_DeInit(CAN1);	//将外设CAN的全部寄存器重设为缺省值
	CAN_StructInit(&CAN_InitStructure);//把CAN_InitStruct中的每一个参数按缺省值填入
	CAN_InitStructure.CAN_TTCM = DISABLE;//没有使能时间触发模式
	CAN_InitStructure.CAN_ABOM = DISABLE;//没有使能自动离线管理
	CAN_InitStructure.CAN_AWUM = DISABLE;//没有使能自动唤醒模式
	CAN_InitStructure.CAN_NART = DISABLE;//没有使能非自动重传模式
	CAN_InitStructure.CAN_RFLM = DISABLE;//没有使能接收FIFO锁定模式
	CAN_InitStructure.CAN_TXFP = DISABLE;//没有使能发送FIFO优先级
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;//CAN设置为正常模式
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq; //重新同步跳跃宽度1个时间单位
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq; //时间段1为3个时间单位
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq; //时间段2为2个时间单位
	CAN_InitStructure.CAN_Prescaler = 24;  //时间单位长度为24	
	CAN_Init(CAN1, &CAN_InitStructure);
	//波特率为：72M/2/24(1+3+2)=0.25 即250K
/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 1;//指定过滤器为1
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;//指定过滤器为标识符屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;//过滤器位宽为32位
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;// 过滤器标识符的高16位值
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;//	 过滤器标识符的低16位值
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;//过滤器屏蔽标识符的高16位值
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;//	过滤器屏蔽标识符的低16位值
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;// 设定了指向过滤器的FIFO为0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;// 使能过滤器
	CAN_FilterInit(&CAN_FilterInitStructure);//	按上面的参数初始化过滤器
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE); //使能FIFO0消息挂号中断
}
/* 发送两个字节的数据*/
void canMsgTx(u8 Data1, u8 Data2, u8 Data3, u8 Data4)
{
	CanTxMsg TxMessage;
	TxMessage.ExtId = 0x181f02f5  ;	//标准标识符为0x00
	TxMessage.IDE = CAN_ID_EXT;//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;//为数据帧
	TxMessage.DLC = 4;	//	消息的数据长度为2个字节
	TxMessage.Data[0] = Data1; //第一个字节数据
	TxMessage.Data[1] = Data2; //第二个字节数据 
	TxMessage.Data[2] = Data3; //第三个字节数据
	TxMessage.Data[3] = Data4; //第四个字节数据
	CAN_Transmit(CAN1, &TxMessage); //发送数据
}

