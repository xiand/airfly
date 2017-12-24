#include "bsp_SysClock.h"


////HSI_SetSysClock(16, 400, 4, 7); //���ﲢ����Ҫ�����������Ϊ�Ѿ�ϵͳ��������ʱ���Ѿ���void SystemInit(void)����������
void HSI_SetSysClock(uint32_t M,uint32_t N,uint32_t p,uint32_t q)
{
	__IO uint32_t HSIStartUpStatus = 0;
	//RCCʱ�ӳ�ʼ��Ϊ��λ״̬
	RCC_DeInit();

	//ʹ��HSI,HSI = 16M
	RCC_HSICmd(ENABLE);

	HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY;

	//HSIʱ�Ӿ���
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
		//�����Ҫ��Ƶ��Ӧ������������
		//����PLL ʱ����Դ������vco��Ƶ����?M
		//����PLL��Ƶ����ΪN
		//����ʱ��ϵͳ��Ƶ����P������OTG FS SDIO RNG��Ƶ����Ϊq
		RCC_PLLConfig(RCC_PLLSource_HSI,M,N,p,q);

		//ʹ��PLL
		RCC_PLLCmd(ENABLE);
		//�ȴ�PLLʱ���ȶ�
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}
		//����FLASHԤȡ��ָ��棬���ݻ���͵ȴ�״̬
		FLASH->ACR = FLASH_ACR_PRFTEN|FLASH_ACR_ICEN|FLASH_ACR_DCEN|FLASH_ACR_LATENCY_2WS;
		//��PLL�ȶ�֮�󣬰�PLLʱ���л�λϵͳʱ��
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		while(0x08 != RCC_GetSYSCLKSource())
		{
		}
	}
	else
	{
		//��ʾHSI����������
		while(1)
		{

		}
	}
	
	
}



void HSE_SetSysClock(uint32_t m, uint32_t n, uint32_t p, uint32_t q)
{
	__IO uint32_t	HSEStartUpStatus = 0;

	// ʹ�� HSE�������ⲿ���񣬱��� F407 ʹ�� HSE=25M 
	RCC_HSEConfig(RCC_HSE_ON);

	// �ȴ� HSE �����ȶ� 
	HSEStartUpStatus	= RCC_WaitForHSEStartUp();

	if (HSEStartUpStatus == SUCCESS)
	{
		// ��ѹ����ѹ�����������Ϊ 1���Ա�������Ϊ���Ƶ�� 
		// ����ʱʹ���ܺ͹���ʵ��ƽ�� 
		RCC->APB1ENR		|= RCC_APB1ENR_PWREN;
		PWR->CR 			|= PWR_CR_VOS;

		// HCLK = SYSCLK / 1 
		RCC_HCLKConfig(RCC_SYSCLK_Div1);

		// PCLK2 = HCLK / 2 
		RCC_PCLK2Config(RCC_HCLK_Div1);

		// PCLK1 = HCLK / 4 
		RCC_PCLK1Config(RCC_HCLK_Div2);

		// ���Ҫ��Ƶ�͵������������� 
		// ���� PLL ��Դʱ�ӣ����� VCO ��Ƶ���� m������ VCO ��Ƶ���� n�� 
		//	����ϵͳʱ�ӷ�Ƶ���� p������ OTG FS,SDIO,RNG ��Ƶ���� q 
		RCC_PLLConfig(RCC_PLLSource_HSE, m, n, p, q);

		// ʹ�� PLL 
		RCC_PLLCmd(ENABLE);

		// �ȴ� PLL �ȶ� 
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}

		/*-----------------------------------------------------*/

		// ���� FLASH Ԥȡָ,ָ���,���ݻ���͵ȴ�״̬ 
		FLASH->ACR			= FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;
		/*-----------------------------------------------------*/

		// �� PLL �ȶ�֮�󣬰� PLL ʱ���л�Ϊϵͳʱ�� SYSCLK 
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		// ��ȡʱ���л�״̬λ��ȷ�� PLLCLK ��ѡΪϵͳʱ�� 
		while (RCC_GetSYSCLKSource() != 0x08)
		{
		}
	}
	else 
	{
		// HSE ���������� 
		while (1)
		{
		}
	}
}



