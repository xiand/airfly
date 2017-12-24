
#include "bsp.h"

//片选线定义
#define SD_SPI_CS_PIN			GPIO_Pin_10
#define SD_SPI_CS_PORT			GPIOA
#define SD_SPI_CS_ENABLE()		GPIO_ResetBits(SD_SPI_CS_PORT,SD_SPI_CS_PIN)
#define SD_SPI_CS_DISENABLE()	GPIO_SetBits(SD_SPI_CS_PORT,SD_SPI_CS_PIN)


#define SD_SPI					SPI1

#define SPI_BAUD				SPI_BaudRatePrescaler_256

uint8_t 		SD_Type = 0; //SD卡的类型	


//SPI时钟使能
//#define ENABLE_SPI_RCC() RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE)
#define ENABLE_SPI_RCC()		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE)

/*******************************************************************************
* Function Name  : spiGpioInit
* Description	 : SPI1控制引脚初始化
* Input 		 : 无
* Output		 : None
* Return		 : None
*******************************************************************************/
static void spiGpioInit(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能gpio时钟

	//PB3 -- SPI1_SCK  
	//PB4 -- SPI1_MISO	
	//PB5 -- SPI1_MOSI 
	//配置引脚为SPI模式复用功能
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


#if 0
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //使能gpio时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	//PB0 -- SPI5_SCK  
	//PA12 -- SPI5_MISO  
	//PA10 -- SPI5_MOSI 
	//配置引脚为SPI模式复用功能
	GPIO_PinAFConfig(GPIOB, GPIO_Pin_0, GPIO_AF_SPI5);
	GPIO_PinAFConfig(GPIOA, GPIO_Pin_12, GPIO_AF_SPI5);
	GPIO_PinAFConfig(GPIOA, GPIO_Pin_10, GPIO_AF_SPI5);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//这两个参数设置有点不懂
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;

	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif


	//初始化CS引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = SD_SPI_CS_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SD_SPI_CS_PORT, &GPIO_InitStructure);

}


/*******************************************************************************
* Function Name  : spiHardInit
* Description	 : SPI1硬件初始化
* Input 		 : 无
* Output		 : None
* Return		 : None
*******************************************************************************/
static void spiHardInit(void)
{
	SPI_InitTypeDef SPI_InitStructure;

	ENABLE_SPI_RCC();

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //数据方向：两线全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	//工作模式：主机模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //数据长度：8位

	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; 	//时钟上升沿采集数据
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//时钟第二个边沿采样数据
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//片选控制方式，软件控制

	//设置波特率分频系数
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BAUD; //这里需要好好进行计算
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //数据位传输次序，高位先传
	SPI_InitStructure.SPI_CRCPolynomial = 7;		//CRC寄存器，复位后为7，不使用
	SPI_Init(SD_SPI, &SPI_InitStructure);

	//SPI_Cmd(SD_SPI,DISABLE);	 //先禁止spi
	SPI_Cmd(SD_SPI, ENABLE);						//开启SPI
}


/*******************************************************************************
* Function Name  : bsp_sdInit
* Description	 : SD卡硬件初始化（配置SPI时钟，以及GPIO片选）
* Input 		 : 无
* Output		 : None
* Return		 : None
*******************************************************************************/
void bsp_sdInit(void)
{
	spiGpioInit();
	spiHardInit();
}


/*******************************************************************************
* Function Name  : sd_ReadWriteByte
* Description	 : SPI读写一个字节数据
* Input 		 : 需要发送的数据
* Output		 : None
* Return		 : 接受到的数据
*******************************************************************************/
uint8_t sd_ReadWriteByte(uint8_t ucData)
{
	while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_TXE) == RESET)
		; //等待发送去空

	SPI_I2S_SendData(SD_SPI, ucData);

	while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_RXNE) == RESET)
		; //等待接收完成

	return SPI_I2S_ReceiveData(SD_SPI);
}


/*******************************************************************************
* Function Name  : sd_waitReady
* Description	 : 等待SD卡Ready
* Input 		 : None
* Output		 : None
* Return		 : uint8_t 
*					0： 成功
*					other：失败
*******************************************************************************/
uint8_t sd_WaitReady(void)
{
	uint8_t 		i	= 0;
	uint16_t		usRetry = 0;

	do 
	{
		i					= sd_ReadWriteByte(0xff);
		usRetry++;

		if (usRetry == 0xfffe)
		{
			return 1;
		}
	}
	while(i != 0xff);

	return 0;
}


