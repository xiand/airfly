
#include "bsp.h"


void bsp_Init(void)
{
	//���ȼ���������Ϊ4,������0-15����ռ���ȼ��������������ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//bsp_InitKey();

#if USE_FREERTOS					== 0
	//bsp_InitTimer();
#endif

	//bsp_InitUart();

	//bsp_myInitDs18b20();							
	//bsp_dwtInit();
	//bsp_RtcInit();
	//bsp_InitLed();
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


