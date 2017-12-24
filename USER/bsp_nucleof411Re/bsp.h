#ifndef _BSP_H_
#define _BSP_H_


#define USE_FREERTOS 0

#if USE_FREERTOS == 1
#include "FreeRTOS.h"
#include "task.h"
#define DISABLE_INT()	 taskENTER_CRITICAL()
#define ENABLE_INT()	 taskEXIT_CRITICAL()
#else
/* 开关全局中断宏*/
#define ENABLE_INT() __set_PRIMASK(0) //使能全局中断
#define DISABLE_INT() __set_PRIMASK(1)  //禁止全局中断
#endif


/* 该宏用于调试阶段排错*/
#define BSP_Printf  printf

#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#include "bsp_led.h"
#include "bsp_timer.h"
#include "bsp_key.h"
#include "bsp_usart_fifo.h"
#include "bsp_DwtDelay.h"

#include "bsp_myDs18b20.h"
#include "bsp_OledShow.h"
//#include "bsp_spiSD.h"
#include "bsp_rtc.h"
#include "stm32_eval_spi_sd.h"
#include "diskio.h"

void bsp_Init(void);
void bsp_Idle(void);

#endif