/*******************************************************************************
* Function Name  : sd_SendCommand
* Description	 : 向SD卡发送一个命令
* Input 		 : uint8_tcmd	命令 
*				   uint32 arg  命令参数
*				   uint8_t crc	 crc校验值
* Output		 : None
* Return		 : uint8_t SD卡返回的响应
*******************************************************************************/
uint8_t sd_SendCommand(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	uint8_t 		ucData;
	uint32_t		uiRetry = 0;

	SD_SPI_CS_DISENABLE();
	sd_ReadWriteByte(0xff);
	SD_SPI_CS_ENABLE(); 							//片选端为低，使能

	//发送命令序列 命令的第二位一定要为1 ，所以对于发送的命令需要和0x40或一下。保证第二位为1。
	sd_ReadWriteByte(cmd | 0x40);
	sd_ReadWriteByte((uint8_t) (arg >> 24));		//参数【31----24】
	sd_ReadWriteByte((uint8_t) (arg >> 16));		//参数【23----16】
	sd_ReadWriteByte((uint8_t) (arg >> 8)); 		//参数【15-----8】
	sd_ReadWriteByte((uint8_t) (arg));
	sd_ReadWriteByte(crc);

	//等待响应，超时退出
	while ((ucData = sd_ReadWriteByte(0xff)) == 0xff)
	{
		uiRetry++;

		if (uiRetry > 800)
		{
			break;
		}
	}

	//关闭片选
	SD_SPI_CS_DISENABLE();


	sd_ReadWriteByte(0xff); 						//在总线上额外增加8个时钟，让sd卡完成余下的工作
	return ucData;
}


/*******************************************************************************
* Function Name  : sd_SendCommandNoDeassert
* Description	 : 向SD卡发送一个命令 但是当结束时不要失能片选，因为接下来还有数据要传
	输
* Input 		 : uint8_tcmd	命令 
*				   uint32 arg  命令参数
*				   uint8_t crc	 crc校验值
* Output		 : None
* Return		 : uint8_t SD卡返回的响应
*******************************************************************************/
uint8_t sd_SendCommandNoDeassert(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	uint8_t 		ucData;
	uint32_t		uiRetry = 0;

	SD_SPI_CS_DISENABLE();
	sd_ReadWriteByte(0xff);
	SD_SPI_CS_ENABLE(); 							//片选端为低，使能

	//发送命令序列
	sd_ReadWriteByte(cmd | 0x40);
	sd_ReadWriteByte((uint8_t) (arg >> 24));		//参数【31----24】
	sd_ReadWriteByte((uint8_t) (arg >> 16));		//参数【23----16】
	sd_ReadWriteByte((uint8_t) (arg >> 8)); 		//参数【15-----8】
	sd_ReadWriteByte((uint8_t) (arg));
	sd_ReadWriteByte(crc);

	//等待响应，超时退出
	while ((ucData = sd_ReadWriteByte(0xff)) == 0xff)
	{
		uiRetry++;

		if (uiRetry > 800)
		{
			break;
		}
	}

	//关闭片选
	//SD_SPI_CS_DISENABLE();
	//sd_ReadWriteByte(0xff);  //在总线上额外增加8个时钟，让sd卡完成余下的工作
	return ucData;
}


