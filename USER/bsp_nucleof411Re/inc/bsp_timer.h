#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H
#include "bsp.h"

#define TMR_COUNT 4  //�����ʱ������

typedef enum
{
	TMR_ONCE_MODE = 0, //һ���Թ���ģʽ
	TMR_AUTO_MODE = 1  //�Զ���ʱ����ģʽ
}TMR_MODE_E;

//��ʱ���ṹ�壬��Ա������volatile������c�������Ż�ʱ����������
typedef struct
{
	volatile uint8_t Mode;
	volatile uint8_t Flag;
	volatile uint32_t Count;
	volatile uint32_t PreLoad;
}SOFT_TMR;

//�ṩ�������ļ����õĺ���
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

