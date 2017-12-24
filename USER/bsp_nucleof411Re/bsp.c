#include "bsp.h"

void bsp_Init(void)
{
	uint8_t i = 0;
	bsp_InitKey();
	bsp_InitTimer();
	bsp_InitUart();
	//OLED_Init();
	i = SD_Init();
	printf("the ret = %d\r\n",i);
	//bsp_myInitDs18b20();							
	//bsp_dwtInit();
	//bsp_RtcInit();
	bsp_InitLed();
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


