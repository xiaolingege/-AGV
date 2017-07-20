#include "ttllcd.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void ttlLcdMsgSed(TTLCOMMOND cmd, float data)
{
	char ptr[26] = { '\0' };
	char ptr_val[10] = { '\0' };
	if (data < 10)
	{
		sprintf(ptr_val, "%s%.1f", " " , data);
	}
	else
	{
		sprintf(ptr_val, "%.1f", data);
	}
	switch (cmd)
	{
	case CHARGEVOL:
		sprintf(ptr, "%s%s%s", "DS32(174,168,'", ptr_val, " V");
		break;
	case CHARGECUR:
		sprintf(ptr, "%s%s%s", "DS32(638,168,'", ptr_val, " A");

		break;
	case CHARGETIME:
		sprintf(ptr, "%s%s%s", "DS32(174,279,'", ptr_val, " s");

		break;
	case MACHINESTATUS:
		strcpy(ptr, "DS32(638,279,'");
		strcat(ptr, ptr_val);
		break;
	case BATTERY:
		if (data == TRUE)
		{
			strcpy(ptr, "DS32(174,408,' YES ");
		}
		else
		{
			strcpy(ptr, "DS32(174,408,'  NO ");
		}
		break;
	case COOL:
		if (data != 0)
		{
			strcpy(ptr, "DS32(638,408,' YES ");
		}
		else
		{
			strcpy(ptr, "DS32(1638,408,'  NO ");
		}
		break;
	default:
		break;
	}
	strcat(ptr, "',1);\r\n");
	USART_OUT(USART2, (u8 *)ptr, 26);
	vTaskDelay(100);

}



