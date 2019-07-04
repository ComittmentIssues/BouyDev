/*
 * RTC.c
 *
 *  Created on: Jul 2, 2019
 *      Author: Jamie
 */

#ifndef RTC_C_
#define RTC_C_

#include "RTC.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4_discovery.h"

void init_Delay(void);

void init_RTC()
{
	/*Disable register write protect */
//	RTC_WriteProtectionCmd(DISABLE);
	/* Enter Initialisation mode */
//	RTC_EnterInitMode();
//	while(RTC_GetFlagStatus(RTC_FLAG_INITF) != SET);
	init_RTC_LSI();
	/* Program prescalers (NOTE: Dependent on input clock source)*/
	RTC_InitTypeDef RTC_InitStruct;
	RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
	RTC_InitStruct.RTC_AsynchPrediv = 127; //values from AN3371
	RTC_InitStruct.RTC_SynchPrediv = 249;
	RTC_Init(&RTC_InitStruct);

	/* init Time and Date to default values*/
	RTC_DateTypeDef RTC_DateStruct;
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateStructInit(&RTC_DateStruct);
	RTC_TimeStructInit(&RTC_TimeStruct);
	RTC_SetTime(RTC_Format_BIN,&RTC_TimeStruct);
	RTC_SetDate(RTC_Format_BIN,&RTC_DateStruct);

}
/*
 * Configure RTC to use LSE clock
 */
void init_RTC_LSI(void)
{
	/* Allow access to the RTC */
	PWR_BackupAccessCmd(ENABLE);
	RCC_LSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	RCC_RTCCLKCmd(ENABLE);
	RTC_WaitForSynchro();
}
void set_RTCAlarm_A(void)
{
	/* Configure Alarm*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RTC_AlarmTypeDef RTC_AlarmInitStruct;
	RTC_AlarmInitStruct.RTC_AlarmTime.RTC_Hours = 0;
	RTC_AlarmInitStruct.RTC_AlarmTime.RTC_Minutes = 0;
	RTC_AlarmInitStruct.RTC_AlarmTime.RTC_Seconds = 1;
	RTC_AlarmInitStruct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmInitStruct.RTC_AlarmDateWeekDay = 1;
	RTC_AlarmInitStruct.RTC_AlarmMask = RTC_AlarmMask_Minutes | RTC_AlarmMask_Hours | RTC_AlarmMask_DateWeekDay| RTC_AlarmMask_Seconds;
	/*set alarm interrupt*/
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmInitStruct);
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);
	/* Map alarm to EXTI 17*/
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line17;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	/* Enable interrupt in NVIC */
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	NVIC_EnableIRQ(RTC_Alarm_IRQn);

	//clear any pending interrupt flags
	RTC_ClearITPendingBit(RTC_IT_ALRA);
	RTC_ClearFlag(RTC_FLAG_ALRAF);
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);

	RTC_WriteProtectionCmd(ENABLE);



}


void init_AutoWakeUp(void)
{
	/* Disable Write Protection*/
	RTC_WriteProtectionCmd(DISABLE);
	/* Disable Wake Up timer */
	RTC_WakeUpCmd(DISABLE);
	while(RTC_GetFlagStatus(RTC_FLAG_WUTWF) == RESET);
	/* Set Wake up timer period*/
	RTC_SetWakeUpCounter(1); //1 second
	/* set configuration 2, 1 s wake up period*/
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	/* Enable Wake-up Timer interrupt */
	RTC_ITConfig(RTC_IT_WUT,ENABLE);

	/* Configure NVIC*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_EnableIRQ(RTC_WKUP_IRQn);

	/* Enable Wake up Timer*/
	RTC_ClearITPendingBit(RTC_IT_WUT);
	RTC_ClearFlag( RTC_FLAG_WUTF );
	PWR->CR |= PWR_CR_CWUF;

	RTC_WakeUpCmd(ENABLE);
	RTC_WriteProtectionCmd(ENABLE);


}
void set_StdBy_Mode(void)
{
	/* Enable Power command*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	/* Set sleep deep bit*/
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	/* select stdby mode*/
	PWR->CR |= PWR_CR_PDDS;
	/* Clear Wake Up flag*/
	PWR->CR |= PWR_CR_CWUF;
	/* enable wake up pin*/
	PWR->CSR |= PWR_CSR_EWUP;
	//clear pending interrupts
	/* request wait for interrupt*/
	__WFI();

}
void TIM2_IRQHandler(void)
{
	__NOP();
}
void RTC_Alarm_IRQHandler()
{
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF) == SET)
	{
		STM_EVAL_LEDToggle(LED4);
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		EXTI_ClearITPendingBit(EXTI_Line17);
	}
}
void RTC_WKUP_IRQHandler()
{
	__NOP();
}
#endif /* RTC_C_ */
