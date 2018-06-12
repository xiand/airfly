#if 0
#include "includes.h"
/*
**********************************************************************************************************
											o����y����?��
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
//											��?��?����?��
//**********************************************************************************************************
//*/
static TaskHandle_t xHandleTaskUserIF = NULL;
static TaskHandle_t xHandleTaskLED = NULL;
static TaskHandle_t xHandleTaskStart = NULL;
static SemaphoreHandle_t  xMutex = NULL;

int main(void)
{
	/**********************************************************
	����������ǰ��Ϊ�˷�ֹ��ʼ��STM32����ʱ���жϷ������ִ�У�
	�������ֹȫ���ж�(����NMI��HardFault)��
	1����ִֹ�е��жϷ����������FreeRTOS��API����
	2����֤ϵͳ�������������ܱ���ж�Ӱ��
	3�������Ƿ�ر�ȫ���жϣ����������
	����ֲ�ļ�port.c�еĺ���prvStartFirstTask�л����¿���ȫ���жϣ�ʹ��ָ��
	cpsie i������__set_PRIMASK(1)��cpsie i�ǵ�Ч��
	**********************************************************/
	__set_PRIMASK(1);

//	//Ӳ����ʼ��
	bsp_Init();

	vSetupSysInfoTest();
	
//	//��������
	AppTaskCreate();

//	//����������ȣ���ʼִ������
	vTaskStartScheduler();

	/*
		���ϵͳ���������ǲ������е�����ģ����е����Ｋ�п�������Ϊ��ʱ������
		���߿��������heap�ռ䲻�㵼�´���ʧ�ܣ���ʱ��Ҫ�Ӵ�FreeRTOSConfig.h
		�ļ��ж��ڵ�heap��С
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
					printf("������   ״̬    ���ȼ�  ʣ��ջ   ��������\r\n");
					vTaskList((char *)&pcWriteBuffer);
					printf("%s\r\n",pcWriteBuffer);

					printf("\r\n������   ���м���   ʹ����\r\n");
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

	//��ȡ��ǰϵͳʱ��
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		LED_TOGGLE();

		//vTaskDelayUntilΪ�����ӳ�
		vTaskDelayUntil(&xLastWakeTime,xFrequency);
	}
}

static void vTaskStart(void *pvParametres)
{
	while(1)
	{
		//����ɨ��
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
	//���������ź���
	xMutex = xSemaphoreCreateMutex();

	if(xMutex == NULL)
	{
		//����ʧ�ܣ����ʧ�ܻ���
	}
}

/*
***********************************************
* ��������App_Printf
* ����˵�����̰߳�ȫ��printf��ʽ
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
	//�����ź���
	xSemaphoreTake(xMutex,portMAX_DELAY);

	printf("%s",buf_str);

	xSemaphoreGive(xMutex);
		
}
#endif


 //δ��FreeRTOSʱ��Ĳ�������
#include "bsp.h"
#include "ff.h"			/* FatFS�ļ�ϵͳģ��*/
uint8_t  g_ucTestWriteBuff[512] = "rrrdfdsasdfffffffasdfdasdfdasdfaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaddddddddddddddddddddddddddddddddddd";
uint8_t  g_ucTestReadBuff[512] = {0};


/*
*********************************************************************************************************
*	�� �� ��: ReadFileData
*	����˵��: ��ȡ�ļ�armfly.txtǰ128���ַ�������ӡ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
 void ReadFileData(void)
{
	/* ������ʹ�õľֲ�����ռ�ý϶࣬���޸������ļ�����֤��ջ�ռ乻�� */
	FRESULT result;
	FATFS fs;
	FIL file;
	UINT brw;
	char ucWriteBuff[128] = "aaaaaaaaaaaaabbbbbbbbbbbbbbbbbbcccccccsadfhajksdjfhjkldsafhd";
	char ucReadBuff[128] = {0};
	
 	/* �����ļ�ϵͳ */
	result = f_mount(&fs,"0:", 1);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		//printf("�����ļ�ϵͳʧ��(%d)\r\n", result);
	}

	/* �򿪸��ļ��� */
	result = f_open(&file,"he.txt", FA_OPEN_ALWAYS|FA_WRITE|FA_READ); /* ���������������ӵ�ǰĿ¼��ʼ */
	if (result != FR_OK)
	{
		//printf("�򿪸�Ŀ¼ʧ��(%d)\r\n", result);
		return;
	}
	else
	{
		result = f_write(&file,ucWriteBuff,sizeof(ucWriteBuff),&brw);
		if(result != FR_OK)
		{
			//printf("�ļ�дʧ��\r\n");
			return;
		}
		f_close(&file);
		/* �����ļ�ϵͳ */
		//result = f_mount(&fs,"0:", 1);			/* Mount a logical drive */
		//��ȡ�ļ�����
		result = f_open(&file,"he.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
	
		if(result != FR_OK)
		{
			//printf("�ļ���ʧ��\r\n");
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
	
	//sd���ײ������Ķ�д��������
//	ret = SD_Init();
//	SD_WriteBlock(g_ucTestWriteBuff,0xff,512);
//	//�ȴ�д�������
//	SD_WaitWriteOperation();
//	SD_ReadBlock(g_ucTestReadBuff, 0xff, 512);
//	//�ȴ����������
//	SD_WaitReadOperation();
//	SD_Erase(0xff,0xff+0xff);
//	
//	SD_ReadBlock(g_ucTestReadBuff, 0xff, 512);
//	//�ȴ����������
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
					printf("�����̰�\r\n");
					break;

				case KEY_1_LONGPRESS:
					printf("��������\r\n");
					printf("sysclock = %x\r\n", sysclock);
					printf("clock_info.HCLK_Frequency = %d,PCLK1_Frequency = %d,PCLK2_Frequency = %d, SYSCLK_Frequency = %d\r\n",
						 
						clock_info.HCLK_Frequency, clock_info.PCLK1_Frequency, clock_info.PCLK2_Frequency, 
						clock_info.SYSCLK_Frequency);
					break;

				case KEY_1_DOUBLEPRESS:
					printf("����˫��\r\n");
					break;

				default:
					printf("��������δ����\r\n");
					break;
			}
		}
	}

}


