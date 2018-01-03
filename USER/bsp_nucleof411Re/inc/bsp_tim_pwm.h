/*
*********************************************************************************************************
*
*	模块名称 : 利用STM32F4内部TIM输出PWM信号，顺便实现
			 : 如果定时器定义为中断模式，相关中断处理函数 需要到stm32f4xx_it文件中自行定义并处理
*	文件名称 : bsp_tim_pwm.h
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
