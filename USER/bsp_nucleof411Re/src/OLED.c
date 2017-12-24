#include  "font.h"
#include "bsp.h"

uint8_t OLED_GRAM[128][8];



void SPI1_Initaize(void)
{
#if 0
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	/* Configure SPI1 pins: SCK, MISO and MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
#endif
#if 0
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //使能PB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); //使能SPI1 

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //时钟速度100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//复用
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE);

	//SPI1设置
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
	#endif
	
}


	 
//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
		retry++;
		if(retry>200)return 0;
		}			  
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
	retry=0;
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); //检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据				    
}


//RS 为0时写命令
void  Write_com(u8  com)
{
 	Clr_OLED_RS;  //RS 为0时写命令
	Clr_OLED_CS;  //CS为0打开片选
	SPI_I2S_SendData(SPI1, com);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) != RESET);				 
	Set_OLED_CS;		  
	Set_OLED_RS; 
}

// //RS 为1时写数据
void  Write_data(u8 dat)
{
	Set_OLED_RS;;  //RS 为1时写数据
	Clr_OLED_CS;  //CS为0打开片选
	SPI_I2S_SendData(SPI1, dat);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) != RESET);			 
	Set_OLED_CS;		  
	Set_OLED_RS; 
}

	 
//更新显存到LCD		 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		Write_com(0xb0+i);    //设置页地址（0~7）
		Write_com(0x00);      //设置显示位置—列低地址,偏移了2列
		Write_com(0x10);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)Write_data(OLED_GRAM[n][i]); 
	}   
}

//开启OLED显示    
void OLED_Display_On(void)
{
	Write_com(0X8D);  //SET DCDC命令
	Write_com(0X14);  //DCDC ON
	Write_com(0XAF);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	Write_com(0X8D);  //SET DCDC命令
	Write_com(0X10);  //DCDC OFF
	Write_com(0XAE);  //DISPLAY OFF
}

//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//更新显示
}

//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
	
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
	{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
		{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
		}													    
	OLED_Refresh_Gram();//更新显示
	}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';//得到偏移后的值				   
	for(t=0;t<size;t++)
		{   
		if(size==12)temp=asc2_1206[chr][t];  //调用1206字体
		else temp=asc2_1608[chr][t];		 //调用1608字体 	                          
		for(t1=0;t1<8;t1++)
			{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else  OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
				{
				y=y0;
				x++;
				break;
				}
			}  	 
		}          
}

//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 mode)
{
	#define MAX_CHAR_POSX 122
	#define MAX_CHAR_POSY 58          
	while(*p!='\0')
		{       
		if(x>MAX_CHAR_POSX){x=0;y+=16;}
		if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
		OLED_ShowChar(x,y,*p,16,mode);	 
		x+=8;
		p++;
		}  
}

void OLED_ShowString12x12(u8 x,u8 y,const u8 *p,u8 mode)
{
	#define MAX_CHAR_POSX 122
	#define MAX_CHAR_POSY 58          
	while(*p!='\0')
		{       
		if(x>MAX_CHAR_POSX){x=0;y+=16;}
		if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
		OLED_ShowChar(x,y,*p,12,mode);	 
		x+=8;
		p++;
		}  
}


//在指定位置显示一个汉字
//x:0~127
//y:0~63
//fno:汉字编号
//mode:0,反白显示;1,正常显示				 
//22*22大小的字符
void OLED_ShowFont_32x32(u8 x,u8 y,u8 fno,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;				   
    for(t=0;t<128;t++)
    {   
		temp=user_hz_32x32[fno][t];    //调用22*22汉字	                           
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==32)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}

void OLED_ShowFont_16x16(u8 x,u8 y,u8 fno,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;				   
    for(t=0;t<32;t++)
    {   
		temp=user_hz16x16[fno][t];    //调用22*22汉字	                           
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==16)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}

void OLED_ShowFont_12x12(u8 x,u8 y,u8 fno,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;				   
    for(t=0;t<24;t++)
    {   
		temp=user_hz12x12[fno][t];    //调用22*22汉字	                           
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==12)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}

void OLED_ShowFont_22x22(u8 x,u8 y,u8 fno,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;				   
    for(t=0;t<66;t++)
    {   
		temp=user_hz22x22[fno][t];    //调用22*22汉字	                           
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==22)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}



//一列64个点向下画点，一列一列画,共128列
void Movie_Show_Img(u16 x,u16 y,u32 picx)
{      			    
	u16 temp,t,t1;
	u16 y0=y;	
	picx*=FRAME_SIZE;				    				   
    for(t=0;t<FRAME_SIZE;t++)
    {   
		temp=movie_data[picx+t];  //得到图片的一个字节数据	                          
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,1); 
			else OLED_DrawPoint(x,y,0);		   
			temp<<=1;
			y++;
			if((y-y0)==MOVIE_YSIZE)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}

//初始化SSD1303					    
void OLED_Init(void)
{ 	 				 	 					    
		GPIO_InitTypeDef  GPIO_InitStructure;	  
		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE );	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE);
				    	 	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9 |GPIO_Pin_6 ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 		 //推挽输出
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		GPIO_SetBits(GPIOC, GPIO_Pin_8 | GPIO_Pin_9 |GPIO_Pin_6);
					  
		Clr_OLED_RST;
		bsp_DelayMS(10);
		Set_OLED_RST; 
		bsp_DelayMS(10);
				  
 		Write_com(0xa0);  //set seg>        
        Write_com(0xc0);  // seg com>
        Write_com(0x81);  //set vol mode       
        Write_com(0xcc);  // set v5 register
        Write_com(0x27);  // set ra\rb ratio
        Write_com(0x2f);  // set vc vr vf on
        Write_com(0xa2);  // set lcd bias      
        Write_com(0x40);  // set display start line address      
        Write_com(0xaf);   // set lcd display on  
		OLED_Clear();
	    Write_com(0xA5); //全屏测试
} 

void  OLED_logo(void)
{
	Movie_Show_Img(0,0,0);//图片
	OLED_Refresh_Gram();//更新显示
	bsp_DelayMS(5000);	
	OLED_Clear();
}


