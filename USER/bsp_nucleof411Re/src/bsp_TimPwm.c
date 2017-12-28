
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

	//连接到AF功能
	GPIO_PinAFConfig(GPIOx,bsp_GetPinSource(GPIO_Pinx),bsp_GetAfOfTim(TIMx));
}

//配置PWM GPIO引脚
void bsp_ConfigGpioOut(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pinx)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//使能GPIO时钟
	RCC_AHB1PeriphClockCmd(GPIOx,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //设置为推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //无上拉下拉
	GPIO_Init(GPIOx,&GPIO_InitStructure);
}

void bsp_SetTimOutPWM(GPIO_TypeDef *GPIOx,uint16_t gpio_pinx,TIM_TypeDef *TIMx,uint8_t _ucChannel,uint32_t _uiFreq,uint32_t _uiDutyCycle)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	uint16_t usPeriod;
	uint16_t usPrescaler;
	uint32_t uiTIMxCLK;

	if(_uiDutyCycle == 0) //占空比等于0
	{
		TIM_Cmd(TIMx,DISABLE);
		bsp_ConfigGpioOut(GPIOx,gpio_pinx);
		GPIO_WriteBit(GPIOx,gpio_pinx,Bit_RESET);
	} 
	else if(_uiDutyCycle == 10000) //占空比为100%, 这里为什么设置的是10000而不是100
	{
		TIM_Cmd(TIMx,DISABLE);
	}

	bsp_ConfigTimGpio(GPIOx,gpio_pinx,TIMx,_ucChannel);
	/*-----------------------------------------------------------------------
		system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)
		
		上述是针对于目前市面上的STM32F407系列开发板，主频在168MHz
		而目前自己使用的是 STM32F411RE 主频在100MHz 时钟配置为：
		HCLK = SYSCLK / 1 (AHB1Periph)
		PCLK2 = HCLK      (APB2Periph)
		PCLK1 = HCLK / 2  (APB1Periph)

		
		因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock; 该条件对于STM32F411不成立 STM32F411应该对于所有的定时器全部是SystemCoreClock时钟频率
		因为APB2 prescaler == 1, 所以 APB2上的TIMxCLK = PCLK2 = SystemCoreClock;
		APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM6, TIM12, TIM13,TIM14
		APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
		由上述及数据手册上可得定时器内部时钟频率 SystemCoreClock
	----------------------------------------------------------------------- */
	if((TIMx == TIM1)||(TIMx == TIM8)||(TIMx == TIM9)||(TIMx == TIM10)||(TIMx == TIM11))
	{
		//对于在APB2总线上的定时器
		uiTIMxCLK = SystemCoreClock;
	}
	else
	{
		//对于在APB1总线上的定时器
		uiTIMxCLK = SystemCoreClock;
	}

	/*
		如果不使用定时器的预分频器，定时器的可允许的最小的频率为
		100 000 000 / 65535 = 1525Hz ，因为自动重载值最大为0xFFFF
		如果频率要小于该值，则需要使用预分频器进行分频处理
		下面的计算为粗略计算
	*/
	if(_uiFreq < 100)
	{
		usPrescaler = 10000 - 1;
		usPeriod = (uiTIMxCLK/10000)/_uiFreq - 1;  //自动重装值
	}
	else if(_uiFreq < 3000)
	{
		usPrescaler = 100 - 1;
		usPeriod = (uiTIMxCLK/100)/_uiFreq - 1;
	}
	else 
	{
		usPrescaler = 0;
		usPeriod = (uiTIMxCLK)/_uiFreq - 1;  //自动重装值
	}

	//Time Base Configuration
	TIM_TimeBaseStructure.TIM_Period = usPeriod;
	TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;  //针对于高级定时器TIM1和TIM8必须初始化该位

	TIM_TimeBaseInit(TIMx,&TIM_TimeBaseStructure);

	//PWM1 Mode configuration 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = (_uiDutyCycle * usPeriod)/10000;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; //仅用于TIM1和TIM8
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;		//仅用于TIM1和TIM8
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;	//仅用于TIM1和TIM8
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;	//仅用于TIM1和TIM8

	if(_ucChannel == 1)
	{
		TIM_OC1Init(TIMx,&TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIMx,TIM_OCPreload_Enable);
	}
	else if(_ucChannel == 2)
	{
		TIM_OC2Init(TIMx,&TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIMx,TIM_OCPreload_Enable);
	}
	else if(_ucChannel == 3)
	{
		TIM_OC3Init(TIMx,&TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIMx,TIM_OCPreload_Enable);
	}
	else if(_ucChannel == 4)
	{
		TIM_OC4Init(TIMx,&TIM_OCInitStructure);
		TIM_OC4PreloadConfig(TIMx,TIM_OCPreload_Enable);
	}

	TIM_ARRPreloadConfig(TIMx,ENABLE);

	TIM_Cmd(TIMx,ENABLE);

	if((TIMx == TIM1)||(TIMx == TIM8))
	{
		TIM_CtrlPWMOutputs(TIMx,ENABLE);
	}
}

