#ifndef _LCD_H
#define _LCD_H

#include "stm32f10x.h"
#include "stm32f10x_spi.h"

#define _CS_PIN GPIOA, GPIO_Pin_6

#define _CS_H GPIO_SetBits(_CS_PIN);
#define _CS_L GPIO_ResetBits(_CS_PIN);
#define LCD_SPI SPI1

void lcdGPIOConfig(void);

static void byteSend(u8 data);
static void cmdSend(u8 data);
static void dataSend(u8 data);
void lcdShowNumber(u8 x_add, float number);
void lcdShowString(u8 x_add, u8 *ptr);
void lcdInit(void);
void lcdShowHex(u8 x_add, u16 number);
enum LCD_POS
{
	X1_Y1 = 0x80,
	X1_Y2,
	X1_Y3,
	X1_Y4,
	X1_Y5,
	X1_Y6,
	X1_Y7,
	X1_Y8,
	X3_Y1,
	X3_Y2,
	X3_Y3,
	X3_Y4,
	X3_Y5,
	X3_Y6,
	X3_Y7,
	X3_Y8,
	X2_Y1,
	X2_Y2,
	X2_Y3,
	X2_Y4,
	X2_Y5,
	X2_Y6,
	X2_Y7,
	X2_Y8,
	X4_Y1,
	X4_Y2,
	X4_Y3,
	X4_Y4,
	X4_Y5,
	X4_Y6,
	X4_Y7,
	X4_Y8,
};

#endif // !_LCD_H

