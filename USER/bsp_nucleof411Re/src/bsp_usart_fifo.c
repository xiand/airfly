
#include "bsp.h"

USART_TypeDef * BSP_USART[USARTn] =
{
	BSP_USART1, BSP_USART2, BSP_USART3
};


GPIO_TypeDef *	BSP_USART_TX_PORT[USARTn] =
{
	BSP_USART1_TX_PORT, BSP_USART2_TX_PORT, BSP_USART3_TX_PORT
};


GPIO_TypeDef *	BSP_USART_RX_PORT[USARTn] =
{
	BSP_USART1_RX_PORT, BSP_USART2_RX_PORT, BSP_USART3_RX_PORT
};


uint16_t		BSP_USART_TX_PIN[USARTn] =
{
	BSP_USART1_TX_PIN, BSP_USART2_TX_PIN, BSP_USART3_TX_PIN
};


uint16_t		BSP_USART_RX_PIN[USARTn] =
{
	BSP_USART1_RX_PIN, BSP_USART2_RX_PIN, BSP_USART3_RX_PIN
};


#if USE_UART1_FIFO_EN			== 1
static BSP_UART_T g_tUart1;
static uint8_t	g_TxBuf1[UART1_TX_BUF_SIZE];
static uint8_t	g_RxBuf1[UART1_RX_BUF_SIZE];

#endif

#if USE_UART2_FIFO_EN			== 1
static BSP_UART_T g_tUart2;
static uint8_t	g_TxBuf2[UART2_TX_BUF_SIZE];
static uint8_t	g_RxBuf2[UART2_RX_BUF_SIZE];

#endif

#if USE_UART6_FIFO_EN			== 1
static BSP_UART_T g_tUart6;
static uint8_t	g_TxBuf6[UART6_TX_BUF_SIZE];
static uint8_t	g_RxBuf6[UART6_RX_BUF_SIZE];

#endif

static void UartVarInit(void);
static void UartHardInit(void);
static void ConfigUartNVIC(void);
static void UartSendData(BSP_UART_T * pUart, uint8_t * ucBuf, uint16_t usLen);
static uint8_t UartGetChar(BSP_UART_T * pUart, uint8_t * pByte);




/*
**************************************
*�������ܣ�����ģ���ʼ��
*************************************
*/
void bsp_InitUart(void)
{
	UartVarInit();									//ȫ�ֱ�����ʼ������ʼ��FIFO�ṹ���
	UartHardInit(); 								//��ʼ������Ӳ��
	ConfigUartNVIC();								//�����жϳ�ʼ��
}


/*
***********************************
*�������ܣ���COM�˿ڱ��ת��ΪUARTָ��
*�βΣ�ucPort���˿ں� CM1-CM6
*����ֵ����Ӧ��BSP_UART_Tָ��
**********************************
*/
BSP_UART_T * ComToUart(COM_PORT_E ucPort)
{
	if (ucPort == COM1)
	{
#if USE_UART1_FIFO_EN					== 1
		return & g_tUart1;

#else

		return NULL;

#endif
	}
	else if (ucPort == COM2)
	{
#if USE_UART2_FIFO_EN					== 1
		return & g_tUart2;

#else

		return NULL;

#endif
	}
	else if (ucPort == COM6)
	{
#if USE_UART6_FIFO_EN					== 1
		return & g_tUart6;

#else

		return NULL;

#endif
	}
	else 
	{
		return NULL;
	}

}


/*
*****************************************
*�������ܣ��򴮿ڷ���һ�����ݣ����ݷ��͵�������֮���������أ�
		   ���������жϣ����ݷ������жϴ�������̨���
*****************************************
*/
void comSendBuf(COM_PORT_E ucPort, uint8_t * pcBuf, uint16_t usBufLen)
{
	BSP_UART_T *	pUart = NULL;

	pUart				= ComToUart(ucPort);

	if (NULL == pUart)
	{
		return;
	}

	if (pUart->SendBefore != NULL)
	{
		/* �����RS485ͨ�ţ���������������н�RS485����Ϊ����ģʽ */
		pUart->SendBefore();						//���ݷ���ǰ�ص�����
	}

	UartSendData(pUart, pcBuf, usBufLen);

}


