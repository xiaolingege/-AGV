#ifndef _TIM_H
#define _TIM_H
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"

#define START_TIME  ChargerTimeCount=0;RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);TIM_Cmd(TIM2, ENABLE)
#define STOP_TIME  TIM_Cmd(TIM2, DISABLE);RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE)

extern volatile u32 ChargerTimeCount;

void TIM2_Configuration(void);

#endif // !_TIM_H
