/*
 * Iridium.h
 *
 * Iridium communication uses USART3 for serial comms. This maps to GPIOB.
 * This can be changed in the defines
 *  Created on: Jun 11, 2019
 *      Author: Jamie
 */

#ifndef IRIDIUM_H_
#define IRIDIUM_H_

#include "stdio.h"
#include "stdint.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"

/*Private Macros*/
#define Iridium_GPIO_RCCPeriph RCC_AHB1Periph_GPIOB
#define Iridium_USART_RCCPeriph RCC_APB1Periph_USART3

#define Iridium_GPIO_PeriphClockCommand RCC_AHB1PeriphClockCmd
#define Iridium_USART_PeriphClockCommand RCC_APB1PeriphClockCmd

#define Iridium_USART USART3
#define Iridium_GPIO GPIOB

#define Iridium_NetAv_Pin GPIO_Pin_8
#define Iridium_Wakeup_Pin GPIO_Pin_9
#define Iridium_USART_TX GPIO_Pin_10
#define Iridium_USART_RX GPIO_Pin_11

#define Iridium_USART_TXsrc GPIO_PinSource10
#define Iridium_USART_RXsrc GPIO_PinSource11
#define Iridium_Wakeup_Pinsrc GPIO_PinSource9
#define Iridium_NetAv_Pinsrc GPIO_PinSource8

#define Iridium_GPIO_AF GPIO_AF_USART3
#define Iridium_USART_IRQHandler USART3_IRQHandler
#define Iridium_USART_IRQn USART3_IRQn
#define Iridium_Baudrate 19200
#define Iridium_Verbose_Token (0xD <<8)|(0xA) //'\r\n'

#define length(x) sizeof(x)/sizeof(*x)
#define Iridium_RX_Buffsize 1000 //increase if necessary
/* Private Variables */
uint8_t Iridium_Rx_Buff[Iridium_RX_Buffsize];
size_t Iridium_Buff_Index;
uint8_t IR_Rx_done;
uint8_t IR_RxV_st;
/*Private functions*/
void init_Iridium_USART(void);
void init_Rx_Buff(void);
void init_Control_Pins(void);
void transmit_Data(uint8_t* tx_buff,size_t len);
void Iridium_Rx_status(void);
char* get_data_from_buff(void);
char* send_AT(uint8_t* args);
void get_Message(uint8_t* args);
#endif /* IRIDIUM_H_ */
