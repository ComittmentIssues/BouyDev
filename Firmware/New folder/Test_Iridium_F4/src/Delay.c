/*
 * Delay.c
 *
 *  Created on: Jun 16, 2019
 *      Author: Jamie
 */

#include "Delay.h"

void init_Delay(void)
{
	RCC_DelayPeriphClockCmd(RCC_Delay_Periph,ENABLE);

	TIM_TimeBaseInitTypeDef timerInitStructure; //create a 1ms delaay

	timerInitStructure.TIM_Prescaler = 3999;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = 19;
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(Delay_Timer, &timerInitStructure);
	NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = Delay_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
	nvicStructure.NVIC_IRQChannelSubPriority = 1;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);
	//enable interrupts
}

void Delay_begin_Timeout(uint32_t time)
{
	ticks = 0;
	timeout = time;
	status_flag = 1;
	timeout_flag = 0;
	assert_param(IS_TIM_ALL_PERIPH(Delay_Timer));
	Delay_Timer->CNT = 0;


}
void Delay_IRQHandler(void)
{
	//increment ticks
	ticks++;
	if((ticks == timeout)&&(status_flag == 1))
	{
		timeout_flag = 1;
		status_flag = 0;
		Delay_Disable();
		ticks = 0;
	}
	//check timeout condition
	TIM_ClearITPendingBit(Delay_Timer, TIM_IT_Update);
}

void Delay_Disable(void)
{
	//turn off timer
	TIM_ITConfig(Delay_Timer,TIM_IT_Update,DISABLE);
	TIM_Cmd(Delay_Timer, DISABLE);
	//clear counter
	assert_param(IS_TIM_ALL_PERIPH(Delay_Timer));
	Delay_Timer->CNT = 0;

}

void Delay_Enable(void)
{
	TIM_Cmd(Delay_Timer, ENABLE);
	TIM_ITConfig(Delay_Timer,TIM_IT_Update,ENABLE);
}