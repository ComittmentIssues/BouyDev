/*
 * memory.h
 *
 *  Created on: Feb 26, 2018
 *      Author: Do Yeou Ku
 */

#ifndef TASKS_MEMORY_H_
#define TASKS_MEMORY_H_


/* Standard includes. */
#include "stm32f4xx.h"

/* Timer variable defines.*/
#define Timer_TIM2_PSC	819
#define Timer_TIM2_ARR	999
// Update Event (Hz) = timer_clock / ((TIM_Prescaler + 1) * (TIM_Period + 1)
// 100 Hz: 820 , 1000

/* Timer function prototypes. */
void TIM2_IRQHandler (void);
void timer_initialize(void);
/* Fatfs includes. */
#include "fatfs/ff.h"

/* Global incudes */
#include "tasks/global.h"

/* Fatfs variable defines. */

enum {
	FAIL		=	0,
	SD_CARD_OK	=	1,
	LOGGING_ON	=	2,
	PASS		=	3,
};

/* Fatfs function prototypes. */
char check_memory (
	FATFS *fs,
	FIL *fp,
	FRESULT err
	);

char write_to_memory	(
	FATFS *fs,
	FIL *fp,
	FRESULT err,
	const TCHAR *path,
	UINT *bw,
	const TCHAR *data,
	UINT data_length,
	FLAG *f
	);



#endif /* TASKS_MEMORY_H_ */
