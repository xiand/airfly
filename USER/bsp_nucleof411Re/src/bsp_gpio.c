#include "bsp.h"


/**********************************************************************
* @Fuctionname   : None
* @Brief         : None
* @Input         : None
* @Output        : None
* @Return        : None
* @Remark        : None
**********************************************************************/
static uint32_t bsp_GetRccOfGPIOx(GPIO_TypeDef * GPIOx)
{
	uint32_t rcc = 0;

	if (GPIOx == GPIOA)
	{
		rcc = RCC_AHB1Periph_GPIOA;
	}
	else if (GPIOx == GPIOB)
	{
		rcc = RCC_AHB1Periph_GPIOB;
	}
	else if (GPIOx == GPIOC)
	{
		rcc = RCC_AHB1Periph_GPIOC;
	}
	else if (GPIOx == GPIOD)
	{
		rcc = RCC_AHB1Periph_GPIOD;
	}
	else if (GPIOx == GPIOE)
	{
		rcc = RCC_AHB1Periph_GPIOE;
	}
	else if (GPIOx == GPIOF)
	{
		rcc = RCC_AHB1Periph_GPIOF;
	}
	else if (GPIOx == GPIOG)
	{
		rcc = RCC_AHB1Periph_GPIOG;
	}
	else if (GPIOx == GPIOH)
	{
		rcc = RCC_AHB1Periph_GPIOH;
	}
	else if (GPIOx == GPIOI)
	{
		rcc = RCC_AHB1Periph_GPIOI;
	}

	return rcc;
}


/**********************************************************************
* @Fuctionname   : None
* @Brief         : None
* @Input         : None
* @Output        : None
* @Return        : None
* @Remark        : None
**********************************************************************/
static uint8_t bsp_GetExtiPortSource(GPIO_TypeDef *GPIOx)
{
	uint8_t ret = 0;
	if(GPIOx == GPIOA)
	{
		ret = EXTI_PortSourceGPIOA;
	}
	else if(GPIOx == GPIOB)
	{
		ret = EXTI_PortSourceGPIOB;
	}
	else if(GPIOx == GPIOC)
	{
		ret = EXTI_PortSourceGPIOC;
	}
	else if(GPIOx == GPIOD)
	{
		ret = EXTI_PortSourceGPIOD;
	}
	else if(GPIOx == GPIOE)
	{
		ret = EXTI_PortSourceGPIOE;
	}
	else if(GPIOx == GPIOF)
	{
		ret = EXTI_PortSourceGPIOF;
	}
	else if(GPIOx == GPIOG)
	{
		ret = EXTI_PortSourceGPIOG;
	}
	else if(GPIOx == GPIOH)
	{
		ret = EXTI_PortSourceGPIOH;
	}
	else if(GPIOx == GPIOK)
	{
		ret = EXTI_PortSourceGPIOK;
	}
	return ret;
	
}


/**********************************************************************
* @Fuctionname   : None
* @Brief         : None
* @Input         : None
* @Output        : None
* @Return        : None
* @Remark        : None
**********************************************************************/
static uint8_t bsp_GetExtiPinSource(uint16_t gpio_pinx)
{
	uint8_t ret = 0;
	while(gpio_pinx > 1)
	{
		gpio_pinx >>= 1;
		ret ++;
	}
	return ret;
}


/**********************************************************************
* @Fuctionname   : None
* @Brief         : None
* @Input         : None
* @Output        : None
* @Return        : None
* @Remark        : None
**********************************************************************/
static uint32_t bsp_GetExtiLine(uint16_t gpio_pinx)
{
	uint32_t ret = 1;
	uint8_t position = 0;
	while(gpio_pinx > 1)
	{
		gpio_pinx >>= 1;
		position ++;
	}
	ret = ret<<position;
	return ret;
}


