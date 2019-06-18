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
#include "Delay.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4_discovery.h"
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

	USART_Cmd(Iridium_USART,ENABLE);

}

/*configure Control pins */
void init_Control_Pins(void)
{
	//using port B
	Iridium_GPIO_PeriphClockCommand(RCC_AHB1Periph_GPIOB,ENABLE);
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
	GPIO_InitTypeDef GPIO_NETAV;
	GPIO_NETAV.GPIO_Pin = Iridium_NetAv_Pin;
	GPIO_NETAV.GPIO_Mode =GPIO_Mode_IN;
	GPIO_NETAV.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_NETAV.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(Iridium_GPIO,&GPIO_NETAV);

	//configure NetAv pin to EXTI line
	EXTI_InitTypeDef EXTI_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	EXTI_InitStruct.EXTI_Line = NetAv_EXTI_Line;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode =EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising; //when high
	EXTI_Init(&EXTI_InitStruct);
	SYSCFG_EXTILineConfig(NetAv_EXTIPortsource,NetAv_EXTIPinSource);
	NVIC_InitTypeDef  NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = NetAv_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}
/* Function to Reset the data buffer and pointer*/

int8_t init_Iridium_Module(void)
{
	init_Iridium_USART();
	init_Control_Pins();
	init_Rx_Buff();
	//wait for module to fully power out
	char* msg = send_ATcmd("AT\r",1000);
	if(strcmp(msg,(char*)"OK") != 0)
	{

		if(strcmp(msg,(char*)"TIMEOUT") == 0)
		{
			//no response received
			free(msg);
			return -2;
		}
		if(strcmp(msg,(char*)"ERROR") == 0)
		{
			//message recieved but not understood
			free(msg);
			return -3;
		}else
		{
			//garbage
			free(msg);
			return -4;
		}
	}
	free(msg);
	//check signal strength

	return 0;
}

void deinit_Iridium_Module(void)
{
	 //Clear Data Register and disable
	 Iridium_USART->DR = 0;
	 USART_DeInit(Iridium_USART);
	 USART_ITConfig(Iridium_USART,USART_IT_RXNE,DISABLE);
	 NVIC_DisableIRQ(Iridium_USART_IRQn);

	 RCC_APB1PeriphClockCmd(Iridium_USART_RCCPeriph,DISABLE);

	 // deinit pins and place in weak pull up
	 GPIO_InitTypeDef  GPIO_InitStructure;
	 GPIO_InitStructure.GPIO_Pin = Iridium_USART_RX|Iridium_USART_TX;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	 GPIO_Init(Iridium_GPIO, &GPIO_InitStructure);

}

void init_Rx_Buff(void)
{
	Iridium_Buff_Index = 0;
	for (int i = 0; i < Iridium_RX_Buffsize; ++i)
	{
		Iridium_Rx_Buff[i] = 0;
	}
}

void transmit_Data(char* tx_buff,size_t len)
{
	//TODO: timeout and error checking
	/* get size of pointer*/
	for (int i = 0; i < len; ++i)
	{
		USART_SendData(Iridium_USART,*(tx_buff++));
		while(!USART_GetFlagStatus(Iridium_USART,USART_FLAG_TXE));
	}
}
/*
 *  Function to initiate an SBDquery session
 *  NOTE:
 	 *Data must be less than 340 bytes in length
 	 *Network availability does not result in success
 	 *  Successful query will return: +SBDIX:<MO status>,<MOMSN>,<MT status>,<MTMSN>,<MT length>,<MT queued>
		 * MO status:
		 	 * 0 - 2 successful transmit
		 	 * 32 - no network available
		 	 * anything else is a failure
		 *MOMSN:
		 	 *Mobile Originated Message Sequence Number
		 	 *incremented each time successful
		 *MT Status:
		 	 * 0 - no message to receive
		 	 * 1 - message successfully received
		 	 * 2 - error checking mailbox
		 *MTMSN:
		 	 *The Mobile Terminated Message Sequence Number
		 *MT length
		 	 *Number of bytes received
		 *Count of mobile terminated SBD messages waiting at the GSS to be transferred to the ISU.
		 	 */
uint8_t create_SBD_Session(void)
{
	/* Initiate SBD session */
	char* cmd = send_ATcmd("AT+SBDIX\r",50000);

		//error = wait a few seconds then try again
	if(strcmp(cmd,(char*)"TIMEOUT\0") == 0)
	{
		//timeout
		free(cmd);
		return -1;
	}
	/*check status of message received*/
	//parse message into status
	get_status(cmd);

	if(SBDIX_status[0] > 2)
	{
		/*
		 * if network unavailable, wait for active network and try again
		 */
		if(SBDIX_status[0] == 32)
		{
			//network unavailable
			return -2;
		}
		else
		{
			//failiure to send
			//wait 3 seconds try again
			//wait longer if still fail
			return -3;
		}
	}



	/* flush MO queue*/
	cmd = send_ATcmd("AT+SBDD0\r",50000);
	free(cmd);
	/**/
	return 0;
}

