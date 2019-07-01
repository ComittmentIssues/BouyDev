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
#include "stm32f4_discovery.h"

void go_to_sleep(void);


int main(void)
{

	if((PWR->CSR & PWR_CSR_SBF))
	{
		PWR_ClearFlag(PWR_FLAG_SB);
		PWR_ClearFlag(PWR_FLAG_WU);
		STM_EVAL_LEDInit(LED3);
		STM_EVAL_LEDOn(LED3);
	}else
	{
		STM_EVAL_LEDInit(LED4);
		STM_EVAL_LEDOn(LED4);
	}
	if(PWR_GetFlagStatus(PWR_FLAG_WU))
	{
		return PWR_FLAG_SB;
	}
  while (1)
  {
	  go_to_sleep();
  }
}
void go_to_sleep(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	PWR_WakeUpPinCmd(ENABLE);
	PWR_EnterSTANDBYMode();

}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
