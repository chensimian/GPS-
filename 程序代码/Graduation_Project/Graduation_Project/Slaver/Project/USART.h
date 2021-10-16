#ifndef _USART_H_
#define _USART_H_

#include "stm32f10x.h"
#include <stdio.h>

#define USART1_BaudRate		9600
#define USART2_BaudRate		9600

#define USART1_TX_GPIO		GPIO_Pin_9	// A9
#define USART1_RX_GPIO		GPIO_Pin_10	// A10

#define USART2_TX_GPIO		GPIO_Pin_2	// A2
#define USART2_RX_GPIO		GPIO_Pin_3	// A3

void USART_config(void);
void USART_GPIO_config(void);
void USART_NVIC_configuration(void);



#endif