/*******************************************************************************
* Function Name  : SD_Init
* Description	 : 初始化SD卡
* Input 		 : None
* Output		 : None
* Return		 : uint8_t 
*				   0：NO_ERR
*				   1：TIME_OUT
*				   99：NO_CARD
*******************************************************************************/
uint8_t SD_Init(void)
{
	uint16_t		i;								// 用来循环计数
	uint8_t 		r1; 							// 存放SD卡的返回值
	uint16_t		retry;							// 用来进行超时计数
	uint8_t 		buff[6];

	//SPI_ControlLine();
	//SPI_SetSpeed(SPI_SPEED_LOW);
	spiGpioInit();
	spiHardInit();
	SD_SPI_CS_ENABLE();
	printf("enter the SD_INIT\r\n");

	// 纯延时，等待SD卡上电完成
	for (i = 0; i < 0xf00; i++)
		;

	//先产生至少74个脉冲，让SD卡自己初始化完成
	for (i = 0; i < 10; i++)
	{
		sd_ReadWriteByte(0xFF); 					//80clks
	}
	//-----------------SD卡复位到idle开始-----------------
	//循环连续发送CMD0，直到SD卡返回0x01,进入IDLE状态
	//超时则直接退出
	retry				= 0;

	do 
	{
		//发送CMD0，让SD卡进入IDLE状态
		r1					= sd_SendCommand(CMD0, 0, 0x95);
		retry++;
	}
	while((r1 != 0x01) && (retry < 200));

	//跳出循环后，检查原因：初始化成功？or 重试超时？
	if (retry == 200)
		return 1; //超时返回1	

	printf("enter the SD_INIT3\r\n");

	//-----------------SD卡复位到idle结束-----------------	 
	//获取卡片的SD版本信息
	r1					= sd_SendCommandNoDeassert(CMD8, 0x1aa, 0x87);

	//如果卡片版本信息是v1.0版本的，即r1=0x05，则进行以下初始化
	if (r1 == 0x05)
	{
		//设置卡类型为SDV1.0，如果后面检测到为MMC卡，再修改为MMC
		SD_Type 			= SD_TYPE_V1;

		//如果是V1.0卡，CMD8指令后没有后续数据
		//片选置高，结束本次命令
		SD_SPI_CS_DISENABLE();

		//多发8个CLK，让SD结束后续操作
		sd_ReadWriteByte(0xFF);

		//-----------------SD卡、MMC卡初始化开始-----------------	 
		//发卡初始化指令CMD55+ACMD41
		// 如果有应答，说明是SD卡，且初始化完成
		// 没有回应，说明是MMC卡，额外进行相应初始化
		retry				= 0;

		do 
		{
			//先发CMD55，应返回0x01；否则出错
			r1					= sd_SendCommand(CMD55, 0, 0);

			if (r1 != 0x01)
				return r1;

			//得到正确响应后，发ACMD41，应得到返回值0x00，否则重试400次
			r1					= sd_SendCommand(ACMD41, 0, 0);
			retry++;
		}
		while((r1 != 0x00) && (retry < 400));

		// 判断是超时还是得到正确回应
		// 若有回应：是SD卡；没有回应：是MMC卡
		//----------MMC卡额外初始化操作开始------------
		if (retry == 400)
		{
			retry				= 0;

			//发送MMC卡初始化命令（没有测试）
			do 
			{
				r1					= sd_SendCommand(CMD1, 0, 0);
				retry++;
			}
			while((r1 != 0x00) && (retry < 400));

			if (retry == 400)
				return 1; //MMC卡初始化超时			

			//写入卡类型
			SD_Type 			= SD_TYPE_MMC;
		}

		//----------MMC卡额外初始化操作结束------------		
		//设置SPI为高速模式
		//SPI_SetSpeed(SPI_SPEED_HIGH);   
		sd_ReadWriteByte(0xFF);

		//禁止CRC校验	   
		r1					= sd_SendCommand(CMD59, 0, 0x95);

		if (r1 != 0x00)
			return r1; //命令错误，返回r1	   

		//设置Sector Size
		r1					= sd_SendCommand(CMD16, 512, 0x95);

		if (r1 != 0x00)
			return r1; //命令错误，返回r1		 

		//-----------------SD卡、MMC卡初始化结束-----------------
	} //SD卡为V1.0版本的初始化结束	 
	//下面是V2.0卡的初始化
	//其中需要读取OCR数据，判断是SD2.0还是SD2.0HC卡
	else if (r1 == 0x01)
	{
		//V2.0的卡，CMD8命令后会传回4字节的数据，要跳过再结束本命令
		buff[0] 			= sd_ReadWriteByte(0xFF); //should be 0x00
		buff[1] 			= sd_ReadWriteByte(0xFF); //should be 0x00
		buff[2] 			= sd_ReadWriteByte(0xFF); //should be 0x01
		buff[3] 			= sd_ReadWriteByte(0xFF); //should be 0xAA
		printf("buff[0] = %x, buf[1] = %x,buf[2] = %x,buff[3] = %x\r\n", buff[0], buff[1], buff[2], buff[3]);
		SD_SPI_CS_DISENABLE();
		sd_ReadWriteByte(0xFF); 					//the next 8 clocks			 

		//判断该卡是否支持2.7V-3.6V的电压范围
		if (buff[2] == 0x01 && buff[3] == 0xAA) //如不判断，让其支持的卡更多
		{
			retry				= 0;

			//发卡初始化指令CMD55+ACMD41
			do 
			{
				retry++;
				r1					= sd_SendCommand(CMD55, 0, 0);

				if (r1 != 0x01)
				{
					return r1;
				}

				r1					= sd_SendCommand(ACMD41, 0x40000000, 1);

				if (retry > 400)
				{
					return r1; //超时则返回r1状态 
				}
			}
			while(r1 != 0);
			//初始化指令发送完成，接下来获取OCR信息		   
			//-----------鉴别SD2.0卡版本开始-----------
			r1					= sd_SendCommandNoDeassert(CMD58, 0, 0);

			if (r1 != 0x00)
				return r1; //如果命令没有返回正确应答，直接退出，返回应答		 

			//读OCR指令发出后，紧接着是4字节的OCR信息
			buff[0] 			= sd_ReadWriteByte(0xFF);
			buff[1] 			= sd_ReadWriteByte(0xFF);
			buff[2] 			= sd_ReadWriteByte(0xFF);
			buff[3] 			= sd_ReadWriteByte(0xFF);

			//OCR接收完成，片选置高
			SD_SPI_CS_DISENABLE();
			sd_ReadWriteByte(0xFF);
			printf("enter the SD_INITsssssss\r\n");

			//检查接收到的OCR中的bit30位（CCS），确定其为SD2.0还是SDHC
			//如果CCS=1：SDHC   CCS=0：SD2.0
			if (buff[0] &0x40)
				SD_Type = SD_TYPE_V2HC; //检查CCS	 
			else 
				SD_Type = SD_TYPE_V2;

			printf("the sd_type = %d\r\n", SD_Type);

			//-----------鉴别SD2.0卡版本结束----------- 
			//设置SPI为高速模式
			//SPI_SetSpeed(1);	
		}
	}
	return r1;
}




