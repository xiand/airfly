
#include "bsp.h"

#define PORT_DQ 				GPIOB
#define PIN_DQ					GPIO_Pin_1
#define RCC_DQ					RCC_AHB1Periph_GPIOB


#if 1 //使用库函数对IO进行操作

#define DQ_0()					GPIO_ResetBits(PORT_DQ, PIN_DQ)
#define DQ_1()					GPIO_SetBits(PORT_DQ,PIN_DQ)
#define DQ_IN() 				GPIO_ReadInputDataBit(PORT_DQ, PIN_DQ)

//#elif //直接操作寄存器方式
#endif

/*
**************************
功能：ds18b20初始化，初始化DQ引脚  ，未接上拉电阻
形参：无
返回值：无
**************************
*/
void bsp_myInitDs18b20(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_DQ, ENABLE); 		//使能时钟

	GPIO_InitStructure.GPIO_Pin = PIN_DQ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(PORT_DQ, &GPIO_InitStructure);
}


/*
************************************
功能：配置ds18b20 DQ引脚为输出
************************************
*/
static void bsp_configWireOut(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//RCC_AHB1PeriphClockCmd(RCC_DQ, ENABLE);		//使能时钟
	GPIO_InitStructure.GPIO_Pin = PIN_DQ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;	//配置为输出时使用
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(PORT_DQ, &GPIO_InitStructure);
}


/*
***********************************
*功能：配置ds18b20 DQ引脚为输入模式
**********************************
*/
static void bsp_configWireIn(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//RCC_AHB1PeriphClockCmd(RCC_DQ, ENABLE);		//使能时钟
	GPIO_InitStructure.GPIO_Pin = PIN_DQ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;

	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //配置为输出时使用
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(PORT_DQ, &GPIO_InitStructure);
}


/*
*********************************************
*功能：bs18b20复位检测函数
*说明：时序：1、将总线拉低至少 480us；
			 2、释放总线；等待15us
			 3、读取总线是否被拉低，在60--240us内读取
*返回值：1成功，0失败			 
*********************************************
*/
uint8_t bsp_ds18b20Reset(void)
{

	uint8_t 		i	= 0;
	uint8_t 		temp = 0;

	bsp_configWireOut();							//配置为输出模式
	DQ_0(); 										//输出低

	DISABLE_INT();									//关闭全局中断

	bsp_DelayUS(520);								//延时520us

	DQ_1();
	bsp_DelayUS(10);

	bsp_configWireIn(); 							//配置总线为输入模式

	//temp = DQ_IN();
	//bsp_DelayUS(20);
	for (i = 0; i < 25; i++)
	{
		temp				= DQ_IN();
		bsp_DelayUS(10);

		if (0 == temp) //在break和continue之间花费了一天时间找bug
		{
			break;
		}
	}

	ENABLE_INT();									//开启全局中断

	if (i == 25)
	{
		return 0;
	}

	bsp_DelayUS(600);
	return 1;
}


/*
***************************************
*功能：主机向ds18b20发送一个字节
*说明：注意时序，先发低字节
***************************************
*/
static void bsp_ds18b20WriteByte(uint8_t _data)
{
	uint8_t 		i	= 0;
	uint8_t 		temp = 0;

	bsp_configWireOut();							//配置为输出模式

	for (i = 0; i < 8; i++)
	{
		temp				= _data &0x01;			//提取最低位,先传输最低位
		DQ_0();
		bsp_DelayUS(2); 							//延时10us

		if (1 == temp)
		{
			DQ_1();
		}
		else 
		{
			DQ_0();
		}

		bsp_DelayUS(60);
		DQ_1();
		bsp_DelayUS(2);
		_data				= _data >> 1;			//将数据右移1位
	}

	return;
}


/*
***************************************
*功能：主机从ds18b20读取一个字节
***************************************
*/
static uint8_t bsp_ds18b20ReadByte(void)
{
	uint8_t 		data = 0;
	uint8_t 		i	= 0;
	uint8_t 		temp = 0;

	bsp_configWireOut();							//配置为输出模式

	for (i = 0; i < 8; i++)
	{
		DQ_0(); 									//拉低 
		bsp_DelayUS(3);
		DQ_1();
		bsp_DelayUS(3);
		data >>= 1;
		bsp_configWireIn();

		//bsp_DelayUS(20);
		temp				= DQ_IN();

		if (1 == temp)
		{
			data				|= 0x80;
		}

		bsp_DelayUS(60);
		bsp_configWireOut();						//配置为输出模式
	}

	return data;
}


/*
*************************************
*功能：读取ds18b20的唯一64位序列号
*说明：每次对Ds18b20进行操作对需要完成下面三个操作
	   1、复位
	   2、发送ROM指令
	   3、发送RAM指令
*形参：_id将数据存放在其中
*返回值：0读取失败，1读取成功
*************************************
*/
uint8_t bsp_ds18b20ReadID(uint8_t * _id)
{
	uint8_t 		i	= 0;

	if (0 == bsp_ds18b20Reset())
	{
		printf("bsp_ds18b20ReadID reset failure\r\n");
		return 0;
	}
	else 
	{
		//printf("bsp_ds18b20ReadID reset success\r\n");
	}

	bsp_ds18b20WriteByte(0x33); 					//读取该节点的64位序列号

	for (i = 0; i < 8; i++)
	{
		_id[i]				= bsp_ds18b20ReadByte();
	}

	return 1;
}


