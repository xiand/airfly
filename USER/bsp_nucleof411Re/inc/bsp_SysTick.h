#ifndef _BSP_SYSTICK_H_
#define _BSP_SYSTICK_H_

#include "stm32f4xx.h"
void Delay_ms(__IO u32 nTime);
void SysTick_Init(void);

#endif
