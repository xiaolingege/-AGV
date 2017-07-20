#include "lcd.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include <stdio.h>
#include "SPI.h"

static void byteSend(u8 data)
{
	while (SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) == RESET);

	/* Send byte through the SPI3 peripheral */
	SPI_I2S_SendData(LCD_SPI, data);
}
static void cmdSend(u8  cmd)
{
	_CS_H;
	byteSend(0XF8);
	byteSend(cmd & 0xF0);
	byteSend((cmd & 0x0F) << 4);
	vTaskDelay(1);
	_CS_L;
}
static void dataSend(u8 data)
{
	_CS_H;
	byteSend(0xFA);
	byteSend(data & 0xF0);
	byteSend((data & 0x0F) << 4);
	vTaskDelay(1);
	_CS_L;
}
void lcdShowHex(u8 x_add, u16 number)
{
		char arr[5] = { '\0' };
	char *ptr = arr;
	sprintf(arr, " %x", number);
	cmdSend(x_add);
	if (number < 10)
	{
		dataSend(' ');
	}
	while (*ptr != '\0')
	{
		dataSend(*ptr);
		++ptr;
	}
}
void lcdShowNumber(u8 x_add, float number)
{
	char arr[5] = { '\0' };
	char *ptr = arr;
	if (number < 0)
	{
		number = 0- number;
	}
	else
	{
//		while (number > 100)
		{
//			number -= 100;
		}
	}
	sprintf(arr, " %.1f", number);
	cmdSend(x_add);
    if (number < 100)
	{
        dataSend(' ');
		dataSend(' ');
	}

    if(number < 1000)
    {
        dataSend(' ');
    }
    else  if (number < 100)
	{
        dataSend(' ');
		dataSend(' ');
	} else if (number < 100)
	{
        dataSend(' ');
		dataSend(' ');
	}
	while (*ptr != '\0')
	{
		dataSend(*ptr);
		++ptr;
	}
}
void lcdShowString(u8 x_add, u8 *ptr)
{
	cmdSend(x_add);
	while (*ptr != '\0')
	{
		dataSend(*ptr);
		++ptr;
	}
}

void lcdInit(void)
{
	_CS_L;
	cmdSend(0x30);//8位数据格式
	cmdSend(0x0c);//显示开，游标开
	cmdSend(0x01);//清屏
	cmdSend(0x02);//
	cmdSend(0x80);
}




