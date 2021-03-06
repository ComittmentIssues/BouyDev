/*
 * SD_Card.h
 * Based on http://www.rjhcoding.com/avrc-sd-interface-2.php
 *  Created on: Jun 9, 2019
 *      Author: Jamie
 */

#include "stm32f4xx.h"
#include "tm_stm32f4_gpio.h"
#ifndef SD_CARD_H_
#define SD_CARD_H_

#define SD_CARD_CS GPIO_Pin_5;
#define SD_SPI SPI1
#define SD_GPIO_PORT GPIOB
#define SD_BLOCK_Length 512
#define MAX_READ_ATTEMPTS 10000
//SET IDLE
#define CMD0        0
#define CMD0_ARG    0x00000000
#define CMD0_CRC    0x94
//SEND IF
#define CMD8        8
#define CMD8_ARG    0x0000001AA
#define CMD8_CRC    0x86
//READ BLOCK
#define CMD17                   17
#define CMD17_CRC               0x00
#define SD_MAX_READ_ATTEMPTS    1563
//REQUEST OCR
#define CMD58       58
#define CMD58_ARG   0x00000000
#define CMD58_CRC   0x00
//APP COMMAND
#define CMD55       55
#define CMD55_ARG   0x00000000
#define CMD55_CRC   0x00
//SEND OP CONDITION
#define ACMD41      41
#define ACMD41_ARG  0x40000000
#define ACMD41_CRC  0x00

/* private enums*/
typedef enum
{
	START_TOKEN = 0xFE,
	TIMEOUT = 0xFF
}SD_Token_t;

void init_SD_CS(void);
uint8_t init_SDCARD(void);
void SD_SET_CS(FunctionalState en);

//init functions
void SD_SET_SPI(void);
void SD_COMMAND(uint8_t cmd,uint32_t arg,uint8_t crc);
uint8_t read_R1(void);
void read_R7(uint8_t* R7);

//Transactions
SD_Token_t read_Block(uint32_t addr, uint8_t* buff,uint8_t* token);
#endif /* SD_CARD_H_ */
