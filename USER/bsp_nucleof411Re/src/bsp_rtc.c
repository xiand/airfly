
#include "bsp.h"

#define RTC_Debug //���ڵ���ģʽ��

#define RTC_CLOCK_SOURCE_LSE //ʹ���ⲿʱ��,Nucleon�����ⲿ��ֹ

//#define RTC_CLOCK_SOURCE_LSI	 //ʹ���ڲ�ʱ��
RTC_TimeTypeDef RTC_TimeStructure;
RTC_InitTypeDef RTC_InitStructure;
RTC_AlarmTypeDef RTC_AlarmStructure;
RTC_DateTypeDef RTC_DateStructure;

__IO uint32_t	uwAsynchPrediv = 0;
__IO uint32_t	uwSynchPrediv = 0;

//����PC13�������������
//���ο���ѡ��1hz����512hz
static void RTC_Signal_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//ʹ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	RTC_CalibOutputConfig(RTC_OUTPUT_FRQ);
	RTC_CalibOutputCmd(ENABLE);
}


//�ж�����
//���� AlarmA�ж�
//����wakeup�ж�
static void RTC_Int_Configure(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	//����RTC�����ж�����
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	//ʹ��RTC�����ж�
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;

	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	//����RTC Wakeup�ж�����
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	//ʹ��RTC�����ж�
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;

	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}


//1������ʱ��
//2��rtc����ʱ������
//3��alarmA�����ж�����
//4��wakeup�ж�����
//5��ʱ�����512hz����1hz
static void RTC_Config(void)
{
	//ʹ��PWRʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	//�������RTC
	PWR_BackupAccessCmd(ENABLE);

	//ѡ��ʱ��Դ
#if defined(RTC_CLOCK_SOURCE_LSI)	

	//ʹ�� LSI
	RCC_LSICmd(ENABLE);

	//�ȴ�LSI�ȶ�
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
	}

	//ѡ��LSI��ΪRTCʱ����Դ
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

#elif defined(RTC_CLOCK_SOURCE_LSE) 
	RCC_LSEConfig(RCC_LSE_ON);

	//�ȴ�LSEʱ�Ӿ���
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}

	//ѡ��RTCʱ����Դ
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

#else

