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


void bsp_RtcInit(void);
void RTC_DateShow(void);



#endif