/*******************************************************************************
* Function Name  : SD_ReceiveData
* Description	 : 从SD卡中读回指定长度的数据，放置在给定位置
* Input 		 : uint8_t *data(存放读回数据的内存>len)
*				   uint16_t len(数据长度）
*				   uint8_t release(传输完成后是否释放总线CS置高 0：不释放 1：释放）
* Output		 : None
* Return		 : uint8_t 
*				   0：NO_ERR
*				   other：错误信息
*******************************************************************************/
uint8_t sd_ReceiveData(uint8_t * data, uint16_t len, uint8_t release)
{
	uint16_t retry;
	uint8_t r1;

	// 启动一次传输
	SD_SPI_CS_ENABLE();

	//等待SD卡发回数据起始令牌0xFE
	retry				= 0;

	do 
	{
		r1					= sd_ReadWriteByte(0xFF);
		retry++;

		if (retry > 4000) //4000次等待后没有应答，退出报错(根据实验测试，此处最好多试几次
		{
			SD_SPI_CS_DISENABLE();
			return 1;
		}
	}
	while(r1 != 0xFE);

	//开始接收数据
	while (len--)
	{
		*data				= sd_ReadWriteByte(0xFF);
		data++;
	}

	//下面是2个伪CRC（dummy CRC）
	sd_ReadWriteByte(0xFF);
	sd_ReadWriteByte(0xFF);

	//按需释放总线，将CS置高
	if (release == RELEASE)
	{
		//传输结束
		SD_SPI_CS_DISENABLE();
		sd_ReadWriteByte(0xFF);
	}

	return 0;
}


/*******************************************************************************
* Function Name  : SD_GetCID
* Description	 : 获取SD卡的CID信息，包括制造商信息
* Input 		 : uint8_t *cid_data(存放CID的内存，至少16Byte）
* Output		 : None
* Return		 : uint8_t 
*				   0：NO_ERR
*				   1：TIME_OUT
*				   other：错误信息
*******************************************************************************/
uint8_t sd_GetCID(uint8_t * cid_data)
{
	uint8_t r1;

	//发CMD10命令，读CID
	r1					= sd_SendCommand(CMD10, 0, 0xFF);

	if (r1 != 0x00)
		return r1; //没返回正确应答，则退出，报错    

	//接收16个字节的数据
	sd_ReceiveData(cid_data, 16, RELEASE);
	return 0;
}


