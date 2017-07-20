#ifndef __CAN_H
#define	__CAN_H

#include "stm32f10x.h" 

void canInit(void);
void canMsgTx(u8 module, u8 Data1, u8 Data2, u8 Data3, u8 Data4);
void canGpioConfig(void);

#endif /* __CAN_H */


