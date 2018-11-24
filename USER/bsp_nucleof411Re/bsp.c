
#include "bsp.h"


void bsp_Init(void)
{
	//优先级分组设置为4,可配置0-15级抢占优先级，不存在子优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
}


void bsp_RunPer10ms(void)
{
	bsp_KeyScan();
}


void bsp_RunPer1ms(void)
{

}


void bsp_Idle(void)
{

}


