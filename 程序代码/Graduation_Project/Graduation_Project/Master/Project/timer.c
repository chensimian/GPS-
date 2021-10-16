#include "timer.h"

//配置时钟TIM2
void timer1_configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM2_Initstruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM2_NCIV_configuration();
	
	TIM2_Initstruct.TIM_ClockDivision = 0;
	TIM2_Initstruct.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM2_Initstruct.TIM_Period = 60000;		// 60000是15 ms
//	TIM2_Initstruct.TIM_Prescaler = 17;		// 250ns
//	TIM2_Initstruct.TIM_Prescaler = 8;		// 125ns
//	TIM2_Initstruct.TIM_Period = 64000;		// 64000是8 ms
	TIM2_Initstruct.TIM_Prescaler = 0;		// 13.8889ns
	TIM2_Initstruct.TIM_Period = 64800;		//64800*13.889=0.9ms
	
	TIM_TimeBaseInit(TIM2,&TIM2_Initstruct);
	TIM_ClearFlag(TIM2,TIM_IT_Update);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
}

//配置时钟的中断
void TIM2_NCIV_configuration(void)
{
	NVIC_InitTypeDef TIM2_NVIC_Initstruct;
	
	TIM2_NVIC_Initstruct.NVIC_IRQChannel = TIM2_IRQn;
	TIM2_NVIC_Initstruct.NVIC_IRQChannelCmd = ENABLE;
	TIM2_NVIC_Initstruct.NVIC_IRQChannelPreemptionPriority = 0;
	TIM2_NVIC_Initstruct.NVIC_IRQChannelSubPriority = 0;
	
	NVIC_Init(&TIM2_NVIC_Initstruct);
	
}

