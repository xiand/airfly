
#include "bsp.h"

#define PORT_DQ 				GPIOB
#define PIN_DQ					GPIO_Pin_1
#define RCC_DQ					RCC_AHB1Periph_GPIOB


#if 1 //ʹ�ÿ⺯����IO���в���

#define DQ_0()					GPIO_ResetBits(PORT_DQ, PIN_DQ)
#define DQ_1()					GPIO_SetBits(PORT_DQ,PIN_DQ)
#define DQ_IN() 				GPIO_ReadInputDataBit(PORT_DQ, PIN_DQ)

//#elif //ֱ�Ӳ����Ĵ�����ʽ
#endif

/*
**************************
���ܣ�ds18b20��ʼ������ʼ��DQ����  ��δ����������
�βΣ���
����ֵ����
**************************
*/
void bsp_myInitDs18b20(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_DQ, ENABLE); 		//ʹ��ʱ��

	GPIO_InitStructure.GPIO_Pin = PIN_DQ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(PORT_DQ, &GPIO_InitStructure);
}


/*
************************************
���ܣ�����ds18b20 DQ����Ϊ���
************************************
*/
static void bsp_configWireOut(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//RCC_AHB1PeriphClockCmd(RCC_DQ, ENABLE);		//ʹ��ʱ��
	GPIO_InitStructure.GPIO_Pin = PIN_DQ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;	//����Ϊ���ʱʹ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(PORT_DQ, &GPIO_InitStructure);
}


/*
***********************************
*���ܣ�����ds18b20 DQ����Ϊ����ģʽ
**********************************
*/
static void bsp_configWireIn(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//RCC_AHB1PeriphClockCmd(RCC_DQ, ENABLE);		//ʹ��ʱ��
	GPIO_InitStructure.GPIO_Pin = PIN_DQ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;

	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //����Ϊ���ʱʹ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(PORT_DQ, &GPIO_InitStructure);
}


/*
*********************************************
*���ܣ�bs18b20��λ��⺯��
*˵����ʱ��1���������������� 480us��
			 2���ͷ����ߣ��ȴ�15us
			 3����ȡ�����Ƿ����ͣ���60--240us�ڶ�ȡ
*����ֵ��1�ɹ���0ʧ��			 
*********************************************
*/
uint8_t bsp_ds18b20Reset(void)
{

	uint8_t 		i	= 0;
	uint8_t 		temp = 0;

	bsp_configWireOut();							//����Ϊ���ģʽ
	DQ_0(); 										//�����

	DISABLE_INT();									//�ر�ȫ���ж�

	bsp_DelayUS(520);								//��ʱ520us

	DQ_1();
	bsp_DelayUS(10);

	bsp_configWireIn(); 							//��������Ϊ����ģʽ

	//temp = DQ_IN();
	//bsp_DelayUS(20);
	for (i = 0; i < 25; i++)
	{
		temp				= DQ_IN();
		bsp_DelayUS(10);

		if (0 == temp) //��break��continue֮�仨����һ��ʱ����bug
		{
			break;
		}
	}

	ENABLE_INT();									//����ȫ���ж�

	if (i == 25)
	{
		return 0;
	}

	bsp_DelayUS(600);
	return 1;
}


/*
***************************************
*���ܣ�������ds18b20����һ���ֽ�
*˵����ע��ʱ���ȷ����ֽ�
***************************************
*/
static void bsp_ds18b20WriteByte(uint8_t _data)
{
	uint8_t 		i	= 0;
	uint8_t 		temp = 0;

	bsp_configWireOut();							//����Ϊ���ģʽ

	for (i = 0; i < 8; i++)
	{
		temp				= _data &0x01;			//��ȡ���λ,�ȴ������λ
		DQ_0();
		bsp_DelayUS(2); 							//��ʱ10us

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
		_data				= _data >> 1;			//����������1λ
	}

	return;
}


