/**
  ******************************************************************************
  * @file    EEPROM_Emulation/src/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    10-October-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "eeprom.h"
#include "stdlib.h"
#include "string.h"
#include "RTC.h"
#include "stm32f4_discovery.h"
#include "time.h"

#define length(x) sizeof(x)/sizeof(*x)


/* Virtual address defined by the user: 0xFFFF value is prohibited */
uint16_t VirtAddVarTab[NB_OF_VAR] = {0x5555, 0x6666, 0x7777};
uint16_t VarDataTab[NB_OF_VAR] = {0, 0, 0};
uint16_t VarValue = 0;
uint8_t dummy_packet[] = {0x01,0x5d,0x1a,0x13,0x49,0x4f,0xd7,0x51,0xc5,0x76,0x75,0xe4,0x44,0x03,0x00,0x02,0xf0,0x01,0x1f,0x1f,0x18,0x66,0x03,0x1d,00,00};

RTC_TimeTypeDef Atime;

RTC_AlarmTypeDef alarmA;
int main(void)
{
		if(((RTC_GetFlagStatus(RTC_FLAG_ALRAF) == SET)&&(PWR_GetFlagStatus(PWR_FLAG_WU)) == SET))
		{
			PWR_ClearFlag(PWR_FLAG_WU);
			RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, DISABLE);
			/* Writes 1 in the DBP bit of PWR_CR register */
			PWR_BackupAccessCmd(ENABLE);
			/* RTC registers are made write accessible */
			RTC_WriteProtectionCmd(DISABLE);
			/* Clear the RTC Alarm-A Flag */
			/* Makes the RTC registers read only */
			RTC_WriteProtectionCmd(ENABLE);
			/* Locks the DBP bit of PWR_CR register again */
			RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, ENABLE);
			RTC_ClearITPendingBit(RTC_IT_ALRA);
			EXTI_ClearITPendingBit(EXTI_Line17);

		}else
		{
			init_RTC();
		}
		RTC_GetTime(RTC_Format_BIN,&Atime);
		alarmA.RTC_AlarmMask = (RTC_AlarmMask_Hours | RTC_AlarmMask_DateWeekDay | RTC_AlarmMask_Seconds);
		if(Atime.RTC_Minutes == 59)
		{
			Atime.RTC_Minutes = 0; //add a minute
			Atime.RTC_Hours += 1;
		}else
		{
			Atime.RTC_Minutes += 1;
		}
		set_RTCAlarm_A(&Atime,&alarmA);


		//add one minute

	while(1)
	{
		set_StdBy_Mode();
	}
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
