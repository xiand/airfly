#ifndef __BSP_OLEDSHOW_H_
#define __BSP_OLEDSHOW_H_			  	 
#include "bsp.h"

  
 

//OLEDģʽ����
//0: 4�ߴ���ģʽ  ��ģ���BS1��BS2����GND��
//1: ����8080ģʽ ��ģ���BS1��BS2����VCC��
#define OLED_MODE 	0 

//�������������嶯��ÿ֡�ĳ���
#define MOVIE_XSIZE	128 
#define MOVIE_YSIZE 64  
#define FRAME_SIZE MOVIE_XSIZE*MOVIE_YSIZE/8  


//�˿ڶ���PA6 -- SCL PA7 - MOSI ʹ��ģ��SPIͨ��
//�˿ڶ���PC0 -- CS  PC1 - RST  PD2 -- RS
//-----------------OLED�˿ڶ���---------------- 
#define OLED_CS_RCC RCC_AHB1Periph_GPIOC
#define OLED_CS_PORT    GPIOC
#define OLED_CS_PIN      GPIO_Pin_0
#define OLED_CS_SET() GPIO_SetBits(OLED_CS_PORT, OLED_CS_PIN)
#define OLED_CS_CLR() GPIO_ResetBits(OLED_CS_PORT, OLED_CS_PIN)

#define OLED_RST_PORT GPIOC
#define OLED_RST_PIN GPIO_Pin_1
#define OLED_RST_SET() GPIO_SetBits(OLED_RST_PORT,OLED_RST_PIN)
#define OLED_RST_CLR() GPIO_ResetBits(OLED_RST_PORT,OLED_RST_PIN)
#define OLED_RST_RCC RCC_AHB1Periph_GPIOC

#define OLED_RS_PORT GPIOD
#define OLED_RS_PIN  GPIO_Pin_2
#define OLED_RS_SET() GPIO_SetBits(OLED_RS_PORT,OLED_RS_PIN)
#define OLED_RS_CLR() GPIO_ResetBits(OLED_RS_PORT,OLED_RS_PIN)
#define OLED_RS_RCC RCC_AHB1Periph_GPIOD

#define OLED_SCL_PORT GPIOA
#define OLED_SCL_PIN GPIO_Pin_6
#define OLED_SCL_SET() GPIO_SetBits(OLED_SCL_PORT,OLED_SCL_PIN)
#define OLED_SCL_CLR() GPIO_ResetBits(OLED_SCL_PORT,OLED_SCL_PIN)
#define OLED_SCL_RCC RCC_AHB1Periph_GPIOA

#define OLED_MOSI_PORT GPIOA
#define OLED_MOSI_PIN GPIO_Pin_7
#define OLED_MOSI_SET() GPIO_SetBits(OLED_MOSI_PORT,OLED_MOSI_PIN)
#define OLED_MOSI_CLR() GPIO_ResetBits(OLED_MOSI_PORT,OLED_MOSI_PIN)
#define OLED_MOSI_RCC RCC_AHB1Periph_GPIOA

//ԭ����
//#define OLED_CS 	PBout(7)
//#define OLED_RST  	PGout(15)	
//#define OLED_RS 	PDout(6)
//#define OLED_WR 	PAout(4)		  
//#define OLED_RD 	PDout(7)
 
//ʹ��4�ߴ��нӿ�ʱʹ�� 
//#define OLED_SCLK 	PCout(6)
//#define OLED_SDIN 	PCout(7)
		     
#define OLED_CMD  	0		//д����
#define OLED_DATA 	1		//д����
//OLED�����ú���
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size);	 
#endif  
	 



