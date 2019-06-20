/*
 * global.h
 *
 *  Created on: Mar 19, 2018
 *      Author: Do Yeou Ku
 */

#ifndef TASKS_GLOBAL_H_
#define TASKS_GLOBAL_H_

/* Integer includes. */
#include "fatfs/integer.h"

typedef struct {
	volatile UINT log_enabled;
	volatile UINT file_opened;
	volatile UINT filename_ok;
} FLAG;

FLAG file_flag;
char mydata[50];

/* Testing size: 100, 250, 500, 1000 */
#define TEST_BUFFER_SIZE	2

/* Testing type: not casted char, not casted uint8_t, casted char pointer */
uint8_t TEST_Buffer [TEST_BUFFER_SIZE];

#endif /* TASKS_GLOBAL_H_ */
