#ifndef _TTL_LCD_H
#define _TTL_LCD_H
#include "usart.h"
#include "stm32f10x.h"

#define _TTL_LCD_CLR  USART_OUT(USART2, (u8 *)"SPG(2)\r\n", 8)
#define _TTL_LCD_SHOW USART_OUT(USART2, (u8 *)"SPG(3)\r\n", 8)

typedef enum _TTLCOMMOND
{
	CHARGEVOL = 0,
	CHARGECUR = 1,
	CHARGETIME = 2,
	MACHINESTATUS = 3,
	BATTERY = 4,
	COOL = 5
}TTLCOMMOND;
void ttlLcdMsgSed(TTLCOMMOND cmd, float data);
#endif // !_TTL_LCD_H