void get_status(char* cmd)
{
	uint8_t count = 0;

	for(int j = 0; j < 6; j++)
	{
		uint8_t numcount =0 ;
		uint len = strlen(cmd);
		while(cmd[count] != ',')
		{
			if((count) == len)
			{
				break;
			}
			if(cmd[count] >= '0' && cmd[count] <= '9')
			{
				numcount++;
			}

			count++;
		}
		//combine numbers
		int16_t temp = 0;

		for (int i = 0; i < numcount; ++i)
		{
			temp = temp*10+ (cmd[count -numcount+i ]-48);
			if (cmd[count-numcount -1] == '-')
			{
				temp = temp*(-1);
			}
		}
		SBDIX_status[j] = temp;
		count++;
	}
}

void clear_Status(void)
{
	for (int i = 0; i < 6; ++i)
	{
		SBDIX_status[i] = 0;
	}
}
/*
 * Retrieve the Iridium response only from Rx buffer
 */
char* get_AT_response(size_t msg_Size)
{
	//clear flag
	IR_Rx_done = 0;
	//find start of verbose message
	size_t size = 0;
	for (int i = 0; i < Iridium_Buff_Index; ++i)
	{
		if(Iridium_Rx_Buff[i] == '\r')
		{
			msg_Size = size+1;
			size = Iridium_Buff_Index - msg_Size -4;
			break;
		}
		size++;
	}
	//create a null character to terminate string copy
	Iridium_Rx_Buff[Iridium_Buff_Index-2 ] = '\0';
	char* temp_ptr = (char*)&Iridium_Rx_Buff[msg_Size+2];
	strcpy(message_buff,temp_ptr);
	init_Rx_Buff();
	return message_buff;

}
/*
 * Sends an AT command over USART
 * returns the response
 */
char* send_ATcmd(char* cmd,uint32_t delay)
{
	Delay_begin_Timeout(delay);
	transmit_Data(cmd,strlen(cmd));
	//timeout after 60 seconds of no activity
	Delay_Enable();
	USART_ITConfig(Iridium_USART,USART_IT_RXNE,ENABLE);

	while(IR_Rx_done != 1)
	{
		if(timeout_flag)
		{
			timeout_flag = 0;
			//Disable Interrupt
			USART_ITConfig(Iridium_USART,USART_IT_RXNE,DISABLE);
			return (char*)"TIMEOUT";
		}
	}
	//disable
	USART_ITConfig(Iridium_USART,USART_IT_RXNE,DISABLE);
	return get_AT_response(strlen(cmd));
}

uint8_t send_ASCII_Message(char* msg)
{
	/* Set Flow control off*/
	char* cmd = send_ATcmd("AT&K0\r",100);
	if(strcmp(cmd,(char*)"OK") != 0)
	{
		if(strcmp(msg,(char*)"TIMEOUT")==0)
		{
			return -1;
		}
		else return -2;
	}
	free(cmd);
	/* create message string*/
	size_t len = strlen(msg)+strlen("AT+SBDWT=\r");
	char atarr[len];
	memset(atarr,0,len);
	strcat(atarr,"AT+SBDWT=");
	strcat(atarr,msg);
	cmd = send_ATcmd(atarr,10000);
	if(strcasecmp(cmd,(char*)"OK") !=0)
	{
		if(strcmp(msg,(char*)"TIMEOUT")==0)
		{
			//timeout during transmission
			free(atarr);
			return -3;

		}else
		{
			//module did not recieve correctly
			free(atarr);
			return -4;
		}
	}
	//begin SBD extended session
	free(atarr);
	return 0;
}

/** IRQ HANDLERS **/
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
	}else
	{
		//if buffer is full, end transmission
		IR_Rx_done = 1;
		IR_RxV_st = 0;
		USART_SendBreak(Iridium_USART);
		USART_ITConfig(Iridium_USART,USART_IT_RXNE,DISABLE);
	}


	USART_ClearITPendingBit(Iridium_USART,USART_IT_RXNE);
}

void NetAV_EXTI_IRQHandler(void)
{
	if(EXTI_GetITStatus(NetAv_EXTI_Line) == SET)
	{
		if(Wait_for_network)
		{
			if(create_SBD_Session() == 0)
			{
				Wait_for_network = 0;
				STM_EVAL_LEDOn(LED3);
			}
		}
	}
	 EXTI_ClearITPendingBit(NetAv_EXTI_Line);
}