/*
***************************************
*���ܣ�������ds18b20��ȡһ���ֽ�
***************************************
*/
static uint8_t bsp_ds18b20ReadByte(void)
{
	uint8_t 		data = 0;
	uint8_t 		i	= 0;
	uint8_t 		temp = 0;

	bsp_configWireOut();							//����Ϊ���ģʽ

	for (i = 0; i < 8; i++)
	{
		DQ_0(); 									//���� 
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
		bsp_configWireOut();						//����Ϊ���ģʽ
	}

	return data;
}


/*
*************************************
*���ܣ���ȡds18b20��Ψһ64λ���к�
*˵����ÿ�ζ�Ds18b20���в�������Ҫ���������������
	   1����λ
	   2������ROMָ��
	   3������RAMָ��
*�βΣ�_id�����ݴ��������
*����ֵ��0��ȡʧ�ܣ�1��ȡ�ɹ�
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

	bsp_ds18b20WriteByte(0x33); 					//��ȡ�ýڵ��64λ���к�

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

	bsp_ds18b20WriteByte(0xcc); 					//����ROM��ָ����0XCCH���� ��ָ��ֻ�ʺ���������ֻ��һ���ڵ㣻������ͨ�����������������ṩ64λROM���кŶ�ֱ�ӷ���RAM���Խ�ʡ����ʱ�䡣
	bsp_ds18b20WriteByte(0x44); 					//�¶�ת����ָ����0X44H��:����DS18B20�����¶�ת������������ڲ�RAM��

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
	bsp_ds18b20WriteByte(0xbe); 					//���ݴ�����ָ����0XBEH�������ݴ���9���ֽ����ݣ���ָ���RAM�ĵ�1���ֽڣ��ֽ�0����ʼ��ȡ��ֱ���Ÿ��ֽڣ��ֽ�8��CRCֵ��������Ϊֹ���������Ҫ���������ֽڵ����ݣ���ô�����������κ�ʱ�򷢳���λ�ź�����ֹ��������

	tempHigh			= bsp_ds18b20ReadByte();
	tempLow 			= bsp_ds18b20ReadByte();
	printf("the tempHigh = %X,tempLow = %X\r\n", tempHigh, tempLow);
	temperature 		= ((tempLow << 8) | tempHigh);
	return temperature;
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_Ds18b20ReadTempByID
*	����˵��: ��ָ��ID���¶ȼĴ�����ֵ��ԭʼ���ݣ�
*	��    ��: ��
*	�� �� ֵ: �¶ȼĴ������� ������16�õ� 1���϶ȵ�λ, Ҳ����С����ǰ�������)
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

	bsp_ds18b20WriteByte(0x55); 					/* ������ */

	for (i = 0; i < 8; i++)
	{
		bsp_ds18b20WriteByte(_id[i]);
	}

	bsp_ds18b20WriteByte(0x44); 					/* ��ת������ */
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

	bsp_ds18b20WriteByte(0x55); 					/* ������ */

	for (i = 0; i < 8; i++)
	{
		bsp_ds18b20WriteByte(_id[i]);
	}

	bsp_ds18b20WriteByte(0xbe);

	temp1				= bsp_ds18b20ReadByte();	/* ���¶�ֵ���ֽ� */
	temp2				= bsp_ds18b20ReadByte();	/* ���¶�ֵ���ֽ� */
	return ((temp2 << 8) | temp1); /* ����16λ�Ĵ���ֵ */
}


#if 0 //main���������ڵ��Դ���

if (0 == bsp_ds18b20ReadID(id))
{
	printf("δ��⵽ DS18B20 \r\n");
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
printf("reg = 0x%04X %6d --> %-4.04f��\r\n", (uint16_t) reg, reg, (float) reg / 16);
printf("\r\n");
break;

//reg = bsp_Ds18b20ReadTempByID(g_rom_id);
reg = bsp_ds18b20ReadTemperature();
printf("reg = 0x%04X %6d --> %-4.04f��\r\n", (uint16_t) reg, reg, (float) reg / 16);
printf("\r\n");


#endif

