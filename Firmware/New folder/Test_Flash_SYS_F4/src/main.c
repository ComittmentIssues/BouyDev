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


int main(void)
{
		init_RTC();
		STM_EVAL_LEDInit(LED3);STM_EVAL_LEDInit(LED5);STM_EVAL_LEDInit(LED6);
		set_RTCAlarm_A();



	while(1)
	{
		set_StdBy_Mode();
	}
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
