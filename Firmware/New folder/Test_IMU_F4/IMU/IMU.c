/*
 * IMU.c
 *
 *  Created on: Jul 13, 2019
 *      Author: Jamie
 */

#include "IMU.h"
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
/* Private Variables*/
int16_t samples[NAxis][Sample_Size];
TM_MPU6050_t MPU6050_Data0;
TM_LIS302DL_LIS3DSH_t LIS3DSH_Data0;
int16_t count = 0;
/* Private Function Definitions*/
void init_Timer(void)
{

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);

		TIM_TimeBaseInitTypeDef timerInitStructure; //create a 1ms delaay
		RCC_ClocksTypeDef rcc;
		RCC_GetClocksFreq(&rcc);

		timerInitStructure.TIM_Prescaler = Sample_PSC;
		timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
		timerInitStructure.TIM_Period= rcc.HCLK_Frequency/(2*SAMPLE_RATE*Sample_PSC);
		timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		timerInitStructure.TIM_RepetitionCounter = 0;
		TIM_TimeBaseInit(Sample_Timer, &timerInitStructure);

		/* Prevent interrupt from triggering*/
		TIM_ClearITPendingBit(Sample_Timer,TIM_IT_Update);
		TIM_UpdateRequestConfig(TIM2,TIM_UpdateSource_Regular);
		TIM_ITConfig(Sample_Timer, TIM_IT_Update,ENABLE);

		NVIC_InitTypeDef nvicStructure;
	    nvicStructure.NVIC_IRQChannel = TIM7_IRQn;
		nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
		nvicStructure.NVIC_IRQChannelSubPriority = 1;
		nvicStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicStructure);
		TIM_Cmd(Sample_Timer,ENABLE);
		//enable interrupts

}

uint8_t init_IMU(void)
{
	uint8_t flag = 0;
#ifdef USE_EXT_IMU
	flag = (TM_MPU6050_Init(&MPU6050_Data0, TM_MPU6050_Device_0, TM_MPU6050_Accelerometer_8G, TM_MPU6050_Gyroscope_250s) == TM_MPU6050_Result_Ok);
	STM_EVAL_LEDOn(LED3);
	#else
	if(TM_LIS302DL_LIS3DSH_Detect() == TM_LIS302DL_LIS3DSH_Device_LIS302DL)
	{
		TM_LIS302DL_LIS3DSH_Init(TM_LIS302DL_Sensitivity_2_3G, TM_LIS302DL_Filter_2Hz);
		flag = 1;
		STM_EVAL_LEDOn(LED4);
	}if(TM_LIS302DL_LIS3DSH_Detect() == TM_LIS302DL_LIS3DSH_Device_LIS3DSH)
	{
		TM_LIS302DL_LIS3DSH_Init(TM_LIS3DSH_Sensitivity_4G, TM_LIS3DSH_Filter_50Hz);
		STM_EVAL_LEDOn(LED5);
		flag = 1;
	}else
	{
		flag = 0;

	}
#endif
	return flag;
}

void read_IMU_Data(void)
{
#ifdef USE_EXT_IMU
	TM_MPU6050_ReadAccelerometer(&MPU6050_Data0);
	TM_MPU6050_ReadGyroscope(&MPU6050_Data0);
	samples[0][count] = MPU6050_Data0.Accelerometer_X;
	samples[1][count] = MPU6050_Data0.Accelerometer_Y;
	samples[2][count] = MPU6050_Data0.Accelerometer_Z;
	samples[3][count] = MPU6050_Data0.Gyroscope_X;
	samples[4][count] = MPU6050_Data0.Gyroscope_Y;
	samples[5][count++] = MPU6050_Data0.Gyroscope_Z;
#else
	TM_LIS302DL_LIS3DSH_ReadAxes(&LIS3DSH_Data0);
	samples[0][count] = LIS3DSH_Data0.X;
	samples[1][count] = LIS3DSH_Data0.Y;
	samples[2][count++] = LIS3DSH_Data0.Z;
#endif

}
void deinit_Timer(void)
{
	TIM_Cmd(Sample_Timer,DISABLE);
	TIM_ITConfig(Sample_Timer, TIM_IT_Update,DISABLE);
	TIM_DeInit(TIM7);
	NVIC_DisableIRQ(TIM7_IRQn);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,DISABLE);
}
/****** IRQ HANDLER **************/
void TIM7_IRQHandler(void)
{
	if(count == Sample_Size)
	{
		TIM_Cmd(Sample_Timer,DISABLE);
		STM_EVAL_LEDOn(LED3);STM_EVAL_LEDOn(LED4);STM_EVAL_LEDOn(LED5);STM_EVAL_LEDOn(LED6);
	}else
	{
		STM_EVAL_LEDToggle(LED6);
		read_IMU_Data();
#ifdef USE_EXT_IMU

#else
#endif

	}
	TIM_ClearITPendingBit(Sample_Timer,TIM_IT_Update);
}