#error please						select the RTC Clock source 
#endif

	/* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
	//17.3.1  Clock and prescalers	��ϸ�ο�
	uwSynchPrediv		= 0xFF;
	uwAsynchPrediv		= 0x7F;

	//ʹ��RTCʱ��
	RCC_RTCCLKCmd(ENABLE);

	//�ȴ�RTC APB�Ĵ���ͬ��
	RTC_WaitForSynchro();

	//����RTCʱ���ʽ�ͷ�Ƶ��
	RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
	RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);					//Init����ֻ�������������

	//����ʱ�䣺�ꡢ�¡��ռ�����
	RTC_DateStructure.RTC_Year = CURRENT_YEAR;
	RTC_DateStructure.RTC_Month = CURRENT_MONTH;
	RTC_DateStructure.RTC_Date = CURRENT_DAY;
	RTC_DateStructure.RTC_WeekDay = CURRENT_WEEKDAY;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

	//����ʱ�䣺���ӡ��롢����ʾ��ʽ
	RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours = CURRENT_HOUR;
	RTC_TimeStructure.RTC_Minutes = CURRENT_MIN;
	RTC_TimeStructure.RTC_Seconds = CURRENT_SEC;
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);

	//���ñ��ݼĴ�������ʾ�Ѿ���ɻ�������
	RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);

	//�������ӣ�BCD��ʽ
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = ALARM_HOUR;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = ALARM_MIN;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = ALARM_SEC;
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

	//����RTC Alarm A�Ĵ���
	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);

	//ʹ��RTC Alarm A�ж�
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	//ʹ������
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

	//���RTC���ӱ�־
	//This flag is set by hardware when the time/date registers (RTC_TR and RTC_DR) match the 
	//Alarm A register (RTC_ALRMAR).
	//This flag is cleared by software by writing 0.
	RTC_ClearFlag(RTC_FLAG_ALRAF);					//��Ҫ������

	//����Weakup����
	RTC_WakeUpCmd(DISABLE);

	//weakupʱ����ԴΪSPRE ΪlHz
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);

	//������
	RTC_SetWakeUpCounter(WAKEUP_PER_TIME);
	RTC_ClearITPendingBit(RTC_IT_WUT);				//���RTC WAKE UP�ı�־  
	EXTI_ClearITPendingBit(EXTI_Line22);			//���LINE22�ϵ��жϱ�־λ   
	RTC_ITConfig(RTC_IT_WUT, ENABLE);

	//ʹ��
	RTC_WakeUpCmd(ENABLE);

	//���RTCwakeup��־λ
	RTC_ClearFlag(RTC_FLAG_WUTF);

	//����Ƶ�����
	RTC_Signal_Output();
}


void RTC_Init(void)
{
	//�ϵ��ʱ�����Ƿ��Ѿ����ù�RTC��������ù������������ʱ
	//����RTC���ݼĴ���Ϊ0x32F2�������⵽���ݼĴ�������0x32F2
	//����ϵͳû��������Ҫ��������
	//The application can write or read data to and from these registers. 
	//They are powered-on by VBAT when VDD is switched off, so that they are not reset by 
	//System reset, and their contents remain valid when the device operates in low-power mode
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)
	{
		//����RTC
		RTC_Config();

		//��ӡ������Ϣ 
#ifdef RTC_Debug
		printf("��һ��ʹ��RTC\n\r");
#endif

		//����ϵ縴λ��־�Ƿ��Ѿ�����
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
			//�����ϵ縴λ
#ifdef RTC_Debug
			printf("�����ϵ縴λ\n\r");
#endif
		}
	}
	else 
	{
#ifdef RTC_Debug
		printf("RTC�Ѿ�������ɣ�����Ҫ�ظ�����\r\n");
#endif

		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
#ifdef RTC_Debug
			printf("�����ϵ縴λ\r\n");
#endif
		}

		if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
#ifdef RTC_Debug
			printf("�������Ÿ�λ\r\n");
#endif
		}

		//ʹ��PWRʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

		//�������RTC
		PWR_BackupAccessCmd(ENABLE);

		//�ȴ�RTC APB�Ĵ���ͬ��
		RTC_WaitForSynchro();
	}

	//rtc�жϹ�������
	RTC_Int_Configure();
}



/*****************************************************************************
 �� �� ��  : RTC_Byte_To_Bcd2
 ��������  : ������ת��ΪBCD��ʽ
 �������  : uint8_t Value  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��11��14��
    ��    ��   : test
    �޸�����   : �����ɺ���

*****************************************************************************/
static uint8_t RTC_Byte_To_Bcd2(uint8_t Value)
{
  uint8_t bcdhigh = 0;
  
  while (Value >= 10)
  {
    bcdhigh++;
    Value -= 10;
  }
  
  return  ((uint8_t)(bcdhigh << 4) | Value);
}


