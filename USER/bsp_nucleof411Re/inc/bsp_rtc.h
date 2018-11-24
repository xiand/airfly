#ifndef _BSP_RTC_H_
#define _BSP_RTC_H_
#include "stm32f4xx.h"

//设置当前时间用于系统初始化
#define CURRENT_YEAR 0x17
#define CURRENT_MONTH RTC_Month_November
#define CURRENT_DAY 0x04
#define CURRENT_WEEKDAY RTC_Weekday_Saturday
#define CURRENT_HOUR 0x23
#define CURRENT_MIN 0x22
#define CURRENT_SEC 0x00

//频率输出为1hz或者512hz
#define RTC_OUTPUT_FRQ RTC_CalibOutput_512Hz   //RTC_CalibOutput_512Hz
#define WAKEUP_PER_TIME 10 //每隔10s唤醒一次

//设置当前时间用于系统初始化

#define ALARM_HOUR 0x23
#define ALARM_MIN 0x22
#define ALARM_SEC 0x30


//当前时间结构体
typedef struct {
	vu8 	hour; 
	vu8 	min; 
	vu8 	sec;       
	//公历日月年周 
	vu16 	year; 
	vu8		month; 
	vu8		date; 
	vu8		week; 
}calender_Stu;

//bkp寄存器中数据位置信息格式：(数据所需要位数<<8)|(数据的开始位置)
#define RTC_INIT_FLAG    (1<<8)|(0)     //判断RTC是否初始化
#define REMOTE_SCH_TIME  (12<<8)|(1)     //遥控器预约有效位--遥控器预约分钟 60 - 111100--  遥控器预约小时 24 - 11000



/*****************************************************************************
 函 数 名  : RTC_Read_Backup_Register
 功能描述  : 从备份寄存器中读取指定的数据
 输入参数  : uint16 data_position  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年11月15日
    作    者   : test
    修改内容   : 新生成函数

*****************************************************************************/
uint32_t RTC_Read_Backup_Register(uint16_t data_info);


/*****************************************************************************
 函 数 名  : RTC_Write_Backup_Register
 功能描述  : 将数据存放到bkp备份寄存器中
 输入参数  : uint32 data           
             uint16 data_position  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年11月14日
    作    者   : test
    修改内容   : 新生成函数

*****************************************************************************/
void RTC_Write_Backup_Register(uint32_t data,uint16_t data_info);


/*****************************************************************************
 函 数 名  : RTC_Set_Time
 功能描述  : 设置rtc时间
 输入参数  : calender_Stu *calender  ---时间采用  RTC_Format_BIN
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年11月15日
    作    者   : test
    修改内容   : 新生成函数

*****************************************************************************/
void RTC_Set_Time(calender_Stu *calender);

/*****************************************************************************
 函 数 名  : RTC_Read_Time
 功能描述  : 读取RTC时间
 输入参数  : calendar_Stu calendar - RTC_Format_BIN时间格式
 输出参数  : 
 返 回 值: 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年11月15日
    作    者   : test
    修改内容   : 新生成函数

*****************************************************************************/
void RTC_Read_Time(calender_Stu *calender);


void RTC_Init(void);




#endif

