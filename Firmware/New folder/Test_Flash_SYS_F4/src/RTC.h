/*
 * RTC.h
 *  Library for initializing Real Time clock on an STM32F407 Discovery board
 *  Based on Application Note AN3371 from STM
 *  Created on: Jul 2, 2019
 *      Author: Jamie Jacobson
 */

#ifndef RTC_H_
#define RTC_H_

/* Function Prototypes*/
void init_RTC(void);
void init_RTC_LSI(void);
void set_RTCAlarm_A(void);
void set_StdBy_Mode(void);
void set_StdBy_Mode(void);
void init_AutoWakeUp(void);

#endif /* RTC_H_ */
