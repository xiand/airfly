/*
*********************************************************************************************************
*
*	ģ������ : ����STM32F4�ڲ�TIM���PWM�źţ�˳��ʵ��
			 : �����ʱ������Ϊ�ж�ģʽ������жϴ����� ��Ҫ��stm32f4xx_it�ļ������ж��岢����
*	�ļ����� : bsp_tim_pwm.h
*********************************************************************************************************
*/

#ifndef __BSP_TIM_PWM_H
#define __BSP_TIM_PWM_H

#include "bsp.h"

void bsp_SetTimOutPWM(GPIO_TypeDef *GPIOx,uint16_t gpio_pinx,TIM_TypeDef *TIMx,uint8_t _ucChannel,uint32_t _uiFreq,uint32_t _uiDutyCycle);


void bsp_StopPwm(TIM_TypeDef *TIMx);


void bsp_SetTIMforInt(TIM_TypeDef* TIMx, uint32_t _ulFreq, uint8_t _PreemptionPriority, uint8_t _SubPriority);

/**********************************************************************
* @Fuctionname   : bsp_SetTimCaptureFunction
* @Brief         : 
* @Input         : None
* @Output        : None
* @Return        : None
* @Remark        : None
**********************************************************************/
	void bsp_SetTimCaptureFunction(TIM_TypeDef *TIMx, uint16_t _ucChannel,GPIO_TypeDef *GPIOx,uint16_t gpio_pinx,uint8_t _ucPreemptionPriority,uint8_t _ucSubPrority);



#endif
/************************************************************/
