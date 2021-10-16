#include "USART.h"
//串口配置
void USART_config(void)
{
	USART_InitTypeDef USART_Initstruct;
	
	USART_NVIC_configuration();
	USART_GPIO_config();
		
	USART_Initstruct.USART_BaudRate = USART1_BaudRate;
	USART_Initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Initstruct.USART_Parity = USART_Parity_No;
	USART_Initstruct.USART_StopBits = USART_StopBits_1;
	USART_Initstruct.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART1, &USART_Initstruct);
	USART_Initstruct.USART_BaudRate = USART2_BaudRate;
	USART_Init(USART2, &USART_Initstruct);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
}
//串口相关GPIO配置
void USART_GPIO_config(void)
{
	GPIO_InitTypeDef USART1_GPIO[2];
	GPIO_InitTypeDef USART2_GPIO[2];
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	USART1_GPIO[0].GPIO_Mode = GPIO_Mode_AF_PP;
	USART1_GPIO[0].GPIO_Pin = USART1_TX_GPIO;
	USART1_GPIO[0].GPIO_Speed = GPIO_Speed_50MHz;
	
	USART1_GPIO[1].GPIO_Mode = GPIO_Mode_IN_FLOATING;
	USART1_GPIO[1].GPIO_Pin = USART1_RX_GPIO;
	USART1_GPIO[1].GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&USART1_GPIO[0]);
	GPIO_Init(GPIOA,&USART1_GPIO[1]);
	
	
	USART2_GPIO[0].GPIO_Mode = GPIO_Mode_AF_PP;
	USART2_GPIO[0].GPIO_Pin = USART2_TX_GPIO;
	USART2_GPIO[0].GPIO_Speed = GPIO_Speed_50MHz;
	
	USART2_GPIO[1].GPIO_Mode = GPIO_Mode_IN_FLOATING;
	USART2_GPIO[1].GPIO_Pin = USART2_RX_GPIO;
	USART2_GPIO[1].GPIO_Speed = GPIO_Speed_50MHz;


	GPIO_Init(GPIOA,&USART2_GPIO[0]);
	GPIO_Init(GPIOA,&USART2_GPIO[1]);
	
}
//串口中断配置
void USART_NVIC_configuration(void)
{
	NVIC_InitTypeDef USART_NVIC_Initstruct[2];
		
	USART_NVIC_Initstruct[0].NVIC_IRQChannel = USART1_IRQn;
	USART_NVIC_Initstruct[0].NVIC_IRQChannelCmd = ENABLE;
	USART_NVIC_Initstruct[0].NVIC_IRQChannelPreemptionPriority = 0;
	USART_NVIC_Initstruct[0].NVIC_IRQChannelSubPriority = 3;
	
	NVIC_Init(&USART_NVIC_Initstruct[0]);
	
	USART_NVIC_Initstruct[1].NVIC_IRQChannel = USART2_IRQn;
	USART_NVIC_Initstruct[1].NVIC_IRQChannelCmd = ENABLE;
	USART_NVIC_Initstruct[1].NVIC_IRQChannelPreemptionPriority = 0;
	USART_NVIC_Initstruct[1].NVIC_IRQChannelSubPriority = 4;
	
	NVIC_Init(&USART_NVIC_Initstruct[1]);
}

int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口 */
	USART_SendData(USART2, (uint8_t) ch);

	/* 等待发送完毕 */
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){};

	 return (ch);
 }

 ///重定向 c 库函数 scanf 到串口，重写向后可使用 scanf、 getchar 等函数
 int fgetc(FILE *f)
 {
	 /* 等待串口输入数据 */
	 while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);

	 return (int)USART_ReceiveData(USART2);
 }