/*******************************************************************************
* Function Name  : SD_GetCSD
* Description	 : 获取SD卡的CSD信息，包括容量和速度信息
* Input 		 : uint8_t *cid_data(存放CID的内存，至少16Byte）
* Output		 : None
* Return		 : uint8_t 
*				   0：NO_ERR
*				   1：TIME_OUT
*				   other：错误信息
*******************************************************************************/
uint8_t sd_GetCSD(uint8_t * csd_data)
{
	uint8_t r1;

	//发CMD9命令，读CSD
	r1					= sd_SendCommand(CMD9, 0, 0xFF);

	if (r1 != 0x00)
		return r1; //没返回正确应答，则退出，报错  

	//接收16个字节的数据
	sd_ReceiveData(csd_data, 16, RELEASE);

	return 0;
}


/*******************************************************************************
* Function Name  : SD_GetCapacity
* Description	 : 获取SD卡的容量（字节）
* Input 		 : None
* Output		 : None
* Return		 : u32 capacity 
*					0： 取容量出错 
*******************************************************************************/
uint32_t SD_GetCapacity(void)
{
	uint8_t csd[16];
	u32 Capacity;
	uint8_t r1;
	uint16_t i;
	uint16_t temp;

	//取CSD信息，如果期间出错，返回0
	if (sd_GetCSD(csd) != 0)
		return 0;

	//如果为SDHC卡，按照下面方式计算
	if ((csd[0] &0xC0) == 0x40)
	{
		printf("该sd卡为SDHC类型\r\n");
		Capacity			= ((u32) csd[8]) << 8;
		Capacity			+= (u32)
		csd[9] +1;
		Capacity			= (Capacity) * 1024;	//得到扇区数
		Capacity			*= 512; 				//得到字节数			   
	}
	else 
	{
		i					= csd[6] &0x03;
		i <<= 8;
		i					+= csd[7];
		i <<= 2;
		i					+= ((csd[8] &0xc0) >> 6);

		//C_SIZE_MULT
		r1					= csd[9] &0x03;
		r1 <<= 1;
		r1					+= ((csd[10] &0x80) >> 7);
		r1					+= 2;					//BLOCKNR
		temp				= 1;

		while (r1)
		{
			temp				*= 2;
			r1--;
		}

		Capacity			= ((u32) (i + 1)) * ((u32) temp);

		// READ_BL_LEN
		i					= csd[5] &0x0f;

		//BLOCK_LEN
		temp				= 1;

		while (i)
		{
			temp				*= 2;
			i--;
		}

		//The final result
		Capacity			*= (u32)
		temp;										//字节为单位	  
	}

	return (u32)
	Capacity;
}


/*******************************************************************************
* Function Name  : SD_ReadSingleBlock
* Description	 : 读SD卡的一个block
* Input 		 : u32 sector 取地址（sector值，非物理地址） 
*				   uint8_t *buffer 数据存储地址（大小至少512byte） 
* Output		 : None
* Return		 : uint8_t r1 
*					0： 成功
*					other：失败
*******************************************************************************/
uint8_t sd_ReadSingleBlock(uint8_t * buffer,u32 sector,  uint32_t len)
{
	uint8_t r1;

	//设置为高速模式
	//SPI_SetSpeed(SPI_SPEED_HIGH);
	//如果不是SDHC，将sector地址转成byte地址
	//sector				= sector << 9;

	r1					= sd_SendCommand(CMD17, sector, 1); //读命令

	if (r1 != 0x00)
	{
			printf("r1 ssss = %d\r\n",r1);
		  return r1;
	}
	r1					= sd_ReceiveData(buffer, len, RELEASE);
	printf("r1 ggggg  = %d\r\n",r1);
	if (r1 != 0)
		return r1; //读数据出错！

	else 
		return 0;
}


