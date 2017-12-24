
#include "bsp.h"

#if 0

//将按键配置为输入模式
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	//使能时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}


void KEY_Exit_Init(void)
{
	EXTI_InitTypeDef KEY_EXTI_InitStruct;
	NVIC_InitTypeDef KEY_NVIC_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

	KEY_EXTI_InitStruct.EXTI_Line = EXTI_Line13;
	KEY_EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	KEY_EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	KEY_EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&KEY_EXTI_InitStruct);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	KEY_NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
	KEY_NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	KEY_NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	KEY_NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&KEY_NVIC_InitStruct);
}


#endif

#define RCC_ALL_KEY 			RCC_AHB1Periph_GPIOC   //按键接口对应的时钟

#define GPIO_PORT_K1			GPIOC
#define GPIO_PIN_K1 			GPIO_Pin_13

static KEY_T	s_tBtn[KEY_COUNT];
static KEY_FIFO_T s_tKey;

static void bsp_InitKeyVar(void);
static void bsp_InitKeyHard(void);
static void bsp_DetectKey(uint8_t i);


static uint8_t IsKeyDown1(void)
{
	if ((GPIO_PORT_K1->IDR & GPIO_PIN_K1) == 0)
		return 1;

	else 
		return 0;
}


void bsp_InitKey(void)
{
	bsp_InitKeyVar();								// 初始化按键变量
	bsp_InitKeyHard();								//初始化按键硬件
}


void bsp_PutKey(uint8_t _KeyCode)
{
	s_tKey.Buf[s_tKey.Write] = _KeyCode;

	if (++s_tKey.Write >= KEY_FIFO_SIZE)
	{
		s_tKey.Write		= 0;
	}
}


uint8_t bsp_GetKey(void)
{
	uint8_t 		ret;

	if (s_tKey.Read == s_tKey.Write)
	{
		return KEY_NONE;
	}
	else 
	{
		ret 				= s_tKey.Buf[s_tKey.Read];

		if (++s_tKey.Read >= KEY_FIFO_SIZE)
		{
			s_tKey.Read 		= 0;
		}

		return ret;
	}
}


KEY_STATE_T bsp_GetKeyState(KEY_ID_T _ucKeyID)
{
	return s_tBtn[_ucKeyID].State;
}


void bsp_ClearKey(void)
{
	s_tKey.Read 		= s_tKey.Write;
}


static void bsp_InitKeyHard(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	//使能时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}


static void bsp_InitKeyVar(void)
{
	uint8_t 		i;

	s_tKey.Read 		= 0;
	s_tKey.Write		= 0;
	s_tKey.Read2		= 0;

	for (i = 0; i < KEY_COUNT; i++)
	{
		s_tBtn[i].Count 	= KEY_FILTER_TIME / 2;
		s_tBtn[i].State 	= KEY_NONE;
		s_tBtn[i].LongCount = 0;
		s_tBtn[i].DoubleCountTime = 0;
		s_tBtn[i].ShortPressFlag = 0;
		s_tBtn[i].DoublePressFlag = 0;
	}

	//按键按下判断函数
	s_tBtn[0].IsKeyDownFunc = IsKeyDown1;
}


static void bsp_DetectKey(uint8_t i)
{
	KEY_T * 		pBtn = NULL;

	pBtn				= &s_tBtn[i];
	pBtn->DoubleCountTime++;

	if (pBtn->IsKeyDownFunc())
	{
		if (pBtn->Count < KEY_FILTER_TIME)
		{
			pBtn->Count 		= KEY_FILTER_TIME;
		}
		else if (pBtn->Count < 2 * KEY_FILTER_TIME)
		{
			pBtn->Count++;
		}
		else 
		{
			pBtn->Count++;

			//发生短按事件
			if (pBtn->ShortPressFlag == 0)
			{
				pBtn->ShortPressFlag = 1;

				//pBtn->State = KEY_SHORTPRESS;
				pBtn->Count 		= 0;
			}

			if (pBtn->ShortPressFlag == 1)
			{
				//时间是否超过长按时间设置
				if (pBtn->Count > KEY_LONG_TIME)
				{
					//超过发生长按事件
					pBtn->State 		= KEY_LONGPRESS;
					pBtn->ShortPressFlag = 0;		//短按标志位清零
					pBtn->Count 		= 0;
					bsp_PutKey(4 * i + 4);

				}
			}

		}

	}
	else 
	{
		if ((pBtn->ShortPressFlag == 1) && (pBtn->State != KEY_LONGPRESS))
		{
			pBtn->ShortPressFlag = 0;

			if (pBtn->DoublePressFlag == 0)
			{
				//将双击标志位值1
				pBtn->DoublePressFlag = 1;
				pBtn->DoubleCountTime = 0;			//重置时间，用于测试是否双击

			}
			else 
			{
				//第一次短按发生后，在500ms时间内，发生第二次短按，完成一次双击
				if (pBtn->DoubleCountTime < 50)
				{
					pBtn->DoublePressFlag = 0;
					pBtn->State 		= KEY_DOUBLEPRESS;
					bsp_PutKey(4 * i + 5);
				}
			}
		}
		else 
		{
			if (pBtn->DoublePressFlag == 1)
			{

				if (pBtn->DoubleCountTime > 50)
				{
					pBtn->DoublePressFlag = 0;
					pBtn->State 		= KEY_SHORTPRESS;
					bsp_PutKey(4 * i + 3);
				}
			}

			pBtn->ShortPressFlag = 0;
			pBtn->State 		= KEY_NONE;
		}
	}
}


void bsp_KeyScan(void)
{
	uint8_t 		i;

	for (i = 0; i < KEY_COUNT; i++)
	{
		bsp_DetectKey(i);
	}
}


