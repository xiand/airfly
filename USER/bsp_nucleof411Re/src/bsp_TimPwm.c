
#include "bsp.h"


uint32_t bsp_GetRccOfGPIOx(GPIO_TypeDef * GPIOx)
{
	uint32_t		ret = 0x01;

	if (GPIOx == GPIOA)
		ret = (uint32_t) ((GPIOx - AHB1PERIPH_BASE) >> 10);

	return ret;
}


uint32_t bsp_GetPinSource(uint16_t gpio_pin)
{
	uint16_t		ret;

	if (gpio_pin == GPIO_Pin_0)
	{
		ret 				= GPIO_PinSource0;
	}
	else if (gpio_pin == GPIO_Pin_1)
	{
		ret 				= GPIO_PinSource1;
	}
	else if (gpio_pin == GPIO_Pin_2)
	{
		ret 				= GPIO_PinSource2;
	}
	else if (gpio_pin == GPIO_Pin_3)
	{
		ret 				= GPIO_PinSource3;
	}
	else if (gpio_pin == GPIO_Pin_4)
	{
		ret 				= GPIO_PinSource4;
	}
	else if (gpio_pin == GPIO_Pin_5)
	{
		ret 				= GPIO_PinSource5;
	}
	else if (gpio_pin == GPIO_Pin_6)
	{
		ret 				= GPIO_PinSource6;
	}
	else if (gpio_pin == GPIO_Pin_7)
	{
		ret 				= GPIO_PinSource7;
	}
	else if (gpio_pin == GPIO_Pin_8)
	{
		ret 				= GPIO_PinSource8;
	}
	else if (gpio_pin == GPIO_Pin_9)
	{
		ret 				= GPIO_PinSource9;
	}
	else if (gpio_pin == GPIO_Pin_10)
	{
		ret 				= GPIO_PinSource10;
	}
	else if (gpio_pin == GPIO_Pin_11)
	{
		ret 				= GPIO_PinSource11;
	}
	else if (gpio_pin == GPIO_Pin_12)
	{
		ret 				= GPIO_PinSource12;
	}
	else if (gpio_pin == GPIO_Pin_13)
	{
		ret 				= GPIO_PinSource13;
	}
	else if (gpio_pin == GPIO_Pin_14)
	{
		ret 				= GPIO_PinSource14;
	}
	else if (gpio_pin == GPIO_Pin_15)
	{
		ret 				= GPIO_PinSource15;
	}

	return ret;
}


uint32_t bsp_GetRccOfTim(TIM_TypeDef * Timx)
{
	uint32_t		rcc;

	/*
		APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
		APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
		详细可以可以参考 stm32f4xx.h文件，可以详细看到每一个总线下面的外设
	*/
	if (Timx == TIM1)
	{
		rcc 				= RCC_APB2Periph_TIM1;
	}
	else if (Timx == TIM8)
	{
		rcc 				= RCC_APB2Periph_TIM8;
	}
	else if (Timx == TIM9)
	{
		rcc 				= RCC_APB2Periph_TIM9;
	}
	else if (TIMx == TIM10)
	{
		rcc 				= RCC_APB2Periph_TIM10;
	}
	else if (TIMx == TIM11)
	{
		rcc 				= RCC_APB2Periph_TIM11;
	}

	/* 下面是 APB1时钟 */
	else if (TIMx == TIM2)
	{
		rcc 				= RCC_APB1Periph_TIM2;
	}
	else if (TIMx == TIM3)
	{
		rcc 				= RCC_APB1Periph_TIM3;
	}
	else if (TIMx == TIM4)
	{
		rcc 				= RCC_APB1Periph_TIM4;
	}
	else if (TIMx == TIM5)
	{
		rcc 				= RCC_APB1Periph_TIM5;
	}
	else if (TIMx == TIM6)
	{
		rcc 				= RCC_APB1Periph_TIM6;
	}
	else if (TIMx == TIM7)
	{
		rcc 				= RCC_APB1Periph_TIM7;
	}
	else if (TIMx == TIM12)
	{
		rcc 				= RCC_APB1Periph_TIM12;
	}
	else if (TIMx == TIM13)
	{
		rcc 				= RCC_APB1Periph_TIM13;
	}
	else if (TIMx == TIM14)
	{
		rcc 				= RCC_APB1Periph_TIM14;
	}

	return rcc;
}


uint8_t bsp_GetAfOfTim(TIM_TypeDef * TIMx)
{
	uint8_t 		ret = 0;

	if (TIMx == TIM1)
	{
		ret 				= GPIO_AF_TIM1;
	}
	else if (TIMx == TIM8)
	{
		ret 				= GPIO_AF_TIM8;
	}
	else if (TIMx == TIM9)
	{
		ret 				= GPIO_AF_TIM9;
	}
	else if (TIMx == TIM10)
	{
		ret 				= GPIO_AF_TIM10;
	}
	else if (TIMx == TIM11)
	{
		ret 				= GPIO_AF_TIM11;
	}

	/* 下面是 APB1时钟 */
	else if (TIMx == TIM2)
	{
		ret 				= GPIO_AF_TIM2;
	}
	else if (TIMx == TIM3)
	{
		ret 				= GPIO_AF_TIM3;
	}
	else if (TIMx == TIM4)
	{
		ret 				= GPIO_AF_TIM4;
	}
	else if (TIMx == TIM5)
	{
		ret 				= GPIO_AF_TIM5;
	}

	/* 没有 TIM6 TIM7 */
	else if (TIMx == TIM8)
	{
		ret 				= GPIO_AF_TIM8;
	}
	else if (TIMx == TIM9)
	{
		ret 				= GPIO_AF_TIM9;
	}
	else if (TIMx == TIM12)
	{
		ret 				= GPIO_AF_TIM12;
	}
	else if (TIMx == TIM13)
	{
		ret 				= GPIO_AF_TIM13;
	}
	else if (TIMx == TIM14)
	{
		ret 				= GPIO_AF_TIM14;
	}

	return ret;
}


void bsp_ConfigTimGpio(GPIO_TypeDef * GPIOx, uint16_t GPIO_Pinx, TIM_TypeDef * TIMx, uint8_t _ucChannel)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//使能GPIO时钟
	RCC_AHB1PeriphClockCmd(bsp_GetRccOfGPIOx(GPIO_Pinx), ENABLE);

	//使能TIM时钟
	if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10) || (TIMx == TIM11))
	{
		RCC_APB2PeriphClockCmd(bsp_GetRccOfTim(TIMx), ENABLE);
	}
	else 
	{
		RCC_APB1PeriphClockCmd(bsp_GetRccOfTim(TIMx), ENABLE);
	}
	//配置GPIO引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOx,&GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOx,bsp_GetPinSource(GPIO_Pinx),bsp_GetAfOfTim(TIMx));
}