int16_t bsp_ds18b20ReadTemperature(void)
{
	uint8_t 		tempHigh = 0;
	uint8_t 		tempLow = 0;
	int16_t 		temperature = 0;

	if (0 == bsp_ds18b20Reset())
	{
		printf("bsp_ds18b20ReadTemperature ds18b20 reset failure\r\n");
		return 0;
	}
	else 
	{
		//printf("bsp_ds18b20ReadTemperature reset success\r\n");
	}

	bsp_ds18b20WriteByte(0xcc); 					//跳过ROM（指令码0XCCH）： 此指令只适合于总线上只有一个节点；该命令通过允许总线主机不提供64位ROM序列号而直接访问RAM，以节省操作时间。
	bsp_ds18b20WriteByte(0x44); 					//温度转换（指令码0X44H）:启动DS18B20进行温度转换，结果存入内部RAM。

	if (0 == bsp_ds18b20Reset())
	{
		printf("bsp_ds18b20ReadTemperature ds18b20 reset failure\r\n");
		return 0;
	}
	else 
	{
		//printf("bsp_ds18b20ReadTemperature reset success\r\n");
	}

	bsp_DelayMS(100);
	bsp_ds18b20WriteByte(0xcc);
	bsp_ds18b20WriteByte(0xbe); 					//读暂存器（指令码0XBEH）：读暂存器9个字节内容，此指令从RAM的第1个字节（字节0）开始读取，直到九个字节（字节8，CRC值）被读出为止。如果不需要读出所有字节的内容，那么主机可以在任何时候发出复位信号以中止读操作。

	tempHigh			= bsp_ds18b20ReadByte();
	tempLow 			= bsp_ds18b20ReadByte();
	printf("the tempHigh = %X,tempLow = %X\r\n", tempHigh, tempLow);
	temperature 		= ((tempLow << 8) | tempHigh);
	return temperature;
}


/*
*********************************************************************************************************
*	函 数 名: bsp_Ds18b20ReadTempByID
*	功能说明: 读指定ID的温度寄存器的值（原始数据）
*	形    参: 无
*	返 回 值: 温度寄存器数据 （除以16得到 1摄氏度单位, 也就是小数点前面的数字)
*********************************************************************************************************
*/
int16_t bsp_Ds18b20ReadTempByID(uint8_t * _id)
{
	uint8_t 		temp1, temp2;
	uint8_t 		i;

	if (0 == bsp_ds18b20Reset())
	{
		printf("bsp_ds18b20ReadTemperature ds18b20 reset failure\r\n");
		return 0;
	}
	else 
	{
		//printf("bsp_ds18b20ReadTemperature reset success\r\n");
	}

	bsp_ds18b20WriteByte(0x55); 					/* 发命令 */

	for (i = 0; i < 8; i++)
	{
		bsp_ds18b20WriteByte(_id[i]);
	}

	bsp_ds18b20WriteByte(0x44); 					/* 发转换命令 */
	bsp_DelayMS(100);

	if (0 == bsp_ds18b20Reset())
	{
		printf("bsp_ds18b20ReadTemperature ds18b20 reset failure\r\n");
		return 0;
	}
	else 
	{
		//printf("bsp_ds18b20ReadTemperature reset success\r\n");
	}

	bsp_ds18b20WriteByte(0x55); 					/* 发命令 */

	for (i = 0; i < 8; i++)
	{
		bsp_ds18b20WriteByte(_id[i]);
	}

	bsp_ds18b20WriteByte(0xbe);

	temp1				= bsp_ds18b20ReadByte();	/* 读温度值低字节 */
	temp2				= bsp_ds18b20ReadByte();	/* 读温度值高字节 */
	return ((temp2 << 8) | temp1); /* 返回16位寄存器值 */
}


#if 0 //main函数中用于调试代码

if (0 == bsp_ds18b20ReadID(id))
{
	printf("未检测到 DS18B20 \r\n");
	memset(g_rom_id, 0, 8);
}


else 
{
	printf("DS18B20 Ok, id = ");

	for (i = 0; i < 8; i++)
	{
		printf("%02X ", id[i]);
	}

	printf("\r\n");

	memcpy(g_rom_id, id, 8);
}


//reg = bsp_Ds18b20ReadTempByID(g_rom_id);
//reg = bsp_ds18b20ReadTemperature();
printf("reg = 0x%04X %6d --> %-4.04f℃\r\n", (uint16_t) reg, reg, (float) reg / 16);
printf("\r\n");
break;

//reg = bsp_Ds18b20ReadTempByID(g_rom_id);
reg = bsp_ds18b20ReadTemperature();
printf("reg = 0x%04X %6d --> %-4.04f℃\r\n", (uint16_t) reg, reg, (float) reg / 16);
printf("\r\n");


#endif