/**********************************************************************
* @Fuctionname   : None
* @Brief         : None
* @Input         : None
* @Output        : None
* @Return        : None
* @Remark        : None
**********************************************************************/
void bsp_GpioSet(GPIO_TypeDef *GPIOx,uint16_t gpio_pinx,functionDefine_t function,interruptMode_t mode)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef GPIO_ExitStruct;
	RCC_AHB1PeriphClockCmd(bsp_GetRccOfGPIOx(GPIOx),ENABLE);
	if(function == function_output)
	{
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_Pin = gpio_pinx;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOx,&GPIO_InitStruct);
	}
	else if(function == function_input)
	{
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Pin = gpio_pinx;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_Init(GPIOx,&GPIO_InitStruct);
	}
	else if(function == function_interrupt)
	{
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Pin = gpio_pinx;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOx,&GPIO_InitStruct);
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
		SYSCFG_EXTILineConfig(bsp_GetExtiPortSource(GPIOx),bsp_GetExtiPinSource(gpio_pinx));
		GPIO_ExitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
		GPIO_ExitStruct.EXTI_Line = bsp_GetExtiLine(gpio_pinx);
		GPIO_ExitStruct.EXTI_Trigger = mode*4;
		GPIO_ExitStruct.EXTI_LineCmd = ENABLE;
		EXTI_Init(&GPIO_ExitStruct);
	}
}

/**********************************************************************
* @Fuctionname   : None
* @Brief         : None
* @Input         : None
* @Output        : None
* @Return        : None
* @Remark        : None
**********************************************************************/
static uint8_t bsp_GetNvicChannelOfPin(uint16_t gpio_pinx)
{
	uint8_t irqChannel;
	if(gpio_pinx == GPIO_Pin_0)
	{
		irqChannel = EXTI0_IRQn;
	}
	else if(gpio_pinx == GPIO_Pin_1)
	{
		irqChannel = EXTI1_IRQn;
	}
	else if(gpio_pinx == GPIO_Pin_2)
	{
		irqChannel = EXTI2_IRQn;
	}
	else if(gpio_pinx == GPIO_Pin_3)
	{
		irqChannel = EXTI3_IRQn; 
	}
	else if(gpio_pinx == GPIO_Pin_4)
	{
		irqChannel = EXTI4_IRQn;
	}
	else if((gpio_pinx == GPIO_Pin_5)||(gpio_pinx == GPIO_Pin_6)||(gpio_pinx == GPIO_Pin_7)||(gpio_pinx == GPIO_Pin_8)||(gpio_pinx == GPIO_Pin_9))
	{
		irqChannel = EXTI9_5_IRQn;
	}
	else
	{
		irqChannel = EXTI15_10_IRQn;
	}
	return irqChannel;
}
/**********************************************************************
* @Fuctionname   : None
* @Brief         : None
* @Input         : None
* @Output        : None
* @Return        : None
* @Remark        : None
**********************************************************************/
void bsp_GpioInterruptSet(uint16_t gpio_pinx,uint8_t ucPreemptionPriority, uint8_t ucSubPriority)
{
	NVIC_InitTypeDef NVIC_InitStruct;

	//ÖÐ¶ÏÅäÖÃ
	NVIC_InitStruct.NVIC_IRQChannel = bsp_GetNvicChannelOfPin(gpio_pinx);
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = ucPreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = ucSubPriority;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
}

/**********************************************************************
* @Fuctionname   : None
* @Brief         : None
* @Input         : None
* @Output        : None
* @Return        : None
* @Remark        : None
**********************************************************************/
uint8_t bsp_GetPinStatus(GPIO_TypeDef *GPIOx, uint16_t gpio_pinx,functionDefine_t function)
{
	uint8_t status = 0;
	if((function == function_input)||(function == function_interrupt))
	{
		status = GPIO_ReadInputDataBit(GPIOx,gpio_pinx);
	}
	else if(function == function_output)
	{
		status = GPIO_ReadOutputDataBit(GPIOx,gpio_pinx);
	}
	return status;
}


