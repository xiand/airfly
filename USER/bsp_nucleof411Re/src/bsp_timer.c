
#include "bsp.h"

/* ��������ȫ�ֱ�������bsp_DelayMS()����*/
static volatile uint32_t s_uiDelayCount = 0;
static volatile uint8_t s_ucTimeOutFlag = 0;

/* �������������ʱ���Ľṹ�����*/
static SOFT_TMR s_tTmr[TMR_COUNT];

/*
	ȫ������ʱ�䣬��λ1ms
	����Ա�ʾ24.85�죬�������ʱ�䳬������ֵ
	����뿼���������
*/
__IO uint32_t	g_iRunTime = 0;

static void bsp_SoftTimerDec(SOFT_TMR * _tmr);


void bsp_InitTimer(void)
{
	uint8_t 		i	= 0;

	/*
			�������е������ʱ��
			�ڰ������������������ʱ��
			��һ��ȫ�µ�˼��
	*/
	for (i = 0; i < TMR_COUNT; i++)
	{
		s_tTmr[i].Count 	= 0;
		s_tTmr[i].PreLoad	= 0;
		s_tTmr[i].Flag		= 0;
		s_tTmr[i].Mode		= TMR_ONCE_MODE;		//ȱʡ��1���Թ���ģʽ
	}

	/*
		����systic�ж�����Ϊ1ms��������systick�ж�
		SystemCoreClock�ǹ̼��ж����ϵͳ�ں�ʱ�ӣ�����
		STM32F4xx F411RE Ϊ100MHz
		SysTick_Config()�������βα�ʾ�ں�ʱ�Ӷ��ٸ����ں󴥷�һ��
		Systick��ʱ�ж�
		--SystemCoreClock/1000 ��ʾ��ʱƵ��Ϊ1000hz����ʱ����Ϊ1ms
		--SystemCoreClock/500 ��ʾ��ʱƵ��Ϊ500hz����ʱ����Ϊ2ms
		--SystemCoreClock/2000 ��ʾ��ʱƵ��Ϊ2000hz����ʱ����Ϊ500us
		���ڳ���Ӧ�ã�����һ��ȡ��ʱ����1ms��
		���ڵ���CPU���ߵ͹���Ӧ�ã��������ö�ʱ��Ϊ10ms
	*/
	SysTick_Config(SystemCoreClock / 1000);

}


extern void bsp_RunPer1ms(void);
extern void bsp_RunPer10ms(void);


void SysTick_ISR(void)
{
	static uint8_t	s_count = 0;
	uint8_t 		i;

	//ÿ��1ms����һ�� ������bsp_DelayMS
	if (s_uiDelayCount > 0)
	{
		if (--s_uiDelayCount == 0)
		{
			s_ucTimeOutFlag 	= 1;
		}
	}

	//ÿ��1ms���������ʱ���ļ��������м�һ����
	for (i = 0; i < TMR_COUNT; i++)
	{
		bsp_SoftTimerDec(&s_tTmr[i]);
	}

	//ȫ������ʱ�䰮��ÿ1ms��1
	g_iRunTime++;

	if (g_iRunTime == 0x7fffffff)
	{
		g_iRunTime			= 0;
	}

	bsp_RunPer1ms();								//ÿ��1ms����һ�δ˺������˺�����bsp.c��

	if (++s_count >= 10)
	{
		s_count 			= 0;

		bsp_RunPer10ms();							//ÿ��10ms����һ�δ˺������˺�����bsp.c��
	}
}


static void bsp_SoftTimerDec(SOFT_TMR * _tmr)
{
	if (_tmr->Count > 0)
	{
		//�����ʱ����������1�����ö�ʱ�������־
		if (--_tmr->Count == 0)
		{
			_tmr->Flag			= 1;

			//������Զ�ģʽ�����Զ���װ������
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

	DISABLE_INT();									//�ر��ж�
	s_uiDelayCount		= n;
	s_ucTimeOutFlag 	= 0;

	ENABLE_INT();

	while (1)
	{
		bsp_Idle(); 								//cpu����ִ�в�������bsp.c�ļ�

		/*
		 �ȴ���ʱʱ�䵽
		 ע��:��������Ϊs_ucTimeOutFlag = 0 ,���Կ����Ż�����
		 ��˸ñ�����������Ϊvolatile
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
	ticks				= n * (SystemCoreClock / 1000000); //��Ҫ�Ľ�����
	tcnt				= 0;
	told				= SysTick->VAL; 			//�ս���ʱ������ֵ

	while (1)
	{
		tnow				= SysTick->VAL;

		if (tnow != told)
		{
			//ע��SYSTICK��һ���ݼ�������
			if (tnow < told)
			{
				tcnt				+= told - tnow;
			}

			//����װ�صݼ�
			else 
			{
				tcnt				+= reload - tnow + told;
			}

			told				= tnow;

			//ʱ�䳬��/����Ҫ��ʱ��ʱ�䣬���˳�
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
		//��ӡ�������Դ�����ļ�������������
		BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);

		while (1)
			;
	}

	DISABLE_INT();									// �ر��ж�

	s_tTmr[_id].Count	= _period;					//ʵʱ��������ֵ
	s_tTmr[_id].PreLoad = _period;					//�������Զ���װֵ
	s_tTmr[_id].Flag	= 0;						//��ʱʱ�䵽��־
	s_tTmr[_id].Mode	= TMR_ONCE_MODE;			// 1���Թ���ģʽ

	ENABLE_INT();									//�����ж�
}


void bsp_StartAutoTimer(uint8_t _id, uint32_t _period)
{
	if (_id >= TMR_COUNT)
	{
		//��ӡ�������Դ�����ļ�������������
		BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);

		while (1)
			;
	}

	DISABLE_INT();									// �ر��ж�

	s_tTmr[_id].Count	= _period;					//ʵʱ��������ֵ
	s_tTmr[_id].PreLoad = _period;					//�������Զ���װֵ
	s_tTmr[_id].Flag	= 0;						//��ʱʱ�䵽��־
	s_tTmr[_id].Mode	= TMR_AUTO_MODE;			// �Զ�����ģʽ

	ENABLE_INT();									//�����ж�
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


#if 0 //���ڵ���

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



