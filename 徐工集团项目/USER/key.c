#include "key.h"
void Delay(vu32 nCount)
{
	for (; nCount != 0; nCount--);
}
/*按键管脚初始化*/
void keyInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能外设时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; //最高输出速率10MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; //最高输出速率10MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/*检测是否有按键按下*/
void  GetKey(void)
{
	if (Bit_RESET == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13))
	{
		Delay(1000000);//去抖动//去抖动
		if (Bit_RESET == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13))
		{
			while (Bit_RESET == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)) { ; }//等待按键释放                        
			canMsgTx(0X55, 0X77, 0x01, 0x02);
			LED1(1); LED2(1);
		}
	}

	if (Bit_RESET == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12))
	{
		Delay(1000000);//去抖动//去抖动
		if (Bit_RESET == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12))
		{
			while (Bit_RESET == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) { ; }//等待按键释放                        
			canMsgTx(0X99, 0Xbb, 0x01, 0x02);
			LED1(1); LED2(1);
		}
	}
}