/*
*********************************************************************************************************
*	�� �� ��: comSendChar
*	����˵��: �򴮿ڷ���1���ֽڡ����ݷŵ����ͻ��������������أ����жϷ�������ں�?
	?��ɷ���
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*			  _ucByte: �����͵�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
	comSendBuf(_ucPort, &_ucByte, 1);
}


/*
*********************************************************************************************************
*	�� �� ��: comGetChar
*	����˵��: �Ӵ��ڻ�������ȡ1�ֽڣ��������������������ݾ���������
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*			  _pByte: ���յ������ݴ���������ַ
*	�� �� ֵ: 0 ��ʾ������, 1 ��ʾ��ȡ����Ч�ֽ�
*********************************************************************************************************
*/
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t * _pByte)
{
	BSP_UART_T *	pUart;

	pUart				= ComToUart(_ucPort);

	if (pUart == 0)
	{
		return 0;
	}

	return UartGetChar(pUart, _pByte);
}


/*
*********************************************************************************************************
*	�� �� ��: comClearTxFifo
*	����˵��: ���㴮�ڷ��ͻ�����
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comClearTxFifo(COM_PORT_E _ucPort)
{
	BSP_UART_T *	pUart;

	pUart				= ComToUart(_ucPort);

	if (pUart == 0)
	{
		return;
	}

	pUart->usTxWrite	= 0;
	pUart->usTxRead 	= 0;
	pUart->usTxCount	= 0;
}


/*
*********************************************************************************************************
*	�� �� ��: comClearRxFifo
*	����˵��: ���㴮�ڽ��ջ�����
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comClearRxFifo(COM_PORT_E _ucPort)
{
	BSP_UART_T *	pUart;

	pUart				= ComToUart(_ucPort);

	if (pUart == 0)
	{
		return;
	}

	pUart->usRxWrite	= 0;
	pUart->usRxRead 	= 0;
	pUart->usRxCount	= 0;
}


/*
*********************************************************************************************************
*	�� �� ��: UartVarInit
*	����˵��: ��ʼ��������صı���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartVarInit(void)
{
#if USE_UART1_FIFO_EN				== 1
	g_tUart1.uart		= USART1;					/* STM32 �����豸 */
	g_tUart1.pTxBuf 	= g_TxBuf1; 				/* ���ͻ�����ָ�� */
	g_tUart1.pRxBuf 	= g_RxBuf1; 				/* ���ջ�����ָ�� */
	g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;		/* ���ͻ�������С */
	g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;		/* ���ջ�������С */
	g_tUart1.usTxWrite	= 0;						/* ����FIFOд���� */
	g_tUart1.usTxRead	= 0;						/* ����FIFO������ */
	g_tUart1.usRxWrite	= 0;						/* ����FIFOд���� */
	g_tUart1.usRxRead	= 0;						/* ����FIFO������ */
	g_tUart1.usRxCount	= 0;						/* ���յ��������ݸ��� */
	g_tUart1.usTxCount	= 0;						/* �����͵����ݸ��� */
	g_tUart1.SendBefore = NULL; 					/* ��������ǰ�Ļص����� */
	g_tUart1.SendOver	= NULL; 					/* ������Ϻ�Ļص����� */
	g_tUart1.ReciveNew	= NULL; 					/* ���յ������ݺ�Ļص����� */
#endif

#if USE_UART2_FIFO_EN				== 1
	g_tUart2.uart		= USART2;					/* STM32 �����豸 */
	g_tUart2.pTxBuf 	= g_TxBuf2; 				/* ���ͻ�����ָ�� */
	g_tUart2.pRxBuf 	= g_RxBuf2; 				/* ���ջ�����ָ�� */
	g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;		/* ���ͻ�������С */
	g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;		/* ���ջ�������С */
	g_tUart2.usTxWrite	= 0;						/* ����FIFOд���� */
	g_tUart2.usTxRead	= 0;						/* ����FIFO������ */
	g_tUart2.usRxWrite	= 0;						/* ����FIFOд���� */
	g_tUart2.usRxRead	= 0;						/* ����FIFO������ */
	g_tUart2.usRxCount	= 0;						/* ���յ��������ݸ��� */
	g_tUart2.usTxCount	= 0;						/* �����͵����ݸ��� */
	g_tUart2.SendBefore = NULL; 					/* ��������ǰ�Ļص����� */
	g_tUart2.SendOver	= NULL; 					/* ������Ϻ�Ļص����� */
	g_tUart2.ReciveNew	= NULL; 					/* ���յ������ݺ�Ļص����� */
