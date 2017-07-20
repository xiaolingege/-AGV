#ifndef _TIM_H
#define _TIM_H
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"

#define START_TIME  ChargerTimeCount=0;RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);TIM_Cmd(TIM2, ENABLE)
#define STOP_TIME  TIM_Cmd(TIM2, DISABLE);RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE)

#define START_CLOSE  ChargerCloseCount=1800;RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);TIM_Cmd(TIM3, ENABLE)
#define STOP_CLOSE  TIM_Cmd(TIM3, DISABLE);RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , DISABLE)

extern volatile u32 ChargerTimeCount;
extern volatile u32 ChargerCloseCount;

void TIM2_Configuration(void);
void TIM3_Configuration(void);

#endif // !_TIM_H
