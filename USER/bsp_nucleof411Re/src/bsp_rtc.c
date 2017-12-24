
#include "bsp.h"

#define RTC_Debug //用于调试模式宏

#define RTC_CLOCK_SOURCE_LSE //使用外部时钟,Nucleon存在外部禁止

//#define RTC_CLOCK_SOURCE_LSI	 //使用内部时钟
RTC_TimeTypeDef RTC_TimeStructure;
RTC_InitTypeDef RTC_InitStructure;
RTC_AlarmTypeDef RTC_AlarmStructure;
RTC_DateTypeDef RTC_DateStructure;

__IO uint32_t	uwAsynchPrediv = 0;
__IO uint32_t	uwSynchPrediv = 0;

//配置PC13号引脚输出波形
//波形可以选择1hz或者512hz
static void rtc_SignalOutput(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//使能时钟
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


//中断配置
//配置 AlarmA中断
//配置wakeup中断
static void rtc_InteruptConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	//配置RTC闹钟中断设置
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	//使能RTC闹钟中断
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;

	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	//配置RTC Wakeup中断设置
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	//使能RTC闹钟中断
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;

	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}


//1、配置时钟
//2、rtc基本时间配置
//3、alarmA设置中断设置
//4、wakeup中断设置
//5、时钟输出512hz或者1hz
static void rtc_Config(void)
{
	//使能PWR时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	//允许访问RTC
	PWR_BackupAccessCmd(ENABLE);

	//选择时钟源
#if defined(RTC_CLOCK_SOURCE_LSI)	

	//使能 LSI
	RCC_LSICmd(ENABLE);

	//等待LSI稳定
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
	}

	//选择LSI作为RTC时钟来源
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

#elif defined(RTC_CLOCK_SOURCE_LSE) 
	RCC_LSEConfig(RCC_LSE_ON);

	//等待LSE时钟就绪
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}

	//选择RTC时钟来源
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

#else

#error please						select the RTC Clock source 
#endif

	/* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
	//17.3.1  Clock and prescalers	详细参考
	uwSynchPrediv		= 0xFF;
	uwAsynchPrediv		= 0x7F;

	//使能RTC时钟
	RCC_RTCCLKCmd(ENABLE);

	//等待RTC APB寄存器同步
	RTC_WaitForSynchro();

	//配置RTC时间格式和分频器
	RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
	RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);					//Init函数只完成这三个设置

	//设置时间：年、月、日及星期
	RTC_DateStructure.RTC_Year = CURRENT_YEAR;
	RTC_DateStructure.RTC_Month = CURRENT_MONTH;
	RTC_DateStructure.RTC_Date = CURRENT_DAY;
	RTC_DateStructure.RTC_WeekDay = CURRENT_WEEKDAY;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

	//设置时间：分钟、秒、及显示格式
	RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours = CURRENT_HOUR;
	RTC_TimeStructure.RTC_Minutes = CURRENT_MIN;
	RTC_TimeStructure.RTC_Seconds = CURRENT_SEC;
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);

	//配置备份寄存器，表示已经完成基本设置
	RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);

	//设置闹钟，BCD格式
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = ALARM_HOUR;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = ALARM_MIN;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = ALARM_SEC;
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

	//配置RTC Alarm A寄存器
	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);

	//使能RTC Alarm A中断
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	//使能闹钟
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

	//清楚RTC闹钟标志
	//This flag is set by hardware when the time/date registers (RTC_TR and RTC_DR) match the 
	//Alarm A register (RTC_ALRMAR).
	//This flag is cleared by software by writing 0.
	RTC_ClearFlag(RTC_FLAG_ALRAF);					//需要软件清除

	//配置Weakup报警
	RTC_WakeUpCmd(DISABLE);

	//weakup时钟来源为SPRE 为lHz
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);

	//计数器
	RTC_SetWakeUpCounter(WAKEUP_PER_TIME);
	RTC_ClearITPendingBit(RTC_IT_WUT);				//清除RTC WAKE UP的标志  
	EXTI_ClearITPendingBit(EXTI_Line22);			//清除LINE22上的中断标志位   
	RTC_ITConfig(RTC_IT_WUT, ENABLE);

	//使能
	RTC_WakeUpCmd(ENABLE);

	//清除RTCwakeup标志位
	RTC_ClearFlag(RTC_FLAG_WUTF);

	//配置频率输出
	rtc_SignalOutput();
}


void bsp_RtcInit(void)
{
	//上电的时候检测是否已经配置过RTC，如果配置过，在配置完成时
	//设置RTC备份寄存器为0x32F2，如果检测到备份寄存器不是0x32F2
	//表明系统没有配置需要重新配置
	//The application can write or read data to and from these registers. 
	//They are powered-on by VBAT when VDD is switched off, so that they are not reset by 
	//System reset, and their contents remain valid when the device operates in low-power mode
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)
	{
		//配置RTC
		rtc_Config();

		//打印调试信息 
#ifdef RTC_Debug
		printf("第一次使用RTC\n\r");
#endif

		//检测上电复位标志是否已经设置
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
			//发生上电复位
#ifdef RTC_Debug
			printf("存在上电复位\n\r");
#endif
		}
	}
	else 
	{
#ifdef RTC_Debug
		printf("RTC已经配置完成，不需要重复配置\r\n");
#endif

		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
#ifdef RTC_Debug
			printf("发生上电复位\r\n");
#endif
		}

		if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
#ifdef RTC_Debug
			printf("发生引脚复位\r\n");
#endif
		}

		//使能PWR时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

		//允许访问RTC
		PWR_BackupAccessCmd(ENABLE);

		//等待RTC APB寄存器同步
		RTC_WaitForSynchro();
	}

	//rtc中断功能配置
	rtc_InteruptConfig();
}


//uint8_t aShowTime[50] = {0};
void RTC_DateShow(void)
{
	/* 得到当前时分秒*/
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	printf("%0.2d年 %0.2d月 %0.2d日 星期%0.1d\r\n", RTC_DateStructure.RTC_Year, 
		RTC_DateStructure.RTC_Month, 
		RTC_DateStructure.RTC_Date, 
		RTC_DateStructure.RTC_WeekDay);
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	printf("%0.2d:%0.2d:%0.2d\r\n", RTC_TimeStructure.RTC_Hours, 
		RTC_TimeStructure.RTC_Minutes, 
		RTC_TimeStructure.RTC_Seconds);


	//printf(" RTC_GetWakeUpCounter = %d\r\n",RTC_GetWakeUpCounter());
}


