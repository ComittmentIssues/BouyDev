/*
 * SD_Card.c
 * Adapted for SPL from https://github.com/afiskon/stm32-sdcard/blob/master/sdcard/sdcard.c#L92
 * Further reading http://www.dejazzer.com/ee379/lecture_notes/lec12_sd_card.pdf
 *  Created on: Jun 9, 2019
 *      Author: Jamie
 */
#include "SD_Card.h"
#include "tm_stm32f4_spi.h"
#include "stdio.h"
#include "tm_stm32f4_delay.h"
void SD_SET_CS(FunctionalState en)
{
	//Low active -> 0 = enable

	if(en)
	{
		GPIO_WriteBit(GPIOB,GPIO_PIN_5,RESET);
	}
	else
	{
		GPIO_WriteBit(GPIOB,GPIO_PIN_5,SET);
	}
}
void ASSERT_CS(void)
{
	TM_SPI_Send(SD_SPI,0xFF);
	SD_SET_CS(ENABLE);
	TM_SPI_Send(SD_SPI,0xFF);
}
void DEASSERT_CS(void)
{
	TM_SPI_Send(SD_SPI,0xFF);
	SD_SET_CS(DISABLE);
	TM_SPI_Send(SD_SPI,0xFF);
}
void init_SD_CS(void)
{
	TM_GPIO_Init(GPIOB,GPIO_PIN_5,TM_GPIO_Mode_OUT,TM_GPIO_OType_PP,TM_GPIO_PuPd_DOWN,TM_GPIO_Speed_Fast);
}


/*
 * Read 8bit return from SD card
 *
 * format:
 *  		7th bit	:	cmd arg outside range
 *  		6th bit :	misaligned address in command
 *  		5th bit :	error in sequence of commands
 *  		4th bit :	crc check error
 *  		3rd bit :	illigal command
 *  		2nd bit :	rase sequence clear
 *  		1st bit :	card in idle state
 */
uint8_t read_R1(void)
{
	uint8_t r1;
	while(1)
	{
		TM_SPI_ReadMulti(SD_SPI,&r1,0xFF,1);
		if((r1&0x80)==0)
		{
			break;
		}
	}
	return r1;
}

void read_R7(uint8_t* R7)
{
	TM_SPI_ReadMulti(SD_SPI,R7,0xFF,1);
	R7++;
	if(R7[0] > 0b1) return;
	TM_SPI_ReadMulti(SD_SPI,R7,0xFF,4);
}
void SD_SET_SPI(void)
{
	//set CS HIGH
	SD_SET_CS(ENABLE);
	for (int i = 0; i < 10; ++i)
	{
		TM_SPI_Send(SD_SPI,0xFF);
	}
	SD_SET_CS(DISABLE);

}
void SD_COMMAND(uint8_t cmd,uint32_t arg,uint8_t crc)
{
    // transmit command to sd card
    TM_SPI_Send(SD_SPI,(cmd|0x40));
    // transmit argument
    TM_SPI_Send(SD_SPI,(uint8_t)(arg >> 24));
    TM_SPI_Send(SD_SPI,(uint8_t)(arg >> 16));
    TM_SPI_Send(SD_SPI,(uint8_t)(arg >> 8));
    TM_SPI_Send(SD_SPI,(uint8_t)(arg));
    // transmit crc
    TM_SPI_Send(SD_SPI,(crc|0x01));
}
uint8_t init_SDCARD(void)
{
	/*Initialise SPI and chip select*/
	TM_SPI_Init(SD_SPI,TM_SPI_PinsPack_1);
	init_SD_CS();
	TM_DELAY_Init();

	/*SET SD CARD INTO SPI MODE AND PLACE IN IDLE STATE*/
	SD_SET_SPI();
	ASSERT_CS();
	SD_COMMAND(CMD0,CMD0_ARG,CMD0_CRC);
	uint8_t r1 = read_R1();
	DEASSERT_CS();
	if(r1 != 0b1)
	{
		return -1;
	}
	/*Send Interface Condition To ensure Valid, Compatible card*/
	ASSERT_CS();
	SD_COMMAND(CMD8,CMD8_ARG,CMD8_CRC);
	uint8_t R7[4];
	r1 = read_R1();
	read_R7(R7);
	uint16_t ccr = (R7[2]<<8)| R7[3];
	if(r1 != 0b1)
		{
			return -2;
		}
	if(ccr != 0x1aa)
	{
		return -3;
	}

	DEASSERT_CS();
	/*Send App Command to INiti */
	for(;;)
	{
		ASSERT_CS();
		SD_COMMAND(CMD55,CMD55_ARG,CMD55_CRC);
		r1 = read_R1();
		if(r1 != 0b1)
		{
			return -2;
		}
		DEASSERT_CS();
		ASSERT_CS();
		SD_COMMAND(ACMD41,ACMD41_ARG,ACMD41_CRC);
		r1 = read_R1();
		if(r1 == 0)
		{
			DEASSERT_CS();
			break;
				}
		if(r1 != 0b1)
		{

			return -2;
		}
		Delayms(10);
		DEASSERT_CS();
	}
	/*Get OCR Information and check CCS bit*/
		ASSERT_CS();
		SD_COMMAND(CMD58,CMD58_ARG,CMD58_CRC);
		r1 = read_R1();
		uint8_t OCR[5];
		read_R7(OCR);
		//check CCS bit
		if(!(OCR[0]&0x80))
		{
			DEASSERT_CS();
			return -3;
		}
		DEASSERT_CS();
	return 0;
}

uint8_t read_Block(uint32_t addr, uint8_t* buff,SD_Token_t* token)
{
	uint8_t R1, read;
	uint16_t readAttempts;
	*token = TIMEOUT;
	ASSERT_CS();
	/* Transmit CMD1 and Read Status R1*/
	SD_COMMAND(CMD17,addr,CMD17_CRC);
	R1 = read_R1();
	if(R1 != 0xFF )
	{
		/* Response Received from card*/
		readAttempts = 0;
		/* try to read card until successful or max attempts reached*/
		while(++readAttempts <= MAX_READ_ATTEMPTS)
		{
			TM_SPI_ReadMulti(SD_SPI,&read,0xFF,1);
			// if response token is 0xFE
			        if(read == 0xFE)
			        {
			            // read 512 byte block
			            TM_SPI_ReadMulti(SD_SPI,buff,0xFF,512);

			            // read 16-bit CRC
			            uint8_t temp[2];
			            TM_SPI_ReadMulti(SD_SPI,temp,0xFF,2);
			        }
			if(read != 0xFF) break;
			*token = read;
		}
	}

	DEASSERT_CS();
	return R1;
}
