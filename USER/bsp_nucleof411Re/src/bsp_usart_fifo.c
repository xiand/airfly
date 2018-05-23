
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
*函数功能：串口模块初始化
*************************************
*/
void bsp_InitUart(void)
{
	UartVarInit();									//全局变量初始化，初始化FIFO结构体等
	UartHardInit(); 								//初始化串口硬件
	ConfigUartNVIC();								//串口中断初始化
}


/*
***********************************
*函数功能：将COM端口编号转换为UART指针
*形参：ucPort：端口号 CM1-CM6
*返回值：对应的BSP_UART_T指针
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
*函数功能：向串口发送一组数据，数据发送到缓存区之后立即返回，
		   开启发送中断，数据发送由中断处理函数后台完成
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
		/* 如果是RS485通信，可以在这个函数中将RS485设置为发送模式 */
		pUart->SendBefore();						//数据发送前回调函数
	}

	UartSendData(pUart, pcBuf, usBufLen);

}


/*
*********************************************************************************************************
*	函 数 名: comSendChar
*	功能说明: 向串口发送1个字节。数据放到发送缓冲区后立即返回，由中断服务程序在后?
	?完成发送
*	形    参: _ucPort: 端口号(COM1 - COM6)
*			  _ucByte: 待发送的数据
*	返 回 值: 无
*********************************************************************************************************
*/
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
	comSendBuf(_ucPort, &_ucByte, 1);
}


/*
*********************************************************************************************************
*	函 数 名: comGetChar
*	功能说明: 从串口缓冲区读取1字节，非阻塞。无论有无数据均立即返回
*	形    参: _ucPort: 端口号(COM1 - COM6)
*			  _pByte: 接收到的数据存放在这个地址
*	返 回 值: 0 表示无数据, 1 表示读取到有效字节
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
*	函 数 名: comClearTxFifo
*	功能说明: 清零串口发送缓冲区
*	形    参: _ucPort: 端口号(COM1 - COM6)
*	返 回 值: 无
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
*	函 数 名: comClearRxFifo
*	功能说明: 清零串口接收缓冲区
*	形    参: _ucPort: 端口号(COM1 - COM6)
*	返 回 值: 无
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
*	函 数 名: UartVarInit
*	功能说明: 初始化串口相关的变量
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartVarInit(void)
{
#if USE_UART1_FIFO_EN				== 1
	g_tUart1.uart		= USART1;					/* STM32 串口设备 */
	g_tUart1.pTxBuf 	= g_TxBuf1; 				/* 发送缓冲区指针 */
	g_tUart1.pRxBuf 	= g_RxBuf1; 				/* 接收缓冲区指针 */
	g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;		/* 发送缓冲区大小 */
	g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;		/* 接收缓冲区大小 */
	g_tUart1.usTxWrite	= 0;						/* 发送FIFO写索引 */
	g_tUart1.usTxRead	= 0;						/* 发送FIFO读索引 */
	g_tUart1.usRxWrite	= 0;						/* 接收FIFO写索引 */
	g_tUart1.usRxRead	= 0;						/* 接收FIFO读索引 */
	g_tUart1.usRxCount	= 0;						/* 接收到的新数据个数 */
	g_tUart1.usTxCount	= 0;						/* 待发送的数据个数 */
	g_tUart1.SendBefore = NULL; 					/* 发送数据前的回调函数 */
	g_tUart1.SendOver	= NULL; 					/* 发送完毕后的回调函数 */
	g_tUart1.ReciveNew	= NULL; 					/* 接收到新数据后的回调函数 */
#endif

#if USE_UART2_FIFO_EN				== 1
	g_tUart2.uart		= USART2;					/* STM32 串口设备 */
	g_tUart2.pTxBuf 	= g_TxBuf2; 				/* 发送缓冲区指针 */
	g_tUart2.pRxBuf 	= g_RxBuf2; 				/* 接收缓冲区指针 */
	g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;		/* 发送缓冲区大小 */
	g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;		/* 接收缓冲区大小 */
	g_tUart2.usTxWrite	= 0;						/* 发送FIFO写索引 */
	g_tUart2.usTxRead	= 0;						/* 发送FIFO读索引 */
	g_tUart2.usRxWrite	= 0;						/* 接收FIFO写索引 */
	g_tUart2.usRxRead	= 0;						/* 接收FIFO读索引 */
	g_tUart2.usRxCount	= 0;						/* 接收到的新数据个数 */
	g_tUart2.usTxCount	= 0;						/* 待发送的数据个数 */
	g_tUart2.SendBefore = NULL; 					/* 发送数据前的回调函数 */
	g_tUart2.SendOver	= NULL; 					/* 发送完毕后的回调函数 */
	g_tUart2.ReciveNew	= NULL; 					/* 接收到新数据后的回调函数 */
