#ifndef _BSP_LED_H_
#define _BSP_LED_H_

#include "stm32f4xx.h"

#define ON 1
#define OFF 0

#define BSP_LED_PIN GPIO_Pin_13
#define BSP_LED_PORT GPIOA

#define LED(a) if(a)\
	GPIO_SetBits(GPIOA,GPIO_Pin_5);\
						else \
						GPIO_ResetBits(GPIOA,GPIO_Pin_5)
						
#define LED_TOGGLE() GPIO_ToggleBits(GPIOA,GPIO_Pin_5)
						
void bsp_InitLed(void);

#endif
