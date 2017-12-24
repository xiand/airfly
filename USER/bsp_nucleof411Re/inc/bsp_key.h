#ifndef __BSP_KEY_H_
#define __BSP_KEY_H_

#include "stm32f4xx.h"
#if 0
#include "stm32f4xx.h"
#include <stm32f4xx_exti.h>

#define KEY GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)
#define BSP_KEY_PIN GPIO_Pin_13
#define BSP_KEY_PORT GPIOC

void KEY_Init(void);

void KEY_Exit_Init(void);

#endif

#define BSP_KEY_PIN GPIO_Pin_13
#define BSP_KEY_PORT GPIOC

#define KEY_COUNT 1 //假设按键数量为10个，而f411re为1个 
 
//根据应用程序需要功能命名按键宏
#define KEY_DOWN_K1 KEY_1_DOWN   //按键一 按下
#define KEY_UP_K1 KEY_1_UP  // 按键一弹起
#define KEY_SHORTPRESS_K1 KEY_1_SHORTPRESS  //按键1短按
#define KEY_LONGPRESS_K1  KEY_1_LONGPRESS   //按键1长按
#define KEY_DOUBLEPRESS_K1 KEY_1_DOUBLEPRESS   //按键1双击

#define KEY_DOWN_K2 KEY_2_DOWN   //按键2 按下
#define KEY_UP_K2 KEY_2_UP  // 按键2弹起
#define KEY_SHORTPRESS_K2 KEY_2_SHORTPRESS  //按键2短按
#define KEY_LONGPRESS_K2  KEY_2_LONGPRESS   //按键2长按
#define KEY_DOUBLEPRESS_K2 KEY_2_DOUBLEPRESS   //按键2双击

#define KEY_DOWN_K3 KEY_3_DOWN   //按键3 按下
#define KEY_UP_K3 KEY_3_UP  // 按键3弹起
#define KEY_SHORTPRESS_K3 KEY_3_SHORTPRESS  //按键3短按
#define KEY_LONGPRESS_K3  KEY_3_LONGPRESS   //按键3长按
#define KEY_DOUBLEPRESS_K3 KEY_3_DOUBLEPRESS   //按键3双击

typedef enum
{
	KID_K1 = 0,
	KID_K2,
	KID_K3
}KEY_ID_T;

typedef enum
{
	KEY_NONE = 0,
	KEY_DOWN ,
	KEY_UP ,
	KEY_SHORTPRESS,
	KEY_LONGPRESS,
	KEY_DOUBLEPRESS
}KEY_STATE_T;

/*
	按键滤波时间50ms，单位为10ms
	只有连续检测到50ms状态不变才认为有效
	即使按键电路不做硬件滤波，该机制也可以
	有效保证可靠检测按键事件
*/
#define KEY_FILTER_TIME 5
#define KEY_LONG_TIME 100   //单位10ms，持续1s，认为是长按事件

typedef struct
{
	uint8_t (*IsKeyDownFunc)(void);  //按键按下判断函数
	KEY_STATE_T State;  //按键当前状态
	uint8_t Count;  //滤波器计数器
	uint8_t ShortPressFlag;
	uint8_t DoublePressFlag;
	uint16_t LongCount;  //长按计数器
	uint32_t DoubleCountTime;
}KEY_T;

#define KEY_FIFO_SIZE 10
typedef struct
{
	uint8_t Buf[KEY_FIFO_SIZE];
	uint8_t Read;
	uint8_t Write;
	uint8_t Read2;
}KEY_FIFO_T;

typedef enum
{
	KEY_N = 0,
	KEY_1_DOWN,
	KEY_1_UP,
	KEY_1_SHORTPRESS,
	KEY_1_LONGPRESS,
	KEY_1_DOUBLEPRESS,

	KEY_2_DOWN,
	KEY_2_UP,
	KEY_2_SHORTPRESS,
	KEY_2_LONGPRESS,
	KEY_2_DOUBLEPRESS,
}KEY_ENUM;


//供外部调用的函数
void bsp_InitKey(void);
void bsp_KeyScan(void);
void bsp_PutKey(uint8_t _KeyCode);
uint8_t bsp_GetKey(void);
//uint8_t bsp_GetKey2(void);
uint8_t bsp_GetKeyState(KEY_ID_T _ucKeyID);
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed);
void bsp_ClearKey(void);


#endif
