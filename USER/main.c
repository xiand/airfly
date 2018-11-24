//完成一个好玩的项目
//1、内存卡驱动-记录上层的操作
//2、远程界面上控制数码管-led灯显示
//3、主要为了熟悉freertos---不断阅读源代码和官方文档

#include "bsp.h"

int main(void)
{
	bsp_Init();
	
}




#if 0
 //未加FreeRTOS时候的测试历程
#include "bsp.h"
#include "ff.h"			/* FatFS文件系统模块*/
uint8_t  g_ucTestWriteBuff[512] = "rrrdfdsasdfffffffasdfdasdfdasdfaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaddddddddddddddddddddddddddddddddddd";
uint8_t  g_ucTestReadBuff[512] = {0};



/*
*********************************************************************************************************
*	函 数 名: ReadFileData
*	功能说明: 读取文件armfly.txt前128个字符，并打印到串口
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
 void ReadFileData(void)
{
	/* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */
	FRESULT result;
	FATFS fs;
	FIL file;
	UINT brw;
	char ucWriteBuff[128] = "aaaaaaaaaaaaabbbbbbbbbbbbbbbbbbcccccccsadfhajksdjfhjkldsafhd";
	char ucReadBuff[128] = {0};
	
 	/* 挂载文件系统 */
	result = f_mount(&fs,"0:", 1);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		//printf("挂载文件系统失败(%d)\r\n", result);
	}

	/* 打开根文件夹 */
	result = f_open(&file,"he.txt", FA_OPEN_ALWAYS|FA_WRITE|FA_READ); /* 如果不带参数，则从当前目录开始 */
	if (result != FR_OK)
	{
		//printf("打开根目录失败(%d)\r\n", result);
		return;
	}
	else
	{
		result = f_write(&file,ucWriteBuff,sizeof(ucWriteBuff),&brw);
		if(result != FR_OK)
		{
			//printf("文件写失败\r\n");
			return;
		}
		f_close(&file);
		/* 挂载文件系统 */
		//result = f_mount(&fs,"0:", 1);			/* Mount a logical drive */
		//读取文件内容
		result = f_open(&file,"he.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
	
		if(result != FR_OK)
		{
			//printf("文件打开失败\r\n");
			return;
		}
		result = f_read(&file,ucReadBuff,sizeof(ucReadBuff),&brw);
		f_close(&file);
	}
	
}

int main(void)
{
	uint32_t        i  = 0;
	uint8_t 		sysclock = 0;
	uint8_t 		ucKeyCode = 0;
	uint8_t         ret = 0;

	SystemCoreClockUpdate();
	RCC_ClocksTypeDef clock_info;
	sysclock			= RCC_GetSYSCLKSource();
	RCC_GetClocksFreq(&clock_info);
	bsp_Init();
	
	//sd卡底层驱动的读写操作测试
//	ret = SD_Init();
//	SD_WriteBlock(g_ucTestWriteBuff,0xff,512);
//	//等待写操作完成
//	SD_WaitWriteOperation();
//	SD_ReadBlock(g_ucTestReadBuff, 0xff, 512);
//	//等待读操作完成
//	SD_WaitReadOperation();
//	SD_Erase(0xff,0xff+0xff);
//	
//	SD_ReadBlock(g_ucTestReadBuff, 0xff, 512);
//	//等待读操作完成
//	SD_WaitReadOperation();
//	while(1);
	ReadFileData();
	while(1);
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

}
#endif

