
#include "bsp.h"
#include "ff.h"
extern void DemoFatFS();
SD_CSD  stSDcsd = {0};
SD_CID 	stSDCid = {0};
SD_CardInfo stSDCardInfo = {0};
uint8_t *g_ucTestData = "asdfdsasdfffffffasdfdasdfdasdfaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaddddddddddddddddddddddddddddddddddd";
uint8_t  g_ucTestBuff[512] = {0};
uint32_t g_SDCapacity = 0; 


void mkfs(){
	FATFS fs;
	FIL fil;            /* File object */
	FRESULT res;        /* API result code */
	u8 bw;            /* Bytes written */
	uint32_t ulData;
	u8 work[_MAX_SS]; /* Work area (larger is better for process time) */
	u8 readByte[15];

	bw=f_mkfs("0",FM_FAT|FM_SFD,512,work,sizeof work);
	printf("%d\r\n",bw);
	res=f_mount (&fs,"0",0);
	printf("the f_mount = %d\r\n",res);
	res=f_open(&fil, "hello.txt", FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
	printf("the f_open = %d\r\n",res);
	res=f_write(&fil,  "hello world, it is for my test function",  sizeof( "hello world, it is for my test function")-1, &ulData);
	printf("the f_write = %d\r\n",res);
	res=f_close(&fil);
}

int main(void)
{
	uint8_t 		sysclock = 0;
	uint8_t 		ucKeyCode = 0;
	
	RCC_ClocksTypeDef clock_info;

	sysclock			= RCC_GetSYSCLKSource();
	RCC_GetClocksFreq(&clock_info);
	bsp_Init();
	
//	bsp_StartAutoTimer(0, 500);
//	SD_Init();

//	SD_GetCardInfo(&stSDCardInfo);
//	g_SDCapacity =  SD_GetCapacity();

//	SD_WriteSingleBlock(1, g_ucTestData, 30);
//	sd_ReadSingleBlock(g_ucTestBuff,1,30);
//	disk_initialize(0);
//	disk_write (0, g_ucTestData,12000, 1);
//	disk_read (0, g_ucTestBuff, 12000, 1);
//	result = f_mount(&fs, "0:", 0);	
//	if (result != FR_OK)
//	{
//		printf("�����ļ�ϵͳʧ�� (%d)\r\n", result);
//	}
//	else
//	{
//		printf("�����ļ�ϵͳ�ɹ� (%d)\r\n",result);
//	}
//	
//	/* ��һ��ʹ�ñ�����и�ʽ�� */
//	result = f_mkfs("0:",0,0);
//	if (result != FR_OK)
//	{
//		printf("��ʽ��ʧ�� (%d)\r\n", result);
//	}
//	else
//	{
//		printf("��ʽ���ɹ� (%d)\r\n", result);
//	}

//	/* ж���ļ�ϵͳ */
//	result  = f_mount(NULL, "0:", 0);
//	if (result != FR_OK)
//	{
//		printf("ж���ļ�ϵͳʧ�� (%d)\r\n", result);
//	}
//	else
//	{
//		printf("ж���ļ�ϵͳ�ɹ� (%d)\r\n", result);
//	}
//	
	mkfs();
//	DemoFatFS();
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