/*****************************************************************************
 �� �� ��  : RTC_Bcd2_To_Byte
 ��������  : ��BCD��ʽ����ת��Ϊʮ����
 �������  : uint8_t Value  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��11��14��
    ��    ��   : test
    �޸�����   : �����ɺ���

*****************************************************************************/
static uint8_t RTC_Bcd2_To_Byte(uint8_t Value)
{
  uint8_t tmp = 0;
  tmp = ((uint8_t)(Value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10;
  return (tmp + (Value & (uint8_t)0x0F));
}

/*****************************************************************************
 �� �� ��  : RTC_Bit_Set
 ��������  : �趨ָ��λΪ1����0
 �������  : unsigned int *p_data   
             unsigned int position  
             unsigned char flag     
 �������  : ��
 �� �� ֵ  : static
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��11��15��
    ��    ��   : test
    �޸�����   : �����ɺ���

*****************************************************************************/
static void RTC_Bit_Set(uint32_t *p_data,  uint32_t position, uint8_t flag)
{
	if(flag == 1)
	{
		*p_data |= (0x01<<(position));
	} 
	else
	{
		uint32_t c=(0x01<<(position));
		*p_data &= ~c;
	
	}		
}


/*****************************************************************************
 �� �� ��  : RTC_Read_Backup_Register
 ��������  : �ӱ��ݼĴ����ж�ȡָ��������
 �������  : uint16_t data_position  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��11��15��
    ��    ��   : test
    �޸�����   : �����ɺ���

*****************************************************************************/
//uint32_t bkr_data[5] = {0,0,0,0,0};//����ʹ��
uint32_t RTC_Read_Backup_Register(uint16_t data_info)
{
	uint8_t data_len = 0;
	uint8_t data_pos = 0; 
	uint8_t i  = 0;
	uint8_t j  = 0;
	uint8_t k  = 0;
	uint8_t m = 0;
	uint32_t ret = 0;
	uint32_t bkr_data[5] = {0};
	bkr_data[0] = RTC_ReadBackupRegister(RTC_BKP_DR0);
	bkr_data[1] = RTC_ReadBackupRegister(RTC_BKP_DR1);
	bkr_data[2] = RTC_ReadBackupRegister(RTC_BKP_DR2);
	bkr_data[3] = RTC_ReadBackupRegister(RTC_BKP_DR3);
	bkr_data[4] = RTC_ReadBackupRegister(RTC_BKP_DR4);

	data_len = (data_info>>8)&0xff;
	data_pos = (data_info&0xff);
	
	i = data_pos/32;
	j = data_pos%32;

	if(data_len > 32 - j)
	{
		for(k = 0; k < data_len; k++)
		{
			if(32 - j - k > 0)
			{
				if((bkr_data[i]>>(j+k))&0x01)
				{
					RTC_Bit_Set(&ret,k,1);
				}
				else
				{
					RTC_Bit_Set(&ret,k,0);
				}
			}
			else
			{
				if((bkr_data[i+1]>>m)&0x01)
				{
					RTC_Bit_Set(&ret,k,1);
				}
				else
				{
					RTC_Bit_Set(&ret,k,0);
				}
				m++;
			}
			
		}
	}
	else
	{
		for(k = 0; k < data_len; k++)
		{
			ret |= (bkr_data[i]>>(j+k))<<k;
		}
	}
	return ret;

}


/*****************************************************************************
 �� �� ��  : RTC_Write_Backup_Register
 ��������  : �����ݴ�ŵ�bkp���ݼĴ�����
 �������  : uint32_t data           
             uint16_t data_position  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��11��14��
    ��    ��   : test
    �޸�����   : �����ɺ���

*****************************************************************************/
//����ʹ��
uint8_t data_len = 0;
uint8_t data_pos = 0; 
uint8_t i  = 0;
uint8_t jk  = 0;
uint8_t k  = 0;
void RTC_Write_Backup_Register(uint32_t data,uint16_t data_info)
{

	uint8_t m = 0;
	uint32_t bkr_data[5] = {0};
	
	RTC_WriteBackupRegister(RTC_BKP_DR0, 0);
	RTC_WriteBackupRegister(RTC_BKP_DR1, 0);
	RTC_WriteBackupRegister(RTC_BKP_DR2, 0);
	RTC_WriteBackupRegister(RTC_BKP_DR3, 0);
	RTC_WriteBackupRegister(RTC_BKP_DR4, 0);
	
	bkr_data[0] = RTC_ReadBackupRegister(RTC_BKP_DR0);
	bkr_data[1] = RTC_ReadBackupRegister(RTC_BKP_DR1);
	bkr_data[2] = RTC_ReadBackupRegister(RTC_BKP_DR2);
	bkr_data[3] = RTC_ReadBackupRegister(RTC_BKP_DR3);
	bkr_data[4] = RTC_ReadBackupRegister(RTC_BKP_DR4);

	data_len = (data_info>>8)&0xff;
	data_pos = (data_info & 0xff);
	
	i = data_pos/32;
	jk = data_pos%32;

	if(data_len > 32 - jk)
	{
		for(k = 0; k < data_len; k++)
		{
			if(32 - jk - k > 0)
			{
				if((data >> k)&0x01) //��λΪ1
				{					
					RTC_Bit_Set(&bkr_data[i],jk+k,1);
				}
				else
				{
					RTC_Bit_Set(&bkr_data[i],jk+k,0);
				}
			}
			else
			{
				if((data >> k)&0x01) //��λΪ1
				{					
					RTC_Bit_Set(&bkr_data[i+1],m,1);
				}
				else
				{
					RTC_Bit_Set(&bkr_data[i+1],m,0);
				}
				m++;
			}
		}
	}
	else
	{
		for(k = 0; k < data_len; k++)
		{
			if((data >> k)&0x01) //��λΪ1
			{
				RTC_Bit_Set(&bkr_data[i],jk+k,1);
			}
			else
			{
				RTC_Bit_Set(&bkr_data[i],jk+k,0);
			}
		
		}
	}
	RTC_WriteBackupRegister(RTC_BKP_DR0, bkr_data[0]);
	RTC_WriteBackupRegister(RTC_BKP_DR1, bkr_data[1]);
	RTC_WriteBackupRegister(RTC_BKP_DR2, bkr_data[2]);
	RTC_WriteBackupRegister(RTC_BKP_DR3, bkr_data[3]);
	RTC_WriteBackupRegister(RTC_BKP_DR4, bkr_data[4]);


}

/*****************************************************************************
 �� �� ��  : RTC_Set_Time
 ��������  : ����rtcʱ��
 �������  : calender_Stu *calender  ---ʱ�����  RTC_Format_BIN
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��11��15��
    ��    ��   : test
    �޸�����   : �����ɺ���

*****************************************************************************/
void RTC_Set_Time(calender_Stu *calender)
{
	RTC_DateTypeDef  RTC_DateStruct;
	RTC_TimeTypeDef  RTC_TimeStruct;
	PWR_BackupAccessCmd(ENABLE);	
	
	RTC_DateStruct.RTC_WeekDay = RTC_Byte_To_Bcd2(calender->week);
	RTC_DateStruct.RTC_Month = RTC_Byte_To_Bcd2(calender->month);
	RTC_DateStruct.RTC_Date = RTC_Byte_To_Bcd2(calender->date);
	RTC_DateStruct.RTC_Year = RTC_Byte_To_Bcd2(calender->year);
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStruct);

	RTC_TimeStruct.RTC_H12 = RTC_H12_AM;
	RTC_TimeStruct.RTC_Hours = RTC_Byte_To_Bcd2(calender->hour);
	RTC_TimeStruct.RTC_Minutes = RTC_Byte_To_Bcd2(calender->min);
	RTC_TimeStruct.RTC_Seconds = RTC_Byte_To_Bcd2(calender->sec);
	
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStruct);

	PWR_BackupAccessCmd(DISABLE);
}