/*******************************************************************************
* Function Name  : SD_WriteSingleBlock
* Description	 : 写入SD卡的一个block
* Input 		 : u32 sector 扇区地址（sector值，非物理地址） 
*				   uint8_t *buffer 数据存储地址（大小至少512byte） 
* Output		 : None
* Return		 : uint8_t r1 
*					0： 成功
*					other：失败
*******************************************************************************/
uint8_t SD_WriteSingleBlock(u32 sector, uint8_t * data, uint32_t uiSize)
{
	uint8_t r1;
	uint16_t i;
	uint16_t retry;

	//设置为高速模式
	//SPI_SetSpeed(SPI_SPEED_HIGH);
	//如果不是SDHC，给定的是sector地址，将其转换成byte地址
	if (SD_Type != SD_TYPE_V2HC)
	{
		sector				= sector << 9;			//512*sector即物理扇区的边界对齐地址
	}

	r1					= sd_SendCommand(CMD24, sector, 0x01);
	
	if (r1 != 0x00)
	{
		printf("the r1 xxxx  = %d\r\n",r1);
		return r1; //应答不正确，直接返回
	}

	//开始准备数据传输
	SD_SPI_CS_ENABLE();

	//先放3个空数据，等待SD卡准备好
	sd_ReadWriteByte(0xff);
	sd_ReadWriteByte(0xff);
	sd_ReadWriteByte(0xff);

	//放起始令牌0xFE
	sd_ReadWriteByte(0xFE);

	//发一个sector的数据
	for (i = 0; i < uiSize; i++)
	{
		sd_ReadWriteByte(*data++);
	}

	//发2个Byte的dummy CRC
	sd_ReadWriteByte(0xff);
	sd_ReadWriteByte(0xff);

	//等待SD卡应答
	r1					= sd_ReadWriteByte(0xff);

	if ((r1 & 0x1F) != 0x05)
	{
		SD_SPI_CS_DISENABLE();
		printf("the r1 xxbbbbbb  = %d\r\n",r1);
		//return r1;
	}

	//等待操作完成
	retry				= 0;

	while (!sd_ReadWriteByte(0xff)) //卡自编程时，数据线被拉低
	{
		retry++;

		if (retry > 65534) //如果长时间写入没有完成，报错退出
		{
			SD_SPI_CS_DISENABLE();
			return 1; //写入超时返回1
		}
	}

	//写入完成，片选置1
	SD_SPI_CS_DISENABLE();
	sd_ReadWriteByte(0xff);

	return 0;
}


/*******************************************************************************
* Function Name  : SD_ReadMultiBlock
* Description	 : 读SD卡的多个block
* Input 		 : u32 sector 取地址（sector值，非物理地址） 
*				   uint8_t *buffer 数据存储地址（大小至少512byte）
*				   uint8_t count 连续读count个block
* Output		 : None
* Return		 : uint8_t r1 
*					0： 成功
*					other：失败
*******************************************************************************/
uint8_t SD_ReadMultiBlock(uint8_t * buffer,u32 sector,uint32_t len,  uint8_t count)
{
	uint8_t r1;

	//SPI_SetSpeed(SPI_SPEED_HIGH);//设置为高速模式	
	sector				= sector << 9;				//如果不是SDHC，将sector地址转成byte地址

	//SD_WaitReady();
	//发读多块命令
	r1					= sd_SendCommand(CMD18, sector, 1); //读命令

	if (r1 != 0x00)
		return r1;

	do //开始接收数据
	{
		if (sd_ReceiveData(buffer, len, NO_RELEASE) != 0x00)
		{
			break;
		}

		buffer				+= len;
	}
	while(--count);

	//全部传输完毕，发送停止命令
	sd_SendCommand(CMD12, 0, 1);

	//释放总线
	SD_SPI_CS_DISENABLE();
	sd_ReadWriteByte(0xFF);

	if (count != 0)
		return count; //如果没有传完，返回剩余个数	 

	else 
		return 0;
}


