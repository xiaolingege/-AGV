#ifndef _SPI_H
#define _SPI_H
#include "stm32f10x_spi.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
void spiInit(void);
void spiGpioConfig(void);
void spiConfig(void);
#endif // !_SPI_H
