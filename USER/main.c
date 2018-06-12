#if 0
#include "includes.h"
/*
**********************************************************************************************************
											oˉêyéù?÷
**********************************************************************************************************
*/
static void vTaskUserIF(void *pvParameters);
static void vTaskLED(void *pvParameters);
static void vTaskStart(void *pvParameters);
static void AppTaskCreate (void);
static void AppObjCreate (void);
void  App_Printf(char *format, ...);

///*
//**********************************************************************************************************
//											±?á?éù?÷
//**********************************************************************************************************
//*/
static TaskHandle_t xHandleTaskUserIF = NULL;
static TaskHandle_t xHandleTaskLED = NULL;
static TaskHandle_t xHandleTaskStart = NULL;
static SemaphoreHandle_t  xMutex = NULL;

int main(void)
{
	/**********************************************************
	在启动调度前，为了防止初始化STM32外设时有中断服务程序执行，
	在这里禁止全局中断(除了NMI和HardFault)。
	1、防止执行的中断服务程序中有FreeRTOS的API函数
	2、保证系统正常启动，不受别的中断影响
	3、关于是否关闭全局中断，视情况而定
	在移植文件port.c中的函数prvStartFirstTask中会重新开启全局中断，使用指令
	cpsie i开启，__set_PRIMASK(1)和cpsie i是等效的
	**********************************************************/
	__set_PRIMASK(1);

//	//硬件初始化
	bsp_Init();

	vSetupSysInfoTest();
	
//	//创建任务
	AppTaskCreate();

//	//启动任务调度，开始执行任务
	vTaskStartScheduler();

	/*
		如果系统正常启动是不会运行到这里的，运行到这里极有可能是因为定时器任务
		或者空闲任务的heap空间不足导致创建失败，此时需要加大FreeRTOSConfig.h
		文件中定于的heap大小
	*/
	while(1);
}

static void vTaskUserIF(void *pvParametres)
{
	uint8_t ucKeyCode;
	uint8_t pcWriteBuffer[500];

	while(1)
	{
		ucKeyCode = bsp_GetKey();

		if(ucKeyCode != KEY_NONE)
		{
			switch (ucKeyCode)
			{
				case KEY_1_SHORTPRESS:
					printf("=======================================\r\n");
					printf("任务名   状态    优先级  剩余栈   任务序列\r\n");
					vTaskList((char *)&pcWriteBuffer);
					printf("%s\r\n",pcWriteBuffer);

					printf("\r\n任务名   运行计算   使用率\r\n");
					vTaskGetRunTimeStats((char *)&pcWriteBuffer);
					printf("%s\r\n",pcWriteBuffer);
					break;
					
				default :
					break;
			}
		}
	}
}

static void vTaskLED(void *pvParameters)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1000;

	//获取当前系统时间
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		LED_TOGGLE();

		//vTaskDelayUntil为绝对延迟
		vTaskDelayUntil(&xLastWakeTime,xFrequency);
	}
}

static void vTaskStart(void *pvParametres)
{
	while(1)
	{
		//按键扫描
		bsp_KeyScan();
		//printf("hello world\r\n");
		vTaskDelay(10);
	}
}

static void AppTaskCreate(void)
{
	xTaskCreate(vTaskUserIF,
				"vTaskUserIF",
				512,
				NULL,
				1,
				&xHandleTaskUserIF);

	xTaskCreate(vTaskLED,
				"vTaskLED",
				512,
				NULL,
				2,
				&xHandleTaskLED);
	
	xTaskCreate(vTaskStart,
				"vTaskStart",
				512,
				NULL,
				4,
				&xHandleTaskStart);
				
}


static void AppObjCreate(void)
{
	//创建互斥信号量
	xMutex = xSemaphoreCreateMutex();

	if(xMutex == NULL)
	{
		//创建失败，添加失败机制
	}
}

/*
***********************************************
* 函数名：App_Printf
* 功能说明：线程安全的printf方式
***********************************************
*/
void App_Printf(char *format,  ...)
{
	char buf_str[200+1];
	va_list v_args;

	va_start(v_args,format);
	(void)vsnprintf((char *)&buf_str[0],
					(size_t)sizeof(buf_str),
					(char const *)format,
								  v_args);
	va_end(v_args);
	//互斥信号量
	xSemaphoreTake(xMutex,portMAX_DELAY);

	printf("%s",buf_str);

	xSemaphoreGive(xMutex);
		
}
#endif


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


