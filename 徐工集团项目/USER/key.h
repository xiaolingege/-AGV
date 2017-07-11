#ifndef _KEY_H
#define _KEY_H
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "led.h"
#include "can.h"
void keyInit(void);

void GetKey(void);
#endif // !_KEY_H
