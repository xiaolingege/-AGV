#include "usart.h"

void usartConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO |  RCC_APB2Periph_GPIOA|  RCC_APB2Periph_GPIOB, ENABLE);
#if _USART1_ENABLE
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_Config(USART1, _USART1_BAUD);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = _USART1_TX_PIN;	            //USART1  TX
	GPIO_Init(_USART1_TX_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = _USART1_RX_PIN;	            // USART1 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 复用开漏输入
	GPIO_Init(_USART1_RX_PORT, &GPIO_InitStructure);
#endif

#if _USART2_ENABLE
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_Config(USART2, _USART2_BAUD);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = _USART2_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_USART2_TX_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = _USART2_RX_PIN;
	GPIO_Init(_USART2_RX_PORT, &GPIO_InitStructure);
#endif
#if _USART3_ENABLE
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	USART_Config(USART3, _USART3_BAUD);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = _USART3_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_USART3_TX_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = _USART3_RX_PIN;
	GPIO_Init(_USART3_RX_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
}



static void USART_Config(USART_TypeDef* USARTx, u32 rate)
{
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = rate;					//速率115200bps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//停止位1位
	USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //无硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
	USART_Init(USARTx, &USART_InitStructure);							//配置串口参数函数
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);                      //使能接收中断
	USART_Cmd(USARTx, ENABLE);
}


void USART2_IRQHandler(void)
{//  unsigned int i;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
		// 	if(!(USART_GetITStatus(USART2,USART_IT_RXNE))); 
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}
void USART3_IRQHandler(void)
{//  unsigned int i;
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
		// 	if(!(USART_GetITStatus(USART2,USART_IT_RXNE))); 
	{
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}

void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data, uint16_t Len)
{
	uint16_t i;
	for (i = 0; i < Len; i++)
	{
		USART_SendData(USARTx, Data[i]);
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);      //判断是否发送完成。
	}
}

void usart485Send(u8 *Data, u16 Len)
{
	Set_DE;
	Set_RE;
	USART_OUT(USART3, Data, Len);
	Clr_DE;
	Clr_RE;
}

