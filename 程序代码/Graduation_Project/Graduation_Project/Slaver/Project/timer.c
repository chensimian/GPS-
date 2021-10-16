#include "timer.h"

void timer1_configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_Initstruct[2];
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM2,ENABLE);
	TIM2_NCIV_configuration();
	
	TIM_Initstruct[0].TIM_ClockDivision = 0;
	TIM_Initstruct[0].TIM_CounterMode = TIM_CounterMode_Up;
	TIM_Initstruct[0].TIM_Period = 20000;		// 10000是20 ms
	TIM_Initstruct[0].TIM_Prescaler = 71;		// 1us
	
	TIM_Initstruct[1].TIM_ClockDivision = 0;
	TIM_Initstruct[1].TIM_CounterMode = TIM_CounterMode_Up;
	TIM_Initstruct[1].TIM_Period = 64800;		// 60000是15 ms
	TIM_Initstruct[1].TIM_Prescaler = 0;		// 250ns 计数一次
	
	TIM_TimeBaseInit(TIM2,&TIM_Initstruct[0]);
	TIM_TimeBaseInit(TIM3,&TIM_Initstruct[1]);
	
	TIM_ClearFlag(TIM2,TIM_IT_Update);
	TIM_ClearFlag(TIM3,TIM_IT_Update);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
}

void TIM2_NCIV_configuration(void)
{
	NVIC_InitTypeDef TIM2_NVIC_Initstruct;
	NVIC_InitTypeDef TIM3_NVIC_Initstruct;
	
	TIM2_NVIC_Initstruct.NVIC_IRQChannel = TIM2_IRQn;
	TIM2_NVIC_Initstruct.NVIC_IRQChannelCmd = ENABLE;
	TIM2_NVIC_Initstruct.NVIC_IRQChannelPreemptionPriority = 0;
	TIM2_NVIC_Initstruct.NVIC_IRQChannelSubPriority = 2;
	
	TIM3_NVIC_Initstruct.NVIC_IRQChannel = TIM3_IRQn;
	TIM3_NVIC_Initstruct.NVIC_IRQChannelCmd = ENABLE;
	TIM3_NVIC_Initstruct.NVIC_IRQChannelPreemptionPriority = 0;
	TIM3_NVIC_Initstruct.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&TIM2_NVIC_Initstruct);
	NVIC_Init(&TIM3_NVIC_Initstruct);
	
}