void bsp_SetTimForInt(TIM_TypeDef* TIMx, uint32_t _ulFreq, uint8_t _ucPreemptionPriority, uint8_t _ucSubPrority)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	uint16_t usPeriod;
	uint16_t usPrescaler;
	uint32_t uiTIMxCLK;
	NVIC_InitTypeDef NVIC_InitStructure;
	uint8_t irq = 0;  //中断编号，定义在 stm32f4xx.h
	
	if((TIMx == TIM1)||(TIMx == TIM8)||(TIMx == TIM9)||(TIMx == TIM10)||(TIMx == TIM11))
	{
		RCC_APB2PeriphClockCmd(bsp_GetRccOfTim(TIMx),ENABLE);
	}
	else
	{
		RCC_APB1PeriphClockCmd(bsp_GetRccOfTim(TIMx),ENABLE);
	}

	if(_ulFreq == 0)
	{
		TIM_Cmd(TIMx,DISABLE);
		return;
	}
	
	/*-----------------------------------------------------------------------
			system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：
	
			HCLK = SYSCLK / 1	  (AHB1Periph)
			PCLK2 = HCLK / 2	  (APB2Periph)
			PCLK1 = HCLK / 4	  (APB1Periph)
			
			上述是针对于目前市面上的STM32F407系列开发板，主频在168MHz
			而目前自己使用的是 STM32F411RE 主频在100MHz 时钟配置为：
			HCLK = SYSCLK / 1 (AHB1Periph)
			PCLK2 = HCLK	  (APB2Periph)
			PCLK1 = HCLK / 2  (APB1Periph)
	
			
			因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock; 该条件对于STM32F411不成立 STM32F411应该对于所有的定时器全部是SystemCoreClock时钟频率
			因为APB2 prescaler == 1, 所以 APB2上的TIMxCLK = PCLK2 = SystemCoreClock;
			APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM6, TIM12, TIM13,TIM14
			APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
			由上述及数据手册上可得定时器内部时钟频率 SystemCoreClock
	----------------------------------------------------------------------- */
	if((TIMx == TIM1)||(TIMx == TIM8)||(TIMx == TIM9)||(TIMx == TIM10)||(TIMx == TIM11))
	{
		//对于在APB2总线上的定时器
		uiTIMxCLK = SystemCoreClock;
	}
	else
	{
		//对于在APB1总线上的定时器
		uiTIMxCLK = SystemCoreClock;
	}

	/*
		如果不使用定时器的预分频器，定时器的可允许的最小的频率为
		100 000 000 / 65535 = 1525Hz ，因为自动重载值最大为0xFFFF
		如果频率要小于该值，则需要使用预分频器进行分频处理
		下面的计算为粗略计算
	*/
	if(_ulFreq < 100)
	{
		usPrescaler = 10000 - 1;
		usPeriod = (uiTIMxCLK/10000)/_ulFreq - 1;  //自动重装值
	}
	else if(_uiFreq < 3000)
	{
		usPrescaler = 100 - 1;
		usPeriod = (uiTIMxCLK/100)/_ulFreq - 1;
	}
	else 
	{
		usPrescaler = 0;
		usPeriod = (uiTIMxCLK)/_ulFreq - 1;  //自动重装值
	}

	TIM_TimeBaseStructure.TIM_Period = usPeriod;
	TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x00;

	TIM_TimeBaseInit(TIMx,&TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIMx,ENABLE);

	//使能中断
	TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIMx,ENABLE);

	if((TIMx == TIM1)||(TIMx == TIM10))
		irq = TIM1_UP_TIM10_IRQn;
	else if(TIMx == TIM2)
		irq = TIM2_IRQn;
	else if(TIMx == TIM3)
		irq = TIM3_IRQn;
	else if(TIMx == TIM4)
		irq = TIM4_IRQn;
	else if(TIMx == TIM5)
		irq = TIM5_IRQn;
	else if(TIMx == TIM6)
		irq = TIM6_IRQn;
	else if(TIMx == TIM7)
		irq = TIM7_IRQn;
	else if((TIMx == TIM8)||(TIMx == TIM13))
		irq = TIM8_UP_TIM13_IRQn;
	else if(TIMx == TIM9)
		irq = TIM1_BRK_TIM9_IRQn;
	else if(TIMx == TIM11)
		irq = TIM1_TRG_COM_TIM11_IRQn;
	else if(TIMx == TIM12)
		irq = TIM8_BRK_TIM12_IRQn;
	else if(TIMx == TIM12)
		irq = TIM8_TRG_COM_TIM14_IRQn;

	NVIC_InitStructure.NVIC_IRQChannel = irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = _ucPreemptionPriority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = _ucSubPrority;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}

