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
#include "stm32f4xx.h"


#define length(x) sizeof(x)/sizeof(*x)


/* Virtual address defined by the user: 0xFFFF value is prohibited */
uint16_t VirtAddVarTab[NB_OF_VAR] = {0x5555, 0x6666, 0x7777};
uint16_t VarDataTab[NB_OF_VAR] = {0, 0, 0};
uint16_t VarValue = 0;
uint8_t dummy_packet[] = {0x01,0x5d,0x1a,0x13,0x49,0x4f,0xd7,0x51,0xc5,0x76,0x75,0xe4,0x44,0x03,0x00,0x02,0xf0,0x01,0x1f,0x1f,0x18,0x66,0x03,0x1d,00,00};

RTC_TimeTypeDef Atime;
RTC_TimeTypeDef ttime;
RTC_AlarmTypeDef alarmA;

void init_RCC_Clock(void);
int main(void)
{
		/* Check if software reset has occurred */
		STM_EVAL_LEDInit(LED3);
		RCC_ClocksTypeDef Clockf;
		RCC_GetClocksFreq(&Clockf);
		init_RCC_Clock();
		RCC_GetClocksFreq(&Clockf);
		if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)
		{
			/* check if source came from alarm flag*/
			if(RTC_GetFlagStatus(RTC_FLAG_ALRAF) == SET)
			{
				/* Disable Power*/
				RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR,DISABLE);
				/* Enable access to back up registers*/
				PWR_BackupAccessCmd(ENABLE);
				// read block at virtual address
				FLASH_Unlock();
				EE_Init();

				if(EE_ReadVariable(VirtAddVarTab[0],&VarValue))
				{
					EE_WriteVariable(VirtAddVarTab[0],0x1);
				}
				else
				{
					VarValue++;
					EE_WriteVariable(VirtAddVarTab[0],VarValue);
				}
				FLASH_Lock();
			}
			PWR_ClearFlag(PWR_FLAG_WU);

		}else
		{
			init_RTC();
		}
		RTC_GetTime(RTC_Format_BIN,&Atime);
		alarmA.RTC_AlarmMask = (RTC_AlarmMask_Hours | RTC_AlarmMask_DateWeekDay | RTC_AlarmMask_Minutes);
		Atime.RTC_Seconds+= 15;
		if(Atime.RTC_Seconds > 59)
		{
			Atime.RTC_Seconds -= 60; //add a minute
			Atime.RTC_Minutes += 1;
		}
		if(Atime.RTC_Minutes > 59)
		{
			Atime.RTC_Minutes = 0;
			Atime.RTC_Hours++;
		}

		set_RTCAlarm_A(&Atime,&alarmA);
		STM_EVAL_LEDOn(LED3);

		//add one minute

	while(1)
	{

			set_StdBy_Mode();

	}
}

void init_RCC_Clock(void)
{
	/* Set RCC value to default state*/
	RCC_DeInit();
	/* Enable external Crystal Oscilator*/
	RCC_HSEConfig(RCC_HSE_ON);
	ErrorStatus errorstatus = RCC_WaitForHSEStartUp();
	if(errorstatus == SUCCESS)
	{
		/* Configure PLL clock for 48 MHz*/
		RCC_PLLConfig(RCC_PLLSource_HSE,4,192,8,8);
		/* enable pll and wait until ready*/
		RCC_PLLCmd(ENABLE);
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

		/* Set Flash Latency*/
		FLASH_SetLatency(FLASH_Latency_1);
		/* Set AHB prescaler*/
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		/* Set APB1 Scaler */
		RCC_PCLK1Config(RCC_HCLK_Div2);
		/* Set APB2 Scaler*/
		RCC_PCLK2Config(RCC_HCLK_Div1);
		 /* Set Clock source to PLL*/
		 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	}else
	{
			//Set clock source to default config
			RCC_DeInit();
	}
	SystemCoreClockUpdate();
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
