#ifndef __CAN_H
#define	__CAN_H


#include "stm32f10x.h" 
 void canInit(void);
 void can_tx(u8 Data1,u8 Data2);
 void can_rx(void);
 void CAN_NVIC_Configuration(void);
void canGpioConfig(void);

#endif /* __CAN_H */


