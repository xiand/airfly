#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_


#include "stm32f4xx.h"
#include <stdio.h>

#define USARTn 3   //411RE一共有三个串口
#define	USE_UART1_FIFO_EN	0
#define	USE_UART2_FIFO_EN	1
#define	USE_UART6_FIFO_EN	0

typedef enum BSP_USART{
	BSP_USART1 = 1,
	BSP_USART2 = 2,
	BSP_USART6 = 6
}BSP_Usart_Typedef;

typedef enum Baudrate{
	BAUDRATE_9600 = 9600,
	BAUDRATE_38400 = 38400,
	BAUDRATE_115200 = 115200,
	BAUDRATE_921600 = 921600
}BSP_Usart_Baudrate;

#define BSP_USART1  USART1
#define BSP_USART1_RX_PORT GPIOA
#define BSP_USART1_RX_PIN	 GPIO_Pin_10
#define BSP_USART1_TX_PORT	GPIOA   
#define BSP_USART1_TX_PIN  GPIO_Pin_9

#define BSP_USART2 USART2 
#define BSP_USART2_RX_PORT GPIOA
#define BSP_USART2_RX_PIN  GPIO_Pin_3
#define BSP_USART2_TX_PORT GPIOA
#define BSP_USART2_TX_PIN  GPIO_Pin_2

#define BSP_USART3  USART6
#define BSP_USART3_RX_PORT GPIOC
#define BSP_USART3_RX_PIN GPIO_Pin_7
#define BSP_USART3_TX_PORT GPIOC
#define BSP_USART3_TX_PIN	GPIO_Pin_6

#define IAP_UART USART2
#define IAP_UART_RX_PORT GPIOA
#define IAP_UART_RX_PIN  GPIO_Pin_3
#define IAP_UART_TX_PORT GPIOA
#define IAP_UART_TX_PIN  GPIO_Pin_2
#define EVAL_COM1 USART2
/* 定义端口号 */
typedef enum
{
	COM1 = 1,	/* USART1  PA9, PA10 或  PB6, PB7*/
	COM2 = 2,	/* USART2, PD5,PD6 或 PA2, PA3 */
	
	COM6 = 6	/* USART6, PC6, PC7 */
}COM_PORT_E;

/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */
#if USE_UART1_FIFO_EN == 1
	#define UART1_BAUD			115200
	#define UART1_TX_BUF_SIZE	1*1024
	#define UART1_RX_BUF_SIZE	1*1024
#endif

#if USE_UART2_FIFO_EN == 1
	#define UART2_BAUD			115200
	#define UART2_TX_BUF_SIZE	1
	#define UART2_RX_BUF_SIZE	2*1024
#endif

#if USE_UART6_FIFO_EN == 1
	#define UART6_BAUD			115200
	#define UART6_TX_BUF_SIZE	1*1024
	#define UART6_RX_BUF_SIZE	1*1024
#endif

/* 串口设备结构体 */
typedef struct
{
	USART_TypeDef *uart;		/* STM32内部串口设备指针 */
	uint8_t *pTxBuf;			/* 发送缓冲区 */
	uint8_t *pRxBuf;			/* 接收缓冲区 */
	uint16_t usTxBufSize;		/* 发送缓冲区大小 */
	uint16_t usRxBufSize;		/* 接收缓冲区大小 */
	uint16_t usTxWrite;			/* 发送缓冲区写指针 */
	uint16_t usTxRead;			/* 发送缓冲区读指针 */
	uint16_t usTxCount;			/* 等待发送的数据个数 */

	uint16_t usRxWrite;			/* 接收缓冲区写指针 */
	uint16_t usRxRead;			/* 接收缓冲区读指针 */
	uint16_t usRxCount;			/* 还未读取的新数据个数 */

	void (*SendBefore)(void); 	/* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
	void (*SendOver)(void); 	/* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
	void (*ReciveNew)(void);	/* 串口收到数据的回调函数指针 */
}BSP_UART_T;

void bsp_InitUart(void);
void comSendBuf(COM_PORT_E ucPort,uint8_t *pcBuf,uint16_t usBufLen);
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E ucPort, uint8_t *pByte);

void comClearTxFifo(COM_PORT_E ucPort);
void comClearRxFifo(COM_PORT_E ucPort);

//该函数用于串口的IAP操作
void bsp_iap_uart_init(void);

#endif
