/*
*********************************************************************************************************
*
*	ģ������ : STM32F4  ��оƬ GPIO���Ź��ܷ�װ�������������Ϊ�ж�ģʽ������жϺ�����Ҫ��stm32f4xx_it.c�ļ��н������ж�����߼���д
*	�ļ����� : bsp_gpio.h
*********************************************************************************************************
*/


#ifndef __BSP_GPIO_H_
#define __BSP_GPIO_H_

#include "bsp.h"
//gpio���Ź��ܶ���
typedef enum functionDefine
{
	function_output,
	function_input,
	function_interrupt,
}functionDefine_t;

typedef enum interruptMode
{
	interrupt_rising = 2,
	interrupt_down = 3,
	interrupt_rising_down = 4,
}interruptMode_t;


void bsp_GpioSet(GPIO_TypeDef *GPIOx,uint16_t gpio_pinx,functionDefine_t function,interruptMode_t mode);

void bsp_GpioInterruptSet(uint16_t gpio_pinx,uint8_t ucPreemptionPriority, uint8_t ucSubPriority);

uint8_t bsp_GetPinStatus(GPIO_TypeDef *GPIOx, uint16_t gpio_pinx,functionDefine_t function);



#endif

