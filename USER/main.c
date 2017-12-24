
#include "bsp.h"

int main(void)
{
	uint8_t 		sysclock = 0;
	uint8_t 		ucKeyCode = 0;
	uint8_t 		id[32] =
	{
		0
	};
	uint8_t 		i;
	int16_t 		reg = 0;
	uint32_t		cap = 0;

	RCC_ClocksTypeDef clock_info;

	sysclock			= RCC_GetSYSCLKSource();
	RCC_GetClocksFreq(&clock_info);
	bsp_Init();
	bsp_StartAutoTimer(0, 500);
	DemoFatFS();
#if 0
	while (1)
	{
		if (bsp_CheckTimer(0))
		{
			LED_TOGGLE();
		}

		ucKeyCode			= bsp_GetKey();

		if (ucKeyCode != KEY_NONE)
		{
			switch (ucKeyCode)
			{
				case KEY_1_SHORTPRESS:
					printf("按键短按\r\n");
					sd_GetCID(id);

					for (i = 0; i < 16; i++)
					{
						printf("%x ", id[i]);
					}

					printf("\r\n");
					cap = SD_GetCapacity();
					printf("the SD_GetCapacity = %X\r\n", cap);
					break;

				case KEY_1_LONGPRESS:
					printf("按键长按\r\n");
					printf("sysclock = %x\r\n", sysclock);
					printf("clock_info.HCLK_Frequency = %d,PCLK1_Frequency = %d,PCLK2_Frequency = %d, SYSCLK_Frequency = %d\r\n",
						 
						clock_info.HCLK_Frequency, clock_info.PCLK1_Frequency, clock_info.PCLK2_Frequency, 
						clock_info.SYSCLK_Frequency);
					break;

				case KEY_1_DOUBLEPRESS:
					printf("按键双击\r\n");
					break;

				default:
					printf("按键动作未定义\r\n");
					break;
			}
		}
	}
#endif
}


