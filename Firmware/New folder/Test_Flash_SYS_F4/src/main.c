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

#define length(x) sizeof(x)/sizeof(*x)

int8_t Save_Data(uint16_t* data_buf,uint16_t virtual_base_address, uint32_t buff_len);
int8_t load_Data(uint16_t*  data_buf,uint16_t virtual_base_address, uint32_t buff_len);
/* Virtual address defined by the user: 0xFFFF value is prohibited */
uint16_t VirtAddVarTab[NB_OF_VAR] = {0x5555, 0x6666, 0x7777};
uint16_t VarDataTab[NB_OF_VAR] = {0, 0, 0};
uint16_t VarValue = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{

			/* Save data 1*/
			uint16_t  data1[] = {1,2,3,4,5,256,512,1024,2048,4098};
			uint16_t rxbuff1[length(data1)];
			uint32_t Virt_base_address = 0x1000;
			load_Data(rxbuff1,Virt_base_address,length(rxbuff1));
			while (1);
}

/*
 * Store an Array of Data in halfword format and send to virtual eeprom
 * takes */
int8_t Save_Data(uint16_t* data_buf,uint16_t virtual_base_address, uint32_t buff_len)
{
	FLASH_Unlock();
	EE_Init();
	for (int i = 0; i < buff_len; ++i)
	{
		EE_WriteVariable(virtual_base_address+i, data_buf[i]);
	}

	return 0;
}

int8_t load_Data(uint16_t*  data_buf,uint16_t virtual_base_address, uint32_t buff_len)
{
	for (int i = 0; i < buff_len;++i)
	{
		if(EE_ReadVariable(virtual_base_address+i,&data_buf[i]) == 1)
		{
			/* invalid virtual address given*/
			return -1;
		}
	}
	return 0;
}
/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