/*******************************************************************************
* Function Name  : SD_WriteMultiBlock
* Description	 : 写入SD卡的N个block
* Input 		 : u32 sector 扇区地址（sector值，非物理地址） 
*				   uint8_t *buffer 数据存储地址（大小至少512byte）
*				   uint8_t count 写入的block数目
* Output		 : None
* Return		 : uint8_t r1 
*					0： 成功
*					other：失败
*******************************************************************************/
uint8_t SD_WriteMultiBlock(u32 sector, const uint8_t * data, uint32_t uiSize,uint8_t count)
{
	uint8_t r1;
	uint16_t i;

	//SPI_SetSpeed(SPI_SPEED_HIGH);//设置为高速模式	 
	if (SD_Type != SD_TYPE_V2HC)
		sector = sector << 9; //如果不是SDHC，给定的是sector地址，将其转换成byte地址  

	if (SD_Type != SD_TYPE_MMC)
		r1 = sd_SendCommand(ACMD23, count, 0x01); //如果目标卡不是MMC卡，启用ACMD23指令使能预擦除   

	r1					= sd_SendCommand(CMD25, sector, 0x01); //发多块写入指令

	if (r1 != 0x00)
		return r1; //应答不正确，直接返回	 

	SD_SPI_CS_ENABLE(); 							//开始准备数据传输	 
	sd_ReadWriteByte(0xff); 						//先放3个空数据，等待SD卡准备好
	sd_ReadWriteByte(0xff);

	//--------下面是N个sector写入的循环部分
	do 
	{
		//放起始令牌0xFC 表明是多块写入
		sd_ReadWriteByte(0xFC);

		//放一个sector的数据
		for (i = 0; i < uiSize; i++)
		{
			sd_ReadWriteByte(*data++);
		}

		//发2个Byte的dummy CRC
		sd_ReadWriteByte(0xff);
		sd_ReadWriteByte(0xff);

		//等待SD卡应答
		r1					= sd_ReadWriteByte(0xff);

		if ((r1 & 0x1F) != 0x05)
		{
			SD_SPI_CS_DISENABLE();					//如果应答为报错，则带错误代码直接退出
			return r1;
		}

		//等待SD卡写入完成
		if (sd_WaitReady() == 1)
		{
			SD_SPI_CS_DISENABLE();					//等待SD卡写入完成超时，直接退出报错
			return 1;
		}
	}
	while(--count);

	//本sector数据传输完成
	//发结束传输令牌0xFD
	r1					= sd_ReadWriteByte(0xFD);

	if (r1 == 0x00)
	{
		count				= 0xfe;
	}

	if (sd_WaitReady()) //等待准备好
	{
		SD_SPI_CS_DISENABLE();
		return 1;
	}

	//写入完成，片选置1
	SD_SPI_CS_DISENABLE();
	sd_ReadWriteByte(0xff);
	return count; //返回count值，如果写完则count=0，否则count=1
}


/*******************************************************************************
* Function Name  : SD_Read_Bytes
* Description	 : 在指定扇区,从offset开始读出bytes个字节
* Input 		 : u32 address 扇区地址（sector值，非物理地址） 
*				   uint8_t *buf 	数据存储地址（大小<=512byte）
*				   uint16_t offset	在扇区里面的偏移量
				   uint16_t bytes	要读出的字节数
* Output		 : None
* Return		 : uint8_t r1 
*					0： 成功
*					other：失败
*******************************************************************************/
uint8_t SD_Read_Bytes(unsigned long address, unsigned char * buf, unsigned int offset, unsigned int bytes)
{
	uint8_t r1;
	uint16_t i			= 0;
	r1					= sd_SendCommand(CMD17, address << 9, 1); //发送读扇区命令	   

	if (r1 != 0x00)
		return r1; //应答不正确，直接返回

	SD_SPI_CS_ENABLE(); 								//选中SD卡

	while (sd_ReadWriteByte(0xff) != 0xFE) //直到读取到了数据的开始头0XFE，才继续
	{
		i++;

		if (i > 3000)
		{
			SD_SPI_CS_DISENABLE();					//关闭SD卡
			return 1; //读取失败
		}
	};

	for (i = 0; i < offset; i++)
		sd_ReadWriteByte(0xff); //跳过offset位 

	for (; i < offset + bytes; i++)
		*buf++ = sd_ReadWriteByte(0xff); //读取有用数据	

	for (; i < 512; i++)
		sd_ReadWriteByte(0xff); //读出剩余字节

	sd_ReadWriteByte(0xff); 						//发送伪CRC码
	sd_ReadWriteByte(0xff);
	SD_SPI_CS_DISENABLE();							//关闭SD卡
	return 0;
}


