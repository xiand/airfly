
#include "bsp.h"


void bsp_Init(void)
{
	//���ȼ���������Ϊ4,������0-15����ռ���ȼ��������������ȼ�
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


