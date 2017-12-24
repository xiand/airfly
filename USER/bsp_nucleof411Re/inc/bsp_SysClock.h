#ifndef __BSP_SYSCLOCK_H_
#define __BSP_SYSCLOCK_H_

#include "stm32f4xx.h"

void HSI_SetSysClock(uint32_t M,uint32_t N,uint32_t p,uint32_t q);

void HSE_SetSysClock(uint32_t m, uint32_t n, uint32_t p, uint32_t q);
#endif