#endif

#if USE_UART6_FIFO_EN				== 1
	g_tUart3.uart		= USART6;					/* STM32 �����豸 */
	g_tUart3.pTxBuf 	= g_TxBuf6; 				/* ���ͻ�����ָ�� */
	g_tUart3.pRxBuf 	= g_RxBuf6; 				/* ���ջ�����ָ�� */
	g_tUart3.usTxBufSize = UART6_TX_BUF_SIZE;		/* ���ͻ�������С */
	g_tUart3.usRxBufSize = UART6_RX_BUF_SIZE;		/* ���ջ�������С */
	g_tUart3.usTxWrite	= 0;						/* ����FIFOд���� */
	g_tUart3.usTxRead	= 0;						/* ����FIFO������ */
	g_tUart3.usRxWrite	= 0;						/* ����FIFOд���� */
	g_tUart3.usRxRead	= 0;						/* ����FIFO������ */
	g_tUart3.usRxCount	= 0;						/* ���յ��������ݸ��� */
	g_tUart3.usTxCount	= 0;						/* �����͵����ݸ��� */
	g_tUart3.SendBefore = NULL; 					/* ��������ǰ�Ļص����� */
	g_tUart3.SendOver	= NULL; 					/* ������Ϻ�Ļص����� */
	g_tUart3.ReciveNew	= NULL; 					/* ���յ������ݺ�Ļص����� */
#endif
}


/*
***************************
*�������ܣ�����Ӳ����ʼ��,Ĭ�Ͽ����ж�
***************************
*/
static void UartHardInit(void)
{

	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

#if USE_UART1_FIFO_EN				== 1

	//����GPIOʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	//��������1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//��PA9ӳ��Ϊ USART1 TX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

	//��PA10ӳ��Ϊ USART1 RX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = UART1_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);		//�������ò�����ʼ��


	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//ʹ�ܽ����ж�

	/*
	USART_ITConfig(USART2,USART_IT_TXE,ENABLE);
	ע�⣬��Ҫ������򿪷����ж�
	�����ж���SendUart�����д���
	*/
	USART_Cmd(USART1, ENABLE);						//�����������֮��ʹ�ܴ���

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART1, USART_FLAG_TC); 		/* �巢����ɱ�־��Transmission Complete flag */
#endif

#if USE_UART2_FIFO_EN				== 1

	//ʹ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = UART2_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);


	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//ʹ�ܽ����ж�

	/*
	USART_ITConfig(USART2,USART_IT_TXE,ENABLE);
	ע�⣬��Ҫ������򿪷����ж�
	�����ж���SendUart�����д���
	*/
	USART_Cmd(USART2, ENABLE);						//�����������֮��ʹ�ܴ���

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART2, USART_FLAG_TC); 		/* �巢����ɱ�־��Transmission Complete flag */
#endif

#if USE_UART6_FIFO_EN  == 1

	//����GPIOʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	//��������1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

	//��PC6ӳ��Ϊ USART6 TX
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);

	//��PC7ӳ��Ϊ USART6 RX
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = UART6_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART6, &USART_InitStructure);		//�������ò�����ʼ��


	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);	//ʹ�ܽ����ж�

	/*
	USART_ITConfig(USART2,USART_IT_TXE,ENABLE);
	ע�⣬��Ҫ������򿪷����ж�
	�����ж���SendUart�����д���
	*/
	USART_Cmd(USART6, ENABLE);						//�����������֮��ʹ�ܴ���

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART6, USART_FLAG_TC); 		/* �巢����ɱ�־��Transmission Complete flag */
#endif
}


/*
***********************************
* ����˵�������ô���Ӳ���ж�
***********************************
*/
static void ConfigUartNVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;

	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

