#ifndef _USART_H
#define _USART_H
#include "stm32f10x.h"
#include "stm32f10x_usart.h"

#define _USART1_ENABLE 0
#define _USART2_ENABLE 1
#define _USART3_ENABLE 1
#define _USART4_ENABLE 0
#define _USART5_ENABLE 0

#define _USART1_RX_PORT GPIOB
#define _USART1_TX_PORT GPIOA

#define _USART1_RX_PIN GPIO_Pin_10
#define _USART1_TX_PIN GPIO_Pin_9

#define _USART2_RX_PORT GPIOA
#define _USART2_TX_PORT GPIOA

#define _USART2_RX_PIN GPIO_Pin_3
#define _USART2_TX_PIN GPIO_Pin_2


#define _USART3_RX_PORT GPIOB
#define _USART3_TX_PORT GPIOB

#define _USART3_RX_PIN GPIO_Pin_11
#define _USART3_TX_PIN GPIO_Pin_10

#define _USART4_RX_PORT GPIOA
#define _USART4_TX_PORT GPIOA

#define _USART5_RX_PORT GPIOA
#define _USART5_TX_PORT GPIOA

#define _USART1_BAUD 115200
#define _USART2_BAUD 115200
#define _USART3_BAUD 2400
#define _USART4_BAUD 115200
#define _USART5_BAUD 115200

/*RE跟GPIOB1管脚相连*/
#define Set_RE  GPIO_SetBits(GPIOB,GPIO_Pin_1);			 
#define Clr_RE  GPIO_ResetBits(GPIOB,GPIO_Pin_1);
/*DE跟GPIOB0管脚相连*/
#define Set_DE  GPIO_SetBits(GPIOB,GPIO_Pin_0);
#define Clr_DE  GPIO_ResetBits(GPIOB,GPIO_Pin_0);

void usartConfig(void);
static void USART_Config(USART_TypeDef* USARTx, u32 rate);
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data, uint16_t Len);
void usart485Send(u8 *Data, u16 Len);
//配置上面的参数可以更改串口设置，另外需要配置的参数包括：对应引脚的时钟，复用时钟，是否重映射，中断配置
#endif // !_USART_H
