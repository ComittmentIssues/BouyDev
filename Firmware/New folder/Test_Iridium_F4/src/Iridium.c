/*
 * Iridium.c
 *
 *  Created on: Jun 11, 2019
 *      Author: Jamie
 */

#include "Iridium.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
void init_Iridium_USART(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART3_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	/* ENABLE RCC */
	Iridium_GPIO_PeriphClockCommand(Iridium_GPIO_RCCPeriph,ENABLE);
	Iridium_USART_PeriphClockCommand(Iridium_USART_RCCPeriph,ENABLE);

	/* Configure GPIO to alternate function Open Drain Pull UP */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = (Iridium_USART_RX | Iridium_USART_TX);
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(Iridium_GPIO,&GPIO_InitStruct);
	//AF Mapping
	GPIO_PinAFConfig(Iridium_GPIO,Iridium_USART_RXsrc,Iridium_GPIO_AF);
	GPIO_PinAFConfig(Iridium_GPIO,Iridium_USART_TXsrc,Iridium_GPIO_AF);

	/* Configure USART	*/
	USART3_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART3_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART3_InitStruct.USART_StopBits = USART_StopBits_1;
	USART3_InitStruct.USART_Parity = USART_Parity_No;
	USART3_InitStruct.USART_BaudRate = Iridium_Baudrate;
	USART3_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(Iridium_USART, &USART3_InitStruct);
	/* Configure NVIC for interrupt */
	NVIC_InitStruct.NVIC_IRQChannel = Iridium_USART_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
	USART_ITConfig(Iridium_USART,USART_IT_RXNE,ENABLE);

	USART_Cmd(Iridium_USART,ENABLE);
}

/*configure Control pins */
void init_Control_Pins(void)
{
	//using port B
	GPIO_InitTypeDef GPIO_InitStruct;
	/*Wake up Pin*/
	GPIO_InitStruct.GPIO_Pin = Iridium_Wakeup_Pin;
	GPIO_InitStruct.GPIO_Mode =GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(Iridium_GPIO,&GPIO_InitStruct);
	GPIO_WriteBit(Iridium_GPIO,Iridium_Wakeup_Pin,RESET);
	/* Network availiable */
	/*
	 * 	3.3V Digital
	 *	Available = high
	 *	Not available= low
	 */
	GPIO_InitStruct.GPIO_Pin = Iridium_NetAv_Pin;
	GPIO_InitStruct.GPIO_Mode =GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(Iridium_GPIO,&GPIO_InitStruct);

	//place device into sleep mode
}
/* Function to Reset the data buffer and pointer*/
void init_Rx_Buff(void)
{
	Iridium_Buff_Index = 0;
	for (int i = 0; i < Iridium_RX_Buffsize; ++i)
	{
		Iridium_Rx_Buff[i] = 0;
	}
}

uint8_t init_Iridium(void)
{
	//init control pins
	init_Iridium_USART();
	init_Control_Pins();

	//wake up device
	GPIO_WriteBit(Iridium_GPIO,Iridium_Wakeup_Pin,SET);

	/*wait for network to become available*/
	//while(!GPIO_ReadInputDataBit(Iridium_GPIO,Iridium_NetAv_Pin));
	//test coms with device
	init_Rx_Buff();
	uint8_t tx[3] = "AT\r";
	transmit_Data(tx,length(tx));
	while(!IR_Rx_done);
	//determine if response was ok
	char* response = get_data_from_buff();
	uint8_t flag = -1;
	for (int i = 0; i < sizeof(response); ++i)
	{
		if(response[i] =='O')
		{
			if(response[i+1] == 'k')
			{
				flag = 0;
				break;
			}
		}
	}

	return flag;


}
void transmit_Data(uint8_t* tx_buff,size_t len)
{
	//TODO: timeout and error checking
	/* get size of pointer*/
	for (int i = 0; i < len; ++i)
	{
		USART_SendData(Iridium_USART,*(tx_buff++));
		while(!USART_GetFlagStatus(Iridium_USART,USART_FLAG_TXE));
	}
}

char* get_data_from_buff(void)
{
	IR_Rx_done = 0;
	char* temp = malloc(Iridium_Buff_Index+1);
	memcpy(temp,Iridium_Rx_Buff,Iridium_Buff_Index+1);
	init_Rx_Buff();

	return temp;
}

void get_Message(uint8_t* args)
{
	//send AT command

}
void Iridium_USART_IRQHandler(void)
{
	// set RX_done flag to 0
	/* get info*/

	uint16_t byte = Iridium_USART->DR;
	/* Add to buffer and increment pointer*/
	if(Iridium_Buff_Index < Iridium_RX_Buffsize)
	{
			Iridium_Rx_Buff[Iridium_Buff_Index] = (uint8_t)byte;
			if((Iridium_Rx_Buff[Iridium_Buff_Index] == 0xA)&&(Iridium_Rx_Buff[Iridium_Buff_Index] == 0xA))
			{
				//verbose message already started
				if(IR_RxV_st == 1)
				{
					IR_Rx_done = 1;
					IR_RxV_st = 0;
					USART_ITConfig(Iridium_USART,USART_IT_RXNE,DISABLE);
				}else
				{
					IR_RxV_st = 1;
				}
			}
			Iridium_Buff_Index++;
	}


	USART_ClearITPendingBit(Iridium_USART,USART_IT_RXNE);
}