#if USE_UART1_FIFO_EN				== 1
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

#endif

#if USE_UART2_FIFO_EN				== 1
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

#endif

#if USE_UART6_FIFO_EN				== 1
	NVIC_InitStruct.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

#endif

}


/*
************************************************
*˵����1����������д��BSP_UART_T���ͻ����������������ж�
*******2���жϴ������������֮���Զ��ر��ж�  --- 
*******3������ʹ�õ���FIFO�Ƚ��ȳ��������ݴ��뵽 �������У��Ǿ��൱��д���ݣ����ڵ�
	��usTxWrite
************************************************
*/
static void UartSendData(BSP_UART_T * pUart, uint8_t * ucBuf, uint16_t usLen)
{
	uint16_t		i	= 0;
	uint16_t		usCount = 0;

	for (i = 0; i < usLen; i++)
	{
		while (1)
		{
			DISABLE_INT();
			usCount 			= pUart->usTxCount;
			ENABLE_INT();

			if (usCount < pUart->usTxBufSize)
			{
				break;
			}
		}

		pUart->pTxBuf[pUart->usTxWrite] = ucBuf[i];

		DISABLE_INT();

		if (++pUart->usTxWrite >= pUart->usTxBufSize)
		{
			pUart->usTxWrite	= 0;
		}

		pUart->usTxCount++;
		ENABLE_INT();

	}

	USART_ITConfig(pUart->uart, USART_IT_TXE, ENABLE);
}


/*
*************************************
*�������ܣ��Ӵ��ڽ��ջ�������ȡһ���ֽ�����
		   1�����ʲô��FIFO �����ݴӶ����ж����ˣ��Ƕ��е�����Ҳ��Ҫ���Ÿı�
*�βΣ�pUart�������豸 pByte����Ŷ�ȡ���ݵ�ָ��
*����ֵ��0��ʾ�����ݣ�1��ʾ��ȡ������
*************************************
*/
static uint8_t UartGetChar(BSP_UART_T * pUart, uint8_t * pByte)
{
	uint16_t		usCount;

	DISABLE_INT();
	usCount 			= pUart->usRxCount;
	ENABLE_INT();


	if (usCount == 0)
	{
		//������������û�����ݣ�ֱ�ӷ���
		return 0;
	}
	else 
	{
		*pByte				= pUart->pRxBuf[pUart->usRxRead];

		//����һ������֮����Ҫ��дFIFO�Ķ�������
		DISABLE_INT();

		if (++pUart->usRxRead >= pUart->usRxBufSize)
		{
			pUart->usRxRead 	= 0;
		}

		pUart->usRxCount--;
		ENABLE_INT();
		return 1;
	}
}


