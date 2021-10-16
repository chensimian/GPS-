#include "USART.h"

//串口的配置
void USART_config(void)
{
	USART_InitTypeDef USART_Initstruct;
	
	USART_GPIO_config();
		
	USART_Initstruct.USART_BaudRate = USART2_BaudRate;
	USART_Initstruct.USART_HardwareFlowControl = DISABLE;
	USART_Initstruct.USART_Mode = USART_Mode_Tx;
	USART_Initstruct.USART_Parity = USART_Parity_No;
	USART_Initstruct.USART_StopBits = USART_StopBits_1;
	USART_Initstruct.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART2, &USART_Initstruct);
	
	
}
//串口相关GPIO的配置
void USART_GPIO_config(void)
{
	GPIO_InitTypeDef USART2_GPIO[2];
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);
	
	USART2_GPIO[0].GPIO_Mode = GPIO_Mode_AF_PP;
	USART2_GPIO[0].GPIO_Pin = USART2_TX_GPIO;
	USART2_GPIO[0].GPIO_Speed = GPIO_Speed_50MHz;
	
	USART2_GPIO[1].GPIO_Mode = GPIO_Mode_IN_FLOATING;
	USART2_GPIO[1].GPIO_Pin = USART2_RX_GPIO;
	USART2_GPIO[1].GPIO_Speed = GPIO_Speed_50MHz;


	GPIO_Init(GPIOA,&USART2_GPIO[0]);
	GPIO_Init(GPIOA,&USART2_GPIO[1]);
	
}

int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口 */
	USART_SendData(USART2, (uint8_t) ch);

	/* 等待发送完毕 */
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){};

	 return (ch);
}

