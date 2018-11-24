#ifndef _BSP_RTC_H_
#define _BSP_RTC_H_
#include "stm32f4xx.h"

//���õ�ǰʱ������ϵͳ��ʼ��
#define CURRENT_YEAR 0x17
#define CURRENT_MONTH RTC_Month_November
#define CURRENT_DAY 0x04
#define CURRENT_WEEKDAY RTC_Weekday_Saturday
#define CURRENT_HOUR 0x23
#define CURRENT_MIN 0x22
#define CURRENT_SEC 0x00

//Ƶ�����Ϊ1hz����512hz
#define RTC_OUTPUT_FRQ RTC_CalibOutput_512Hz   //RTC_CalibOutput_512Hz
#define WAKEUP_PER_TIME 10 //ÿ��10s����һ��

//���õ�ǰʱ������ϵͳ��ʼ��

#define ALARM_HOUR 0x23
#define ALARM_MIN 0x22
#define ALARM_SEC 0x30


//��ǰʱ��ṹ��
typedef struct {
	vu8 	hour; 
	vu8 	min; 
	vu8 	sec;       
	//������������ 
	vu16 	year; 
	vu8		month; 
	vu8		date; 
	vu8		week; 
}calender_Stu;

//bkp�Ĵ���������λ����Ϣ��ʽ��(��������Ҫλ��<<8)|(���ݵĿ�ʼλ��)
#define RTC_INIT_FLAG    (1<<8)|(0)     //�ж�RTC�Ƿ��ʼ��
#define REMOTE_SCH_TIME  (12<<8)|(1)     //ң����ԤԼ��Чλ--ң����ԤԼ���� 60 - 111100--  ң����ԤԼСʱ 24 - 11000



/*****************************************************************************
 �� �� ��  : RTC_Read_Backup_Register
 ��������  : �ӱ��ݼĴ����ж�ȡָ��������
 �������  : uint16 data_position  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��11��15��
    ��    ��   : test
    �޸�����   : �����ɺ���

*****************************************************************************/
uint32_t RTC_Read_Backup_Register(uint16_t data_info);


/*****************************************************************************
 �� �� ��  : RTC_Write_Backup_Register
 ��������  : �����ݴ�ŵ�bkp���ݼĴ�����
 �������  : uint32 data           
             uint16 data_position  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��11��14��
    ��    ��   : test
    �޸�����   : �����ɺ���

*****************************************************************************/
void RTC_Write_Backup_Register(uint32_t data,uint16_t data_info);


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
void RTC_Set_Time(calender_Stu *calender);

/*****************************************************************************
 �� �� ��  : RTC_Read_Time
 ��������  : ��ȡRTCʱ��
 �������  : calendar_Stu calendar - RTC_Format_BINʱ���ʽ
 �������  : 
 �� �� ֵ: 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��11��15��
    ��    ��   : test
    �޸�����   : �����ɺ���

*****************************************************************************/
void RTC_Read_Time(calender_Stu *calender);


void RTC_Init(void);




#endif