static void UartDealIRQ(BSP_UART_T * pUart)
{
	//��������ж�
	if (USART_GetITStatus(pUart->uart, USART_IT_RXNE) != RESET)
	{
		//�����ڽ��յ������ݴ�Ž���Ӧ��FIFO��
		pUart->pRxBuf[pUart->usRxWrite] = USART_ReceiveData(pUart->uart);

		if (++pUart->usRxWrite >= pUart->usRxBufSize)
		{
			pUart->usRxWrite	= 0;				//�����������Ѿ��Ų�����ô������
		}

		if (pUart->usRxCount < pUart->usRxBufSize)
		{
			pUart->usRxCount++; 					//������һ
		}

		//���ûص�������֪ͨӦ�ó�����ܵ��µ�����
		//һ����������һ����Ϣ��Ϊ��־
		if (pUart->usRxCount == 1)
		{
			if (pUart->ReciveNew != NULL)
			{
				pUart->ReciveNew();
			}
		}
	}

	//�������ж�
	if (USART_GetITStatus(pUart->uart, USART_IT_TXE) != RESET)
	{
		if (pUart->usTxCount == 0) //ͨ��usTxCount �ж��Ƿ������
		{
			//�������ͻ��������Ǹ������Ѿ�������ϣ�
			//��ֹ�����жϣ�ע���ʱ�������һ������δ������ɣ�
			USART_ITConfig(pUart->uart, USART_IT_TXE, DISABLE);

			//ʹ�����ݷ�������ж�
			USART_ITConfig(pUart->uart, USART_IT_TC, ENABLE);
		}
		else 
		{
			//�ӷ���FIFO��ȡ��һ���ֽ�д�뵽���ڷ������ݼĴ�����
			USART_SendData(pUart->uart, pUart->pTxBuf[pUart->usTxRead]);

			if (++pUart->usTxRead >= pUart->usTxBufSize)
			{
				pUart->usTxRead 	= 0;			//����ȥ��������ȥд
			}

			pUart->usTxCount--;
		}
	}
	else if (USART_GetITStatus(pUart->uart, USART_IT_TC) != RESET)
	{
		if (pUart->usTxCount == 0) //��һ���ж��Ƿ������
		{
			//����Ѿ�ȫ��������ɣ���ֹ���ݷ�������ж�
			USART_ITConfig(pUart->uart, USART_IT_TC, DISABLE);

			/* �ص�����, һ����������RS485ͨ�ţ���RS485оƬ����Ϊ����ģʽ��������ռ���� 
				*/
			if (pUart->SendOver != NULL)
			{
				pUart->SendOver();
			}
		}
		else 
		{
			//��������£���������else�У����ʾ���ݻ�δ�����꣬����ȷ�����˷�������ж�
			//��������
			USART_SendData(pUart->uart, pUart->pTxBuf[pUart->usTxRead]);

			if (++pUart->usTxRead >= pUart->usTxBufSize)
			{
				pUart->usTxRead 	= 0;
			}

			pUart->usTxCount--;

		}
	}
}


/*
*********************************************************************************************************
*	�� �� ��: USART1_IRQHandler	USART2_IRQHandler USART6_IRQHandler 
*	����˵��: USART�жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if USE_UART1_FIFO_EN			== 1


void USART1_IRQHandler(void)
{
	UartDealIRQ(&g_tUart1);
}


#endif

#if USE_UART2_FIFO_EN			== 1


void USART2_IRQHandler(void)
{
	UartDealIRQ(&g_tUart2);
}


#endif

#if USE_UART6_FIFO_EN			== 1


void USART6_IRQHandler(void)
{
	UartDealIRQ(&g_tUart6);
}


#endif


/*
*********************************************************************************************************
*	�� �� ��: fputc
*	����˵��: �ض���putc��������������ʹ��printf�����Ӵ���1��ӡ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int fputc(int ch, FILE * f)
{
#if 1 /* ����Ҫprintf���ַ�ͨ�������ж�FIFO���ͳ�ȥ��printf�������������� */
	comSendChar(COM2, ch);

	return ch;

#else /* ����������ʽ����ÿ���ַ�,�ȴ����ݷ������ */

	/* дһ���ֽڵ�USART1 */
	USART_SendData(USART2, (uint8_t) ch);

	/* �ȴ����ͽ��� */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
	}

	return ch;

#endif
}


/*
*********************************************************************************************************
*	�� �� ��: fgetc
*	����˵��: �ض���getc��������������ʹ��getchar�����Ӵ���1��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int fgetc(FILE * f)
{

#if 1 /* �Ӵ��ڽ���FIFO��ȡ1������, ֻ��ȡ�����ݲŷ��� */
	uint8_t 		ucData;

	while (comGetChar(COM2, &ucData) == 0)
		;

	return ucData;

#else

	/* �ȴ�����1�������� */
	while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
		;

	return (int)
	USART_ReceiveData(USART1);

#endif
}

void bsp_iap_uart_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//ʹ������ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	//���ŵ��ض���
	GPIO_PinAFConfig(IAP_UART_RX_PORT, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(IAP_UART_TX_PORT, GPIO_PinSource3, GPIO_AF_USART2);
	
	GPIO_InitStructure.GPIO_Pin = IAP_UART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(IAP_UART_TX_PORT,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IAP_UART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(IAP_UART_RX_PORT,&GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(IAP_UART,&USART_InitStructure);

	USART_Cmd(IAP_UART, ENABLE);						//�����������֮��ʹ�ܴ���

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(IAP_UART, USART_FLAG_TC); 		/* �巢����ɱ�־��Transmission Complete flag */
}


