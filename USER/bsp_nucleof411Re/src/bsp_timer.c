
#include "bsp.h"

/* 定义两个全局变量用于bsp_DelayMS()函数*/
static volatile uint32_t s_uiDelayCount = 0;
static volatile uint8_t s_ucTimeOutFlag = 0;

/* 定于用于软件定时器的结构体变量*/
static SOFT_TMR s_tTmr[TMR_COUNT];

/*
	全局运行时间，单位1ms
	最长可以表示24.85天，如果运行时间超过该数值
	则必须考虑溢出问题
*/
__IO uint32_t	g_iRunTime = 0;

static void bsp_SoftTimerDec(SOFT_TMR * _tmr);


void bsp_InitTimer(void)
{
	uint8_t 		i	= 0;

	/*
			清零所有的软件定时器
			在安富莱的例子中软件定时器
			是一种全新的思想
	*/
	for (i = 0; i < TMR_COUNT; i++)
	{
		s_tTmr[i].Count 	= 0;
		s_tTmr[i].PreLoad	= 0;
		s_tTmr[i].Flag		= 0;
		s_tTmr[i].Mode		= TMR_ONCE_MODE;		//缺省是1次性工作模式
	}

	/*
		配置systic中断周期为1ms，并启动systick中断
		SystemCoreClock是固件中定义的系统内核时钟，对于
		STM32F4xx F411RE 为100MHz
		SysTick_Config()函数的形参表示内核时钟多少个周期后触发一次
		Systick定时中断
		--SystemCoreClock/1000 表示定时频率为1000hz，定时周期为1ms
		--SystemCoreClock/500 表示定时频率为500hz，定时周期为2ms
		--SystemCoreClock/2000 表示定时频率为2000hz，定时周期为500us
		对于常规应用，我们一般取定时周期1ms。
		对于低速CPU或者低功耗应用，可以设置定时器为10ms
	*/
	SysTick_Config(SystemCoreClock / 1000);

}


extern void bsp_RunPer1ms(void);
extern void bsp_RunPer10ms(void);


void SysTick_ISR(void)
{
	static uint8_t	s_count = 0;
	uint8_t 		i;

	//每隔1ms进入一次 仅用于bsp_DelayMS
	if (s_uiDelayCount > 0)
	{
		if (--s_uiDelayCount == 0)
		{
			s_ucTimeOutFlag 	= 1;
		}
	}

	//每隔1ms，对软件定时器的计数器进行减一操作
	for (i = 0; i < TMR_COUNT; i++)
	{
		bsp_SoftTimerDec(&s_tTmr[i]);
	}

	//全局运行时间爱你每1ms增1
	g_iRunTime++;

	if (g_iRunTime == 0x7fffffff)
	{
		g_iRunTime			= 0;
	}

	bsp_RunPer1ms();								//每隔1ms调用一次此函数，此函数在bsp.c中

	if (++s_count >= 10)
	{
		s_count 			= 0;

		bsp_RunPer10ms();							//每隔10ms调用一次此函数，此函数在bsp.c中
	}
}


static void bsp_SoftTimerDec(SOFT_TMR * _tmr)
{
	if (_tmr->Count > 0)
	{
		//如果定时器变量减到1则设置定时器到达标志
		if (--_tmr->Count == 0)
		{
			_tmr->Flag			= 1;

			//如果是自动模式，则自动重装计数器
			if (_tmr->Mode == TMR_AUTO_MODE)
			{
				_tmr->Count 		= _tmr->PreLoad;
			}
		}
	}
}


void bsp_DelayMS(uint32_t n)
{
	if (n == 0)
	{
		return;
	}
	else if (n == 1)
	{
		n					= 2;
	}

	DISABLE_INT();									//关闭中断
	s_uiDelayCount		= n;
	s_ucTimeOutFlag 	= 0;

	ENABLE_INT();

	while (1)
	{
		bsp_Idle(); 								//cpu空闲执行操作，见bsp.c文件

		/*
		 等待延时时间到
		 注意:编译器认为s_ucTimeOutFlag = 0 ,所以可能优化错误
		 因此该变量必须声明为volatile
			*/
		if (s_ucTimeOutFlag == 1)
		{
			break;
		}
	}

}


void bsp_DelayUS(uint32_t n)
{
	uint32_t		ticks;
	uint32_t		told;
	uint32_t		tnow;
	uint32_t		tcnt = 0;
	uint32_t		reload;

	reload				= SysTick->LOAD;
	ticks				= n * (SystemCoreClock / 1000000); //需要的节拍数
	tcnt				= 0;
	told				= SysTick->VAL; 			//刚进入时计数器值

	while (1)
	{
		tnow				= SysTick->VAL;

		if (tnow != told)
		{
			//注意SYSTICK是一个递减计数器
			if (tnow < told)
			{
				tcnt				+= told - tnow;
			}

			//重新装载递减
			else 
			{
				tcnt				+= reload - tnow + told;
			}

			told				= tnow;

			//时间超过/等于要延时的时间，则退出
			if (tcnt >= ticks)
			{
				break;
			}
		}
	}
}


void bsp_StartTimer(uint8_t _id, uint32_t _period)
{
	if (_id >= TMR_COUNT)
	{
		//打印出错误的源代码文件名，函数名称
		BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);

		while (1)
			;
	}

	DISABLE_INT();									// 关闭中断

	s_tTmr[_id].Count	= _period;					//实时计数器初值
	s_tTmr[_id].PreLoad = _period;					//计数器自动重装值
	s_tTmr[_id].Flag	= 0;						//定时时间到标志
	s_tTmr[_id].Mode	= TMR_ONCE_MODE;			// 1次性工作模式

	ENABLE_INT();									//开启中断
}


void bsp_StartAutoTimer(uint8_t _id, uint32_t _period)
{
	if (_id >= TMR_COUNT)
	{
		//打印出错误的源代码文件名，函数名称
		BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);

		while (1)
			;
	}

	DISABLE_INT();									// 关闭中断

	s_tTmr[_id].Count	= _period;					//实时计数器初值
	s_tTmr[_id].PreLoad = _period;					//计数器自动重装值
	s_tTmr[_id].Flag	= 0;						//定时时间到标志
	s_tTmr[_id].Mode	= TMR_AUTO_MODE;			// 自动工作模式

	ENABLE_INT();									//开启中断
}


void bsp_StopTimer(uint8_t _id)
{
	if (_id >= TMR_COUNT)
	{
		BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);

		while (1)
			;
	}

	DISABLE_INT();

	s_tTmr[_id].Count	= 0;
	s_tTmr[_id].Flag	= 0;
	s_tTmr[_id].Mode	= TMR_ONCE_MODE;

	ENABLE_INT();
}


uint8_t bsp_CheckTimer(uint8_t _id)
{
	if (_id >= TMR_COUNT)
	{
		return 0;
	}

	if (s_tTmr[_id].Flag == 1)
	{
		s_tTmr[_id].Flag	= 0;
		return 1;
	}
	else 
	{
		return 0;
	}
}


int32_t bsp_GetRunTime(void)
{
	int32_t 		runtime;

	DISABLE_INT();

	runtime 			= g_iRunTime;

	ENABLE_INT();

	return runtime;
}


#if 0 //用于调试

static uint32_t ui_count = 0;

ui_count++;

if (ui_count == 1000)
{
	printf("hello world \r\n");
	ui_count			= 0;
}


#endif


void My_SysTick_Handler(void)
{
	SysTick_ISR();
}



