
#include "bsp.h"

/*
*  ���ע��
* (1)DWT�Ĵ�����ARMv7�ܹ���оƬ�ڲ���һ��ͨ��ģ��
*	   ���е�ARMv7�ܹ���оƬ���иù��ܣ����Դ˴�����Ϊcommon����
* (2)DWTģ��ȫ��Ϊ"The Debug Watchpoint and Trace (DWT) unit"��
*	   ����ϵͳ���Լ�����,��ϸ���Բο�ARM��˾�Ĺٷ��ֲ�
* (3)�ڵ���ģʽ�£��ù����Զ���ʹ�ܣ�������ģʽ���򲻻�ʹ�ܣ�
*	   ��������Ҫʹ�ܸù���ģ��
* (4)��ģ���CYCCNT����������Ҫ�õ��Ĺ��ܣ��ü�������ģ��ʹ�ܺ�
*	   ������CPU��ÿ���������Լ�1�Σ�������λ����
* (5)���ڹ���DWT���ܵ���ϸ ����ĵ���ARMv7-M Architecture Reference Manual-DDI0403E_B_armv7m_arm��
*/
//ʹ��DWTģ�鹦��ʹ��  
//Global enable for all DWT and ITM features:
//0  DWT and ITM units disabled.
//1  DWT and ITM units enabled.
//#define DEMCR  (*(unsigned int *)0xE000EDFC)	//����һ��Ҫʹ��volatile���в������� �������us��ʱ����
static volatile uint32_t * DEMCR = (unsigned int *) 0xE000EDFC;

#define TRCENA					0x01000000u  //DEMCRģ����DWT����ʹ��λ

//ʹ��CYCCNT��������ʼ����
//#define DWT_CTRL (*(unsigned int *)0xE0001000)
static volatile uint32_t * DWT_CONTROL = (unsigned int *) 0xE0001000;

#define CYCCNTENA				0x01u  //DWT��SYCCNTʹ��λ

//CYCCNT�������ڲ�ֵ(32λ�޷���)
//#define DWT_CYCCNT (*(unsigned int*)0xE0001004)
static volatile uint32_t * DWT_CYCCNT = (unsigned int *) 0xE0001004;

static volatile uint32_t * LAR = (uint32_t *) 0xE0001FB0; // <-- added lock access register


//ϵͳCPUƵ��Hz
//static unsigned int g_ulsysClockFreq = 0;
//��ʱ�����壬���ڲ��Դ�������ʱ�䳤��
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
*���ܣ���ʼ��DWT����ģ��
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
*�������ܣ�ʹ��DWT�������΢�뼶��ʱ
*�βΣ�ulTime��Ҫ��ʱʱ��
*����ֵ����
***********************************
*/
void bsp_DwtDelayUs(unsigned int ulTime)
{
	unsigned int	ulTickStart = 0;

	//unsigned int ulTickEnd =0;
	unsigned int	ulCurTick = 0;
	unsigned int	ulTickDelay = 0;


	//��¼�µ�ǰ��CYCCNT����ֵ
	ulTickStart 		= *DWT_CYCCNT;

	//g_ulsysClockFreq = DEFAULT_CPU_SYSCLK_HZ;
	//��΢�������ɵδ���
	ulTickDelay 		= (ulTime * (DEFAULT_CPU_SYSCLK_HZ / 1000000));

	//����һ�ֺܺõļ��㷽ʽ�������Ǽ��������¼�������Ȼ������ȷ����
	while (ulCurTick < ulTickDelay)
	{
		ulCurTick			= *DWT_CYCCNT - ulTickStart;

		//printf("debug ulCurTick = %ud,ulTickStart = %X,DWT_CYCCNT = %X\r\n",ulCurTick,*DWT_CYCCNT);
	}

#if 0 //����Ҳ��һ�ַ�ʽ����

	//Ŀ�����ֵ 
	ulTickEnd			= ulTickStart + ulTickDelay;

	if (ulTickEnd > ulTickStart)
	{ //��������������

		while (DWT_CYCCNT < ulTickEnd)
			;
	}
	else 
	{ //�����������λ��ת 

		while (DWT_CYCCNT >= ulTickEnd)
			;

		while (DWT_CYCCNT < ulTickEnd)
			;
	}

#endif
}


/*
****************************************
�������ܣ�dwt���ܺ��뼶��ʱ����
�βΣ���Ҫ��ʱ�ĺ�����
����ֵ����
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
*�������ܣ������������ʱ��
*������_id--��ʱ���ı�� ��Χ��0��(DWT_TIME_COUNTER_NUM-1)
	   flag -- ��ʱ���Ķ���  DWT_TIME_COUNTER_BEGIN:��ʱ��ʼ
						   DWT_TIME_COUNTER_END  :��ʱ����
	   ulRunTime ��������ʱ��
*����ֵ��0-��ȷ
		 -1 _id��������
		 -2 _flag��������
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
		//��������ʼ��������¼��ǰ�ļ���ֵ
		m_stTimeCounter[_id].beginTime = *DWT_CYCCNT;
		return 0;
	}
	else if (flag == DWT_TIME_COUNTER_END)
	{
		//��������������¼��ǰ�ļ���ֵ���������ֵ
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