#endif

#if USE_UART6_FIFO_EN				== 1
	g_tUart3.uart		= USART6;					/* STM32 串口设备 */
	g_tUart3.pTxBuf 	= g_TxBuf6; 				/* 发送缓冲区指针 */
	g_tUart3.pRxBuf 	= g_RxBuf6; 				/* 接收缓冲区指针 */
	g_tUart3.usTxBufSize = UART6_TX_BUF_SIZE;		/* 发送缓冲区大小 */
	g_tUart3.usRxBufSize = UART6_RX_BUF_SIZE;		/* 接收缓冲区大小 */
	g_tUart3.usTxWrite	= 0;						/* 发送FIFO写索引 */
	g_tUart3.usTxRead	= 0;						/* 发送FIFO读索引 */
	g_tUart3.usRxWrite	= 0;						/* 接收FIFO写索引 */
	g_tUart3.usRxRead	= 0;						/* 接收FIFO读索引 */
	g_tUart3.usRxCount	= 0;						/* 接收到的新数据个数 */
	g_tUart3.usTxCount	= 0;						/* 待发送的数据个数 */
	g_tUart3.SendBefore = NULL; 					/* 发送数据前的回调函数 */
	g_tUart3.SendOver	= NULL; 					/* 发送完毕后的回调函数 */
	g_tUart3.ReciveNew	= NULL; 					/* 接收到新数据后的回调函数 */
#endif
}


/*
***************************
*函数功能：串口硬件初始化,默认开启中断
***************************
*/
static void UartHardInit(void)
{

	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

#if USE_UART1_FIFO_EN				== 1

	//开启GPIO时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	//开启串口1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//将PA9映射为 USART1 TX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

	//将PA10映射为 USART1 RX
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
	USART_Init(USART1, &USART_InitStructure);		//串口配置参数初始化


	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//使能接收中断

	/*
	USART_ITConfig(USART2,USART_IT_TXE,ENABLE);
	注意，不要在这里打开发送中断
	发送中断在SendUart函数中处理
	*/
	USART_Cmd(USART1, ENABLE);						//所有设置完成之后，使能串口

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART1, USART_FLAG_TC); 		/* 清发送完成标志，Transmission Complete flag */
#endif

#if USE_UART2_FIFO_EN				== 1

	//使能时钟
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


	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//使能接收中断

	/*
	USART_ITConfig(USART2,USART_IT_TXE,ENABLE);
	注意，不要在这里打开发送中断
	发送中断在SendUart函数中处理
	*/
	USART_Cmd(USART2, ENABLE);						//所有设置完成之后，使能串口

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART2, USART_FLAG_TC); 		/* 清发送完成标志，Transmission Complete flag */
#endif

#if USE_UART6_FIFO_EN  == 1

	//开启GPIO时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	//开启串口1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

	//将PC6映射为 USART6 TX
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);

	//将PC7映射为 USART6 RX
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
	USART_Init(USART6, &USART_InitStructure);		//串口配置参数初始化


	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);	//使能接收中断

	/*
	USART_ITConfig(USART2,USART_IT_TXE,ENABLE);
	注意，不要在这里打开发送中断
	发送中断在SendUart函数中处理
	*/
	USART_Cmd(USART6, ENABLE);						//所有设置完成之后，使能串口

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART6, USART_FLAG_TC); 		/* 清发送完成标志，Transmission Complete flag */
#endif
}


