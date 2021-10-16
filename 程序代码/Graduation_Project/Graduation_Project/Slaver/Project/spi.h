#ifndef _SPI_H_
#define _SPI_H_

#include "stm32f10x.h"

#define SPI1_CS_GPIO	GPIO_Pin_1	// B1
#define SPI1_SCK_GPIO	GPIO_Pin_5	// A5
#define SPI1_MISO_GPIO	GPIO_Pin_6	// A6
#define SPI1_MOSI_GPIO	GPIO_Pin_7	// A7

#define NRF_CE_GPIO		GPIO_Pin_0	//B0
#define NRF_IRQ_GPIO	GPIO_Pin_10	//B10

void SPI_configuration(void);
void SPI_GPIO_configuration(void);
void NRF_IRQ_EXTI(void);


#endif
