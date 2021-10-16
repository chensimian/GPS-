#include "spi.h"

//SPI配置
void SPI_configuration(void)
{
	SPI_InitTypeDef SPI1_Initstruct;
	
	SPI_GPIO_configuration();
	NRF_IRQ_EXTI();
	
	SPI1_Initstruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI1_Initstruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI1_Initstruct.SPI_CPOL = SPI_CPOL_Low;
	SPI1_Initstruct.SPI_CRCPolynomial = 7;
	SPI1_Initstruct.SPI_DataSize = SPI_DataSize_8b;
	SPI1_Initstruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI1_Initstruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI1_Initstruct.SPI_Mode = SPI_Mode_Master;
	SPI1_Initstruct.SPI_NSS = SPI_NSS_Soft;		// 软件 NSS
	
	SPI_Init(SPI1, &SPI1_Initstruct);
	
	SPI_Cmd(SPI1,ENABLE);
}
//SPI相关GPIO的配置
void SPI_GPIO_configuration(void)
{
	GPIO_InitTypeDef SPI1_GPIO[6];
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOB,ENABLE);	// 在 USART 那里配置了 GPIO AIFO 的时钟
	
	// CS
	SPI1_GPIO[0].GPIO_Mode = GPIO_Mode_Out_PP;
	SPI1_GPIO[0].GPIO_Pin = SPI1_CS_GPIO;
	SPI1_GPIO[0].GPIO_Speed = GPIO_Speed_50MHz;

	
	// SCK
	SPI1_GPIO[1].GPIO_Mode = GPIO_Mode_AF_PP;
	SPI1_GPIO[1].GPIO_Pin = SPI1_SCK_GPIO;
	SPI1_GPIO[1].GPIO_Speed = GPIO_Speed_50MHz;
	
	// MISO
	SPI1_GPIO[2].GPIO_Mode = GPIO_Mode_IN_FLOATING;
	SPI1_GPIO[2].GPIO_Pin = SPI1_MISO_GPIO;
	SPI1_GPIO[2].GPIO_Speed = GPIO_Speed_50MHz;
	
	// MOSI
	SPI1_GPIO[3].GPIO_Mode = GPIO_Mode_AF_PP;
	SPI1_GPIO[3].GPIO_Pin = SPI1_MOSI_GPIO;
	SPI1_GPIO[3].GPIO_Speed = GPIO_Speed_50MHz;

	// CE
	SPI1_GPIO[4].GPIO_Mode = GPIO_Mode_Out_PP;
	SPI1_GPIO[4].GPIO_Pin = NRF_CE_GPIO;
	SPI1_GPIO[4].GPIO_Speed = GPIO_Speed_50MHz;
	
	// IRQ
	SPI1_GPIO[5].GPIO_Mode = GPIO_Mode_IPU;
	SPI1_GPIO[5].GPIO_Pin = NRF_IRQ_GPIO;
//	SPI1_GPIO[5].GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB, &SPI1_GPIO[0]);
	GPIO_Init(GPIOA, &SPI1_GPIO[1]);
	GPIO_Init(GPIOA, &SPI1_GPIO[2]);
	GPIO_Init(GPIOA, &SPI1_GPIO[3]);
	GPIO_Init(GPIOB, &SPI1_GPIO[4]);
	GPIO_Init(GPIOB, &SPI1_GPIO[5]);
	
	GPIO_SetBits(GPIOB,SPI1_CS_GPIO);
	GPIO_ResetBits(GPIOB,NRF_CE_GPIO);
}
//GPIO中断的配置（EXTI10）
void NRF_IRQ_EXTI(void)
{
	NVIC_InitTypeDef NRF_IRQ_NVIC;
	EXTI_InitTypeDef NRF_IRQ_EXTI;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB,ENABLE);
	
	NRF_IRQ_NVIC.NVIC_IRQChannel = EXTI15_10_IRQn;
	NRF_IRQ_NVIC.NVIC_IRQChannelCmd = ENABLE;
	NRF_IRQ_NVIC.NVIC_IRQChannelPreemptionPriority = 0;
	NRF_IRQ_NVIC.NVIC_IRQChannelSubPriority = 3;
	
	NRF_IRQ_EXTI.EXTI_Line = EXTI_Line10;
	NRF_IRQ_EXTI.EXTI_LineCmd = ENABLE;
	NRF_IRQ_EXTI.EXTI_Mode = EXTI_Mode_Interrupt;
	NRF_IRQ_EXTI.EXTI_Trigger = EXTI_Trigger_Falling;
	
	NVIC_Init(&NRF_IRQ_NVIC);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource10);
	EXTI_Init(&NRF_IRQ_EXTI);
	
}
