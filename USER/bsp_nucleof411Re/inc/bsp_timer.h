#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H
#include "bsp.h"

#define TMR_COUNT 4  //软件定时器个数

typedef enum
{
	TMR_ONCE_MODE = 0, //一次性工作模式
	TMR_AUTO_MODE = 1  //自动定时工作模式
}TMR_MODE_E;

//定时器结构体，成员必须是volatile，否则c编译器优化时可能有问题
typedef struct
{
	volatile uint8_t Mode;
	volatile uint8_t Flag;
	volatile uint32_t Count;
	volatile uint32_t PreLoad;
}SOFT_TMR;

//提供给其他文件调用的函数
void bsp_InitTimer(void);
void bsp_DelayMS(uint32_t n);
void bsp_DelayUS(uint32_t n);
void bsp_StartTimer(uint8_t _id, uint32_t _period);
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period);
void bsp_StopTimer(uint8_t _id);
uint8_t bsp_CheckTimer(uint8_t _id);
int32_t bsp_GetRunTime(void);
void My_SysTick_Handler(void);

#endif

