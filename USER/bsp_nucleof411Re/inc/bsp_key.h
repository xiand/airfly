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

#define KEY_COUNT 1 //���谴������Ϊ10������f411reΪ1�� 
 
//����Ӧ�ó�����Ҫ��������������
#define KEY_DOWN_K1 KEY_1_DOWN   //����һ ����
#define KEY_UP_K1 KEY_1_UP  // ����һ����
#define KEY_SHORTPRESS_K1 KEY_1_SHORTPRESS  //����1�̰�
#define KEY_LONGPRESS_K1  KEY_1_LONGPRESS   //����1����
#define KEY_DOUBLEPRESS_K1 KEY_1_DOUBLEPRESS   //����1˫��

#define KEY_DOWN_K2 KEY_2_DOWN   //����2 ����
#define KEY_UP_K2 KEY_2_UP  // ����2����
#define KEY_SHORTPRESS_K2 KEY_2_SHORTPRESS  //����2�̰�
#define KEY_LONGPRESS_K2  KEY_2_LONGPRESS   //����2����
#define KEY_DOUBLEPRESS_K2 KEY_2_DOUBLEPRESS   //����2˫��

#define KEY_DOWN_K3 KEY_3_DOWN   //����3 ����
#define KEY_UP_K3 KEY_3_UP  // ����3����
#define KEY_SHORTPRESS_K3 KEY_3_SHORTPRESS  //����3�̰�
#define KEY_LONGPRESS_K3  KEY_3_LONGPRESS   //����3����
#define KEY_DOUBLEPRESS_K3 KEY_3_DOUBLEPRESS   //����3˫��

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
	�����˲�ʱ��50ms����λΪ10ms
	ֻ��������⵽50ms״̬�������Ϊ��Ч
	��ʹ������·����Ӳ���˲����û���Ҳ����
	��Ч��֤�ɿ���ⰴ���¼�
*/
#define KEY_FILTER_TIME 5
#define KEY_LONG_TIME 100   //��λ10ms������1s����Ϊ�ǳ����¼�

typedef struct
{
	uint8_t (*IsKeyDownFunc)(void);  //���������жϺ���
	KEY_STATE_T State;  //������ǰ״̬
	uint8_t Count;  //�˲���������
	uint8_t ShortPressFlag;
	uint8_t DoublePressFlag;
	uint16_t LongCount;  //����������
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


//���ⲿ���õĺ���
void bsp_InitKey(void);
void bsp_KeyScan(void);
void bsp_PutKey(uint8_t _KeyCode);
uint8_t bsp_GetKey(void);
//uint8_t bsp_GetKey2(void);
uint8_t bsp_GetKeyState(KEY_ID_T _ucKeyID);
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed);
void bsp_ClearKey(void);


#endif
