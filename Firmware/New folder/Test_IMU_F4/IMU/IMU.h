/*
 * IMU.h
 *
 *  Created on: Jul 13, 2019
 *      Author: Jamie
 */

#ifndef IMU_H_
#define IMU_H_

#include "../IMU/tm_stm32f4_i2c.h"
#include "../IMU/tm_stm32f4_mpu6050.h"
#include "../IMU/tm_stm32f4_lis302dl_lis3dsh.h"
#include "eeprom.h"

#define ADDRESS 0xD0
#define SAMPLE_RATE 20 //Hz
#define Sample_Timer TIM7
#define Sample_PSC  65535
#define Sample_Size 2400

#define USE_EXT_IMU
#define USE_INT_IMU

#ifdef USE_EXT_IMU
#define NAxis 6 //incl magnetometer
#else
#define NAxis 3 //accelerator only
#endif

uint8_t sample_finished;

uint8_t init_IMU(void);
void init_Timer(void);
void deinit_Timer(void);
#endif /* IMU_H_ */
