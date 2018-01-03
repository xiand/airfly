/*
*********************************************************************************************************
*
*	模块名称 : STM32F4  主芯片 GPIO引脚功能封装，如果引脚设置为中断模式，相关中断函数需要到stm32f4xx_it.c文件中进行自行定义和逻辑编写
*	文件名称 : bsp_gpio.h
*********************************************************************************************************
*/


#ifndef __BSP_GPIO_H_
#define __BSP_GPIO_H_

#include "bsp.h"
//gpio引脚功能定义
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

