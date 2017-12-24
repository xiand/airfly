
#include "bsp.h"

/*
*  添加注释
* (1)DWT寄存器是ARMv7架构的芯片内部的一个通用模块
*	   所有的ARMv7架构的芯片都有该功能，所以此处定义为common功能
* (2)DWT模块全称为"The Debug Watchpoint and Trace (DWT) unit"，
*	   用于系统调试及跟踪,详细可以参考ARM公司的官方手册
* (3)在调试模式下，该功能自动会使能，但正常模式下则不会使能，
*	   所以首先要使能该功能模块
* (4)该模块的CYCCNT计数器是需要用到的功能，该计数器从模块使能后，
*	   会随着CPU的每次跳动，自加1次，溢满后复位重来
* (5)关于关于DWT功能的详细 请见文档《ARMv7-M Architecture Reference Manual-DDI0403E_B_armv7m_arm》
*/
//使能DWT模块功能使能  
//Global enable for all DWT and ITM features:
//0  DWT and ITM units disabled.
//1  DWT and ITM units enabled.
//#define DEMCR  (*(unsigned int *)0xE000EDFC)	//这里一定要使用volatile进行参数定义 看下面的us延时函数
static volatile uint32_t * DEMCR = (unsigned int *) 0xE000EDFC;

#define TRCENA					0x01000000u  //DEMCR模块中DWT功能使能位

//使能CYCCNT计数器开始计数
//#define DWT_CTRL (*(unsigned int *)0xE0001000)
static volatile uint32_t * DWT_CONTROL = (unsigned int *) 0xE0001000;

#define CYCCNTENA				0x01u  //DWT的SYCCNT使能位

//CYCCNT计数器内部值(32位无符号)
//#define DWT_CYCCNT (*(unsigned int*)0xE0001004)
static volatile uint32_t * DWT_CYCCNT = (unsigned int *) 0xE0001004;

static volatile uint32_t * LAR = (uint32_t *) 0xE0001FB0; // <-- added lock access register


//系统CPU频率Hz
//static unsigned int g_ulsysClockFreq = 0;
//计时器定义，用于测试代码运行时间长度
typedef struct _Time_Counter_struct_
{
unsigned int	beginTime;
unsigned int	endTime;
unsigned int	deltaTime;
unsigned int	maxVal;
unsigned int	minVal;
} TIME_COUNTER_STRUCT;


TIME_COUNTER_STRUCT m_stTimeCounter[DWT_TIME_COUNTER_NUM] =
{
	{
		0, 0, 0, 0, 1000000000
	},
	{
		0, 0, 0, 0, 1000000000
	},
	{
		0, 0, 0, 0, 1000000000
	},
	{
		0, 0, 0, 0, 1000000000
	},
	{
		0, 0, 0, 0, 1000000000
	},
	{
		0, 0, 0, 0, 1000000000
	},
	{
		0, 0, 0, 0, 1000000000
	},
	{
		0, 0, 0, 0, 1000000000
	},
	{
		0, 0, 0, 0, 1000000000
	},
	{
		0, 0, 0, 0, 1000000000
	}
};


/*
********************************
*功能：初始化DWT功能模块
********************************
*/
void bsp_dwtInit(void)
{
	*DEMCR				= *DEMCR | TRCENA;
	*LAR				= 0xC5ACCE55;				// <-- added unlock access to DWT (ITM, etc.)registers	
	*DWT_CYCCNT 		= 0;						// clear DWT cycle counter
	*DWT_CONTROL		= *DWT_CONTROL | CYCCNTENA; // enable DWT cycle counter
}


/*
************************************
*函数功能：使用DWT功能完成微秒级延时
*形参：ulTime需要延时时间
*返回值：无
***********************************
*/
void bsp_DwtDelayUs(unsigned int ulTime)
{
	unsigned int	ulTickStart = 0;

	//unsigned int ulTickEnd =0;
	unsigned int	ulCurTick = 0;
	unsigned int	ulTickDelay = 0;


	//记录下当前的CYCCNT计数值
	ulTickStart 		= *DWT_CYCCNT;

	//g_ulsysClockFreq = DEFAULT_CPU_SYSCLK_HZ;
	//将微妙数换成滴答数
	ulTickDelay 		= (ulTime * (DEFAULT_CPU_SYSCLK_HZ / 1000000));

	//这是一种很好的计算方式，就算是计数器重新计数，依然可以正确计算
	while (ulCurTick < ulTickDelay)
	{
		ulCurTick			= *DWT_CYCCNT - ulTickStart;

		//printf("debug ulCurTick = %ud,ulTickStart = %X,DWT_CYCCNT = %X\r\n",ulCurTick,*DWT_CYCCNT);
	}

#if 0 //下面也是一种方式进行

	//目标计数值 
	ulTickEnd			= ulTickStart + ulTickDelay;

	if (ulTickEnd > ulTickStart)
	{ //正常不溢出情况下

		while (DWT_CYCCNT < ulTickEnd)
			;
	}
	else 
	{ //计数溢出，复位翻转 

		while (DWT_CYCCNT >= ulTickEnd)
			;

		while (DWT_CYCCNT < ulTickEnd)
			;
	}

#endif
}


/*
****************************************
函数功能：dwt功能毫秒级延时函数
形参：需要延时的毫秒数
返回值：无
****************************************
*/
void bsp_DwtDelayMs(unsigned int ultime)
{
	unsigned int	i	= 0;

	for (; i < ultime; i++)
	{
		bsp_DwtDelayUs(1000);
	}
}


/*
*************************************
*函数功能：计算代码运行时间
*参数：_id--计时器的编号 范围从0到(DWT_TIME_COUNTER_NUM-1)
	   flag -- 计时器的动作  DWT_TIME_COUNTER_BEGIN:计时开始
						   DWT_TIME_COUNTER_END  :计时结束
	   ulRunTime 保存运行时间
*返回值：0-正确
		 -1 _id参数错误
		 -2 _flag参数错误
*************************************
*/
int bsp_TimeCounter(int _id, int flag, unsigned int * ulRunTime)
{
	if (_id > DWT_TIME_COUNTER_NUM)
	{
		return - 1;
	}

	if (flag == DWT_TIME_COUNTER_BEGIN)
	{
		//计数器开始计数，记录当前的计数值
		m_stTimeCounter[_id].beginTime = *DWT_CYCCNT;
		return 0;
	}
	else if (flag == DWT_TIME_COUNTER_END)
	{
		//计数器结束，记录当前的计数值，并计算差值
		m_stTimeCounter[_id].endTime = *DWT_CYCCNT;

		m_stTimeCounter[_id].deltaTime = m_stTimeCounter[_id].endTime - m_stTimeCounter[_id].beginTime;

		if (m_stTimeCounter[_id].deltaTime > m_stTimeCounter[_id].maxVal)
			m_stTimeCounter[_id].maxVal = m_stTimeCounter[_id].deltaTime;

		if (m_stTimeCounter[_id].deltaTime < m_stTimeCounter[_id].minVal)
			m_stTimeCounter[_id].minVal = m_stTimeCounter[_id].deltaTime;

		*ulRunTime			= m_stTimeCounter[_id].deltaTime;
		return 0;
	}
	else 
	{
		return - 2;
	}
}


