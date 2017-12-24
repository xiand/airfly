#include "bsp_SysClock.h"


////HSI_SetSysClock(16, 400, 4, 7); //这里并不需要这个函数，因为已经系统在启动的时候已经在void SystemInit(void)进行了配置
void HSI_SetSysClock(uint32_t M,uint32_t N,uint32_t p,uint32_t q)
{
	__IO uint32_t HSIStartUpStatus = 0;
	//RCC时钟初始化为复位状态
	RCC_DeInit();

	//使能HSI,HSI = 16M
	RCC_HSICmd(ENABLE);

	HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY;

	//HSI时钟就绪
	if(HSIStartUpStatus == RCC_CR_HSIRDY)
	{
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR |= PWR_CR_VOS;

		//HCLK = SYSCLK/1
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		//PCLK2 = HCLK/2
		RCC_PCLK2Config(RCC_HCLK_Div1);
		//PCLK1 = HCLK/2
		RCC_PCLK1Config(RCC_HCLK_Div2);
		//如果需要超频就应该设置在这里
		//设置PLL 时钟来源，设置vco分频因子?M
		//设置PLL倍频因子为N
		//设置时钟系统分频因子P，设置OTG FS SDIO RNG分频因子为q
		RCC_PLLConfig(RCC_PLLSource_HSI,M,N,p,q);

		//使能PLL
		RCC_PLLCmd(ENABLE);
		//等待PLL时钟稳定
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}
		//配置FLASH预取，指令缓存，数据缓存和等待状态
		FLASH->ACR = FLASH_ACR_PRFTEN|FLASH_ACR_ICEN|FLASH_ACR_DCEN|FLASH_ACR_LATENCY_2WS;
		//当PLL稳定之后，把PLL时钟切换位系统时钟
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		while(0x08 != RCC_GetSYSCLKSource())
		{
		}
	}
	else
	{
		//表示HSI启动出错处理
		while(1)
		{

		}
	}
	
	
}



void HSE_SetSysClock(uint32_t m, uint32_t n, uint32_t p, uint32_t q)
{
	__IO uint32_t	HSEStartUpStatus = 0;

	// 使能 HSE，开启外部晶振，秉火 F407 使用 HSE=25M 
	RCC_HSEConfig(RCC_HSE_ON);

	// 等待 HSE 启动稳定 
	HSEStartUpStatus	= RCC_WaitForHSEStartUp();

	if (HSEStartUpStatus == SUCCESS)
	{
		// 调压器电压输出级别配置为 1，以便在器件为最大频率 
		// 工作时使性能和功耗实现平衡 
		RCC->APB1ENR		|= RCC_APB1ENR_PWREN;
		PWR->CR 			|= PWR_CR_VOS;

		// HCLK = SYSCLK / 1 
		RCC_HCLKConfig(RCC_SYSCLK_Div1);

		// PCLK2 = HCLK / 2 
		RCC_PCLK2Config(RCC_HCLK_Div1);

		// PCLK1 = HCLK / 4 
		RCC_PCLK1Config(RCC_HCLK_Div2);

		// 如果要超频就得在这里下手啦 
		// 设置 PLL 来源时钟，设置 VCO 分频因子 m，设置 VCO 倍频因子 n， 
		//	设置系统时钟分频因子 p，设置 OTG FS,SDIO,RNG 分频因子 q 
		RCC_PLLConfig(RCC_PLLSource_HSE, m, n, p, q);

		// 使能 PLL 
		RCC_PLLCmd(ENABLE);

		// 等待 PLL 稳定 
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}

		/*-----------------------------------------------------*/

		// 配置 FLASH 预取指,指令缓存,数据缓存和等待状态 
		FLASH->ACR			= FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;
		/*-----------------------------------------------------*/

		// 当 PLL 稳定之后，把 PLL 时钟切换为系统时钟 SYSCLK 
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		// 读取时钟切换状态位，确保 PLLCLK 被选为系统时钟 
		while (RCC_GetSYSCLKSource() != 0x08)
		{
		}
	}
	else 
	{
		// HSE 启动出错处理 
		while (1)
		{
		}
	}
}



