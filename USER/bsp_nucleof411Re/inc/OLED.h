#ifndef  __OLED_H
#define  __OLED_H
#include "bsp.h"

#if 0
//#define OLED_RS PCout(8)
#define OLED_RS_PORT              GPIOC
#define OLED_RS_PIN               GPIO_Pin_8
#define Set_OLED_RS  {GPIO_SetBits(OLED_RS_PORT,OLED_RS_PIN);}
#define Clr_OLED_RS  {GPIO_ResetBits(OLED_RS_PORT,OLED_RS_PIN);}

//#define OLED_CS PCout(9)
#define OLED_CS_PORT              GPIOC
#define OLED_CS_PIN               GPIO_Pin_9
#define Set_OLED_CS  {GPIO_SetBits(OLED_CS_PORT,OLED_CS_PIN);}
#define Clr_OLED_CS  {GPIO_ResetBits(OLED_CS_PORT,OLED_CS_PIN);} 

//#define OLED_RST  PBout(14)//在MINISTM32上直接接到了STM32的复位脚！
#define OLED_RST_PORT              GPIOC
#define OLED_RST_PIN               GPIO_Pin_6
#define Set_OLED_RST  {GPIO_SetBits(OLED_RST_PORT,OLED_RST_PIN);}
#define Clr_OLED_RST  {GPIO_ResetBits(OLED_RST_PORT,OLED_RST_PIN);}
#endif

//端口定义PA6 -- SCL PA7 - MOSI 使用模拟SPI通信
//端口定义PC0 -- CS  PC1 - RST  PD2 -- RS
//-----------------OLED端口定义---------------- 
#define OLED_CS_RCC RCC_AHB1Periph_GPIOC
#define OLED_CS_PORT GPIOC
#define OLED_CS_PIN GPIO_Pin_0
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


//这两个参数定义动画每帧的长宽。
#define MOVIE_XSIZE	128 
#define MOVIE_YSIZE 64  
#define FRAME_SIZE MOVIE_XSIZE*MOVIE_YSIZE/8  

u8 SPI1_ReadWriteByte(u8 TxData);
void  SPI1_Initaize(void);
void  Write_com(u8  com);
void  Write_data(u8 dat);
void OLED_Init(void);
void OLED_Refresh_Gram(void);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Clear(void) ;
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot) ;
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 mode);
void OLED_ShowFont_32x32(u8 x,u8 y,u8 fno,u8 mode);
void Movie_Show_Img(u16 x,u16 y,u32 picx);
void OLED_ShowFont_16x16(u8 x,u8 y,u8 fno,u8 mode);
void OLED_ShowFont_12x12(u8 x,u8 y,u8 fno,u8 mode);
void OLED_ShowFont_22x22(u8 x,u8 y,u8 fno,u8 mode);

void OLED_ShowString12x12(u8 x,u8 y,const u8 *p,u8 mode);
void  OLED_logo(void); 
#endif
