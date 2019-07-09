/*
 * DS18B20.c
 *
 *  Created on: Jul 8, 2019
 *      Author: Jamie
 */

#include "DS18B20.h"
static void init_OneWire_GPIO(void);
static void SWI_sendBit(int i);




static void init_OneWire_GPIO(void)
{
	/* clock power*/
	RCC_AHB1PeriphClockCmd(RCC_OneWirePeriph,ENABLE);
	GPIO_InitTypeDef OneWire_struct;
	OneWire_struct.GPIO_Mode = GPIO_Mode_OUT;
	OneWire_struct.GPIO_OType = GPIO_OType_OD;
	OneWire_struct.GPIO_Speed = GPIO_Speed_100MHz;
	OneWire_struct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(OneWire_Port,&OneWire_struct);
}

 uint8_t reset_OneWire(void)
{
	/* RESET to signal Bus*/
		SWI_LOW;
		DWT_delay_us(480);

		SWI_FLOAT;
		DWT_delay_us(60);
		/* Check if Bus is high*/
		if((OneWire_Port->ODR &GPIO_ODR_ODR_6) == 1)
		{
			return 1;
		}

		return 0;

}

static void SWI_sendBit(int i)
{
	if(i == 1)
	{
		SWI_LOW;
		DWT_delay_us(60);
		SWI_FLOAT;
		DWT_delay_us(15);
	}else
	{
		SWI_LOW;
		DWT_delay_us(5);
		SWI_FLOAT;
		DWT_delay_us(60);
	}
}

void SWI_sendByte(uint8_t byte)
{
	for (int i = 0; i < 8; ++i)
	{
		uint8_t bit = byte&(0b1<<i)>>i;
		SWI_sendBit(bit);
	}
}

uint8_t SWI_readByte(void)
{
	uint8_t recieved;
	for (int i = 0; i < 8; ++i)
	{
		SWI_LOW;
		DWT_delay_us(5);
		SWI_FLOAT;
		DWT_delay_us(10);
		recieved = recieved >> 1;
		uint8_t temp = 0x1 & (OneWire_Port->IDR >> 6);
		recieved = recieved | (temp<< 7);
		DWT_delay_us(30);
	}
	return recieved;
}
uint8_t init_OneWire(void)
{
	init_OneWire_GPIO();
	DWT_Delay_init();
	//set Bus HIGH
	SWI_FLOAT;
	DWT_delay_us(10);
	if((OneWire_Port->ODR &GPIO_ODR_ODR_6) == 1)
	{
		//error setting bus high
		return 1;
	}
	//reset the bus
	if(reset_OneWire())
	{
		// no devices present
		return 1;
	}
	//found a device
	return 0;
}
/*
 * DWT Initalise delay
 */

uint32_t DWT_Delay_init(void)
{
	/* Disable TRC */
	 CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
	 /* Enable TRC */
	 CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;
	 /* Disable clock cycle counter */
	 DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
	 /* Enable clock cycle counter */
	 DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; //0x00000001;
	 /* Reset the clock cycle counter value */
	 DWT->CYCCNT = 0;
	/* 3 NO OPERATION instructions */
	__NOP();
	__NOP();
	__NOP();
	 /* Check if clock cycle counter has started */
	if(DWT->CYCCNT)
	{
	 return 0; /*clock cycle counter started*/
	}
	else
	 {
	 return 1; /*clock cycle counter not started*/
	 }
}