/*
***********************************
* 功能说明：配置串口硬件中断
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
*说明：1、将数据填写到BSP_UART_T发送缓存区，启动发送中断
*******2、中断处理函数发送完成之后，自动关闭中断  --- 
*******3、这里使用的是FIFO先进先出，将数据存入到 缓存区中，那就相当于写数据，对于的
	是usTxWrite
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
*函数功能：从串口接收缓存区读取一个字节数据
		   1、理解什么是FIFO ，数据从队列中读出了，那队列的索引也是要跟着改变
*形参：pUart：串口设备 pByte：存放读取数据的指针
*返回值：0表示无数据，1表示读取到数据
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
		//表明缓存区中没有数据，直接返回
		return 0;
	}
	else 
	{
		*pByte				= pUart->pRxBuf[pUart->usRxRead];

		//读出一个数据之后，需要改写FIFO的读索引项
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
	//处理接收中断
	if (USART_GetITStatus(pUart->uart, USART_IT_RXNE) != RESET)
	{
		//将串口接收到的数据存放进对应的FIFO中
		pUart->pRxBuf[pUart->usRxWrite] = USART_ReceiveData(pUart->uart);

		if (++pUart->usRxWrite >= pUart->usRxBufSize)
		{
			pUart->usRxWrite	= 0;				//表明缓存区已经放不下这么多数据
		}

		if (pUart->usRxCount < pUart->usRxBufSize)
		{
			pUart->usRxCount++; 					//计数加一
		}

		//调用回调函数，通知应用程序接受到新的数据
		//一般用来发送一个消息作为标志
		if (pUart->usRxCount == 1)
		{
			if (pUart->ReciveNew != NULL)
			{
				pUart->ReciveNew();
			}
		}
	}

	//处理发送中断
	if (USART_GetITStatus(pUart->uart, USART_IT_TXE) != RESET)
	{
		if (pUart->usTxCount == 0) //通过usTxCount 判断是否发送完成
		{
			//表明发送缓存区找那个数据已经发送完毕，
			//禁止发送中断（注意此时还有最后一个数据未发送完成）
			USART_ITConfig(pUart->uart, USART_IT_TXE, DISABLE);

			//使能数据发送完毕中断
			USART_ITConfig(pUart->uart, USART_IT_TC, ENABLE);
		}
		else 
		{
			//从发送FIFO中取出一个字节写入到串口发送数据寄存器中
			USART_SendData(pUart->uart, pUart->pTxBuf[pUart->usTxRead]);

			if (++pUart->usTxRead >= pUart->usTxBufSize)
			{
				pUart->usTxRead 	= 0;			//发送去读，接受去写
			}

			pUart->usTxCount--;
		}
	}
	else if (USART_GetITStatus(pUart->uart, USART_IT_TC) != RESET)
	{
		if (pUart->usTxCount == 0) //进一步判断是否发送完成
		{
			//如果已经全部发送完成，禁止数据发送完毕中断
			USART_ITConfig(pUart->uart, USART_IT_TC, DISABLE);

			/* 回调函数, 一般用来处理RS485通信，将RS485芯片设置为接收模式，避免抢占总线 
				*/
			if (pUart->SendOver != NULL)
			{
				pUart->SendOver();
			}
		}
		else 
		{
			//正常情况下，不会进入该else中，其表示数据还未发送完，但是确进入了发送完成中断
			//继续发送
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
*	函 数 名: USART1_IRQHandler	USART2_IRQHandler USART6_IRQHandler 
*	功能说明: USART中断服务程序
*	形    参: 无
*	返 回 值: 无
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
*	函 数 名: fputc
*	功能说明: 重定义putc函数，这样可以使用printf函数从串口1打印输出
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fputc(int ch, FILE * f)
{
#if 1 /* 将需要printf的字符通过串口中断FIFO发送出去，printf函数会立即返回 */
	comSendChar(COM2, ch);

	return ch;

#else /* 采用阻塞方式发送每个字符,等待数据发送完毕 */

	/* 写一个字节到USART1 */
	USART_SendData(USART2, (uint8_t) ch);

	/* 等待发送结束 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
	}

	return ch;

#endif
}


/*
*********************************************************************************************************
*	函 数 名: fgetc
*	功能说明: 重定义getc函数，这样可以使用getchar函数从串口1输入数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fgetc(FILE * f)
{

#if 1 /* 从串口接收FIFO中取1个数据, 只有取到数据才返回 */
	uint8_t 		ucData;

	while (comGetChar(COM2, &ucData) == 0)
		;

	return ucData;

#else

	/* 等待串口1输入数据 */
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
	//使能引脚时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	//引脚的重定向
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

	USART_Cmd(IAP_UART, ENABLE);						//所有设置完成之后，使能串口

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(IAP_UART, USART_FLAG_TC); 		/* 清发送完成标志，Transmission Complete flag */
}


