//���һ���������Ŀ
//1���ڴ濨����-��¼�ϲ�Ĳ���
//2��Զ�̽����Ͽ��������-led����ʾ
//3����ҪΪ����Ϥfreertos---�����Ķ�Դ����͹ٷ��ĵ�

#include "bsp.h"

int main(void)
{
	bsp_Init();
	
}




#if 0
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

	SystemCoreClockUpdate();
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
#endif