/*****************************************************************************
 �� �� ��  : RTC_Read_Time
 ��������  : ��ȡRTCʱ��
 �������  : calendar_Stu calendar - RTC_Format_BINʱ���ʽ
 �������  : 
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��11��15��
    ��    ��   : test
    �޸�����   : �����ɺ���

*****************************************************************************/
void RTC_Read_Time(calender_Stu *calender)
{	
	RTC_DateTypeDef  RTC_DateStruct;
	RTC_TimeTypeDef  RTC_TimeStruct;
	RTC_WaitForSynchro();
	RTC_GetDate(RTC_Format_BCD, &RTC_DateStruct);
	RTC_GetTime(RTC_Format_BCD, &RTC_TimeStruct);

	calender->year 		= 2000 + RTC_Bcd2_To_Byte(RTC_DateStruct.RTC_Year) % 2000;
	calender->month		= RTC_Bcd2_To_Byte(RTC_DateStruct.RTC_Month);
	calender->date 		= RTC_Bcd2_To_Byte(RTC_DateStruct.RTC_Date);
	calender->week		= RTC_Bcd2_To_Byte(RTC_DateStruct.RTC_WeekDay);
	calender->hour		= RTC_Bcd2_To_Byte(RTC_TimeStruct.RTC_Hours);
	calender->min		= RTC_Bcd2_To_Byte(RTC_TimeStruct.RTC_Minutes);
	calender->sec		= RTC_Bcd2_To_Byte(RTC_TimeStruct.RTC_Seconds);
}


