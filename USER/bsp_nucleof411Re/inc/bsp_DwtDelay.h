

#ifndef _BSP_DWTDELAY_H_
#define _BSP_DWTDELAY_H_

//默认的系统CPU频率 100MHz
#define DEFAULT_CPU_SYSCLK_HZ	(100000000)

//定义计时器个数
#define DWT_TIME_COUNTER_NUM	(10)	

//计时器开始标志
#define DWT_TIME_COUNTER_BEGIN	(0) 	

//计时器结束标志
#define DWT_TIME_COUNTER_END	(1) 	



#ifdef __cplusplus
extern "C"
{
#endif

	/*
	功 	 能：    初始化DWT模块功能
	参 	 数：    ulsysclk -- 当前CPU的时钟频率 单位:Hz
	返  回  值：	无
	注意 事 项：	 
	所在文件名：	  
	*/
	void bsp_dwtInit(void);

	/*
	功 	 能：    精确的微妙级延时
	参 	 数：    nCount -- 需要延时的时间 单位:微妙(us)
	返  回  值：	无
	注意 事 项：	 
	所在文件名：	  
	*/
	void bsp_DwtDelayUs(unsigned int ulTime);


	/*
	****************************************
	函数功能：dwt功能毫秒级延时函数
	形参：需要延时的毫秒数
	返回值：无
	****************************************
	*/
	void bsp_DwtDelayMs(unsigned int ultime);

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
	int bsp_TimeCounter(int _id, int flag, unsigned int * ulRunTime);

#ifdef __cplusplus
}


#endif


#endif

