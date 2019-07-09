/*
 * DS18B20.h
 *
 *  Created on: Jul 8, 2019
 *      Author: Jamie
 */

#ifndef DS18B20_H_
#define DS18B20_H_

#include "stm32f4xx.h"

#define OneWire_Port GPIOD
#define OneWire_Pin GPIO_Pin_6
#define RCC_OneWirePeriph RCC_AHB1Periph_GPIOD

#define SWI_FLOAT (GPIOD->ODR |=  GPIO_ODR_ODR_6)
#define SWI_LOW	  (GPIOD->ODR &= ~GPIO_ODR_ODR_6)

uint8_t init_OneWire(void);
uint8_t reset_OneWire(void);
void SWI_sendByte(uint8_t byte);
uint8_t SWI_readByte(void);

/* DWT Timer for microsecond delay*/
/*
 * Source: Knut Knusper
 * Taken from https://community.st.com/s/question/0D50X00009XkeRYSAZ/delay-in-us
 */
uint32_t DWT_Delay_init();

__STATIC_INLINE void DWT_delay_us(uint32_t us)
{
	/* Get number of cycles*/
	uint32_t cycle_start = DWT->CYCCNT;
	/* Get number of ticks*/
	uint32_t multiplier = SystemCoreClock/2000000;
	us *= multiplier;
	/* Delay*/
	while((DWT->CYCCNT - cycle_start) < us);
}

#endif /* DS18B20_H_ */
