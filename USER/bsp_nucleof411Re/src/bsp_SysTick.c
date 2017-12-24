#include "bsp_SysTick.h"

static __IO u32 TimingDelay;




void Delay_ms(__IO u32 nTime)
{
    TimingDelay = nTime;

    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    while(TimingDelay != 0);
}

void SysTick_Init(void)
{
    /*
    * t : 定时时间
    * Ticks : 多少个时钟周期产生一次中断
    * f : 时钟频率 100000000
    * t = Ticks * 1/f = (100000000/1000) * (1/100000000) = 1ms
    */
    if(SysTick_Config(SystemCoreClock/1000))
    {
        while(1);
    }
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
}
//bsp_SysTick定时器中断调用
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00)
    {
        TimingDelay--;
    }
}

