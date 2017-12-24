
#include "bsp.h"

//Ƭѡ�߶���
#define SD_SPI_CS_PIN			GPIO_Pin_10
#define SD_SPI_CS_PORT			GPIOA
#define SD_SPI_CS_ENABLE()		GPIO_ResetBits(SD_SPI_CS_PORT,SD_SPI_CS_PIN)
#define SD_SPI_CS_DISENABLE()	GPIO_SetBits(SD_SPI_CS_PORT,SD_SPI_CS_PIN)


#define SD_SPI					SPI1

#define SPI_BAUD				SPI_BaudRatePrescaler_256

uint8_t 		SD_Type = 0; //SD��������	


//SPIʱ��ʹ��
//#define ENABLE_SPI_RCC() RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE)
#define ENABLE_SPI_RCC()		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE)

/*******************************************************************************
* Function Name  : spiGpioInit
* Description	 : SPI1�������ų�ʼ��
* Input 		 : ��
* Output		 : None
* Return		 : None
*******************************************************************************/
static void spiGpioInit(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //ʹ��gpioʱ��

	//PB3 -- SPI1_SCK  
	//PB4 -- SPI1_MISO	
	//PB5 -- SPI1_MOSI 
	//��������ΪSPIģʽ���ù���
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

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //ʹ��gpioʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	//PB0 -- SPI5_SCK  
	//PA12 -- SPI5_MISO  
	//PA10 -- SPI5_MOSI 
	//��������ΪSPIģʽ���ù���
	GPIO_PinAFConfig(GPIOB, GPIO_Pin_0, GPIO_AF_SPI5);
	GPIO_PinAFConfig(GPIOA, GPIO_Pin_12, GPIO_AF_SPI5);
	GPIO_PinAFConfig(GPIOA, GPIO_Pin_10, GPIO_AF_SPI5);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//���������������е㲻��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;

	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif


	//��ʼ��CS����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = SD_SPI_CS_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SD_SPI_CS_PORT, &GPIO_InitStructure);

}


/*******************************************************************************
* Function Name  : spiHardInit
* Description	 : SPI1Ӳ����ʼ��
* Input 		 : ��
* Output		 : None
* Return		 : None
*******************************************************************************/
static void spiHardInit(void)
{
	SPI_InitTypeDef SPI_InitStructure;

	ENABLE_SPI_RCC();

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //���ݷ�������ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	//����ģʽ������ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //���ݳ��ȣ�8λ

	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; 	//ʱ�������زɼ�����
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//ʱ�ӵڶ������ز�������
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//Ƭѡ���Ʒ�ʽ���������

	//���ò����ʷ�Ƶϵ��
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BAUD; //������Ҫ�úý��м���
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //����λ������򣬸�λ�ȴ�
	SPI_InitStructure.SPI_CRCPolynomial = 7;		//CRC�Ĵ�������λ��Ϊ7����ʹ��
	SPI_Init(SD_SPI, &SPI_InitStructure);

	//SPI_Cmd(SD_SPI,DISABLE);	 //�Ƚ�ֹspi
	SPI_Cmd(SD_SPI, ENABLE);						//����SPI
}


/*******************************************************************************
* Function Name  : bsp_sdInit
* Description	 : SD��Ӳ����ʼ��������SPIʱ�ӣ��Լ�GPIOƬѡ��
* Input 		 : ��
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
* Description	 : SPI��дһ���ֽ�����
* Input 		 : ��Ҫ���͵�����
* Output		 : None
* Return		 : ���ܵ�������
*******************************************************************************/
uint8_t sd_ReadWriteByte(uint8_t ucData)
{
	while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_TXE) == RESET)
		; //�ȴ�����ȥ��

	SPI_I2S_SendData(SD_SPI, ucData);

	while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_RXNE) == RESET)
		; //�ȴ��������

	return SPI_I2S_ReceiveData(SD_SPI);
}


/*******************************************************************************
* Function Name  : sd_waitReady
* Description	 : �ȴ�SD��Ready
* Input 		 : None
* Output		 : None
* Return		 : uint8_t 
*					0�� �ɹ�
*					other��ʧ��
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
* Description	 : ��SD������һ������
* Input 		 : uint8_tcmd	���� 
*				   uint32 arg  �������
*				   uint8_t crc	 crcУ��ֵ
* Output		 : None
* Return		 : uint8_t SD�����ص���Ӧ
*******************************************************************************/
uint8_t sd_SendCommand(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	uint8_t 		ucData;
	uint32_t		uiRetry = 0;

	SD_SPI_CS_DISENABLE();
	sd_ReadWriteByte(0xff);
	SD_SPI_CS_ENABLE(); 							//Ƭѡ��Ϊ�ͣ�ʹ��

	//������������ ����ĵڶ�λһ��ҪΪ1 �����Զ��ڷ��͵�������Ҫ��0x40��һ�¡���֤�ڶ�λΪ1��
	sd_ReadWriteByte(cmd | 0x40);
	sd_ReadWriteByte((uint8_t) (arg >> 24));		//������31----24��
	sd_ReadWriteByte((uint8_t) (arg >> 16));		//������23----16��
	sd_ReadWriteByte((uint8_t) (arg >> 8)); 		//������15-----8��
	sd_ReadWriteByte((uint8_t) (arg));
	sd_ReadWriteByte(crc);

	//�ȴ���Ӧ����ʱ�˳�
	while ((ucData = sd_ReadWriteByte(0xff)) == 0xff)
	{
		uiRetry++;

		if (uiRetry > 800)
		{
			break;
		}
	}

	//�ر�Ƭѡ
	SD_SPI_CS_DISENABLE();


	sd_ReadWriteByte(0xff); 						//�������϶�������8��ʱ�ӣ���sd��������µĹ���
	return ucData;
}


/*******************************************************************************
* Function Name  : sd_SendCommandNoDeassert
* Description	 : ��SD������һ������ ���ǵ�����ʱ��Ҫʧ��Ƭѡ����Ϊ��������������Ҫ��
	��
* Input 		 : uint8_tcmd	���� 
*				   uint32 arg  �������
*				   uint8_t crc	 crcУ��ֵ
* Output		 : None
* Return		 : uint8_t SD�����ص���Ӧ
*******************************************************************************/
uint8_t sd_SendCommandNoDeassert(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	uint8_t 		ucData;
	uint32_t		uiRetry = 0;

	SD_SPI_CS_DISENABLE();
	sd_ReadWriteByte(0xff);
	SD_SPI_CS_ENABLE(); 							//Ƭѡ��Ϊ�ͣ�ʹ��

	//������������
	sd_ReadWriteByte(cmd | 0x40);
	sd_ReadWriteByte((uint8_t) (arg >> 24));		//������31----24��
	sd_ReadWriteByte((uint8_t) (arg >> 16));		//������23----16��
	sd_ReadWriteByte((uint8_t) (arg >> 8)); 		//������15-----8��
	sd_ReadWriteByte((uint8_t) (arg));
	sd_ReadWriteByte(crc);

	//�ȴ���Ӧ����ʱ�˳�
	while ((ucData = sd_ReadWriteByte(0xff)) == 0xff)
	{
		uiRetry++;

		if (uiRetry > 800)
		{
			break;
		}
	}

	//�ر�Ƭѡ
	//SD_SPI_CS_DISENABLE();
	//sd_ReadWriteByte(0xff);  //�������϶�������8��ʱ�ӣ���sd��������µĹ���
	return ucData;
}


/*******************************************************************************
* Function Name  : SD_Init
* Description	 : ��ʼ��SD��
* Input 		 : None
* Output		 : None
* Return		 : uint8_t 
*				   0��NO_ERR
*				   1��TIME_OUT
*				   99��NO_CARD
*******************************************************************************/
uint8_t SD_Init(void)
{
	uint16_t		i;								// ����ѭ������
	uint8_t 		r1; 							// ���SD���ķ���ֵ
	uint16_t		retry;							// �������г�ʱ����
	uint8_t 		buff[6];

	//SPI_ControlLine();
	//SPI_SetSpeed(SPI_SPEED_LOW);
	spiGpioInit();
	spiHardInit();
	SD_SPI_CS_ENABLE();
	printf("enter the SD_INIT\r\n");

	// ����ʱ���ȴ�SD���ϵ����
	for (i = 0; i < 0xf00; i++)
		;

	//�Ȳ�������74�����壬��SD���Լ���ʼ�����
	for (i = 0; i < 10; i++)
	{
		sd_ReadWriteByte(0xFF); 					//80clks
	}
	//-----------------SD����λ��idle��ʼ-----------------
	//ѭ����������CMD0��ֱ��SD������0x01,����IDLE״̬
	//��ʱ��ֱ���˳�
	retry				= 0;

	do 
	{
		//����CMD0����SD������IDLE״̬
		r1					= sd_SendCommand(CMD0, 0, 0x95);
		retry++;
	}
	while((r1 != 0x01) && (retry < 200));

	//����ѭ���󣬼��ԭ�򣺳�ʼ���ɹ���or ���Գ�ʱ��
	if (retry == 200)
		return 1; //��ʱ����1	

	printf("enter the SD_INIT3\r\n");

	//-----------------SD����λ��idle����-----------------	 
	//��ȡ��Ƭ��SD�汾��Ϣ
	r1					= sd_SendCommandNoDeassert(CMD8, 0x1aa, 0x87);

	//�����Ƭ�汾��Ϣ��v1.0�汾�ģ���r1=0x05����������³�ʼ��
	if (r1 == 0x05)
	{
		//���ÿ�����ΪSDV1.0����������⵽ΪMMC�������޸�ΪMMC
		SD_Type 			= SD_TYPE_V1;

		//�����V1.0����CMD8ָ���û�к�������
		//Ƭѡ�øߣ�������������
		SD_SPI_CS_DISENABLE();

		//�෢8��CLK����SD������������
		sd_ReadWriteByte(0xFF);

		//-----------------SD����MMC����ʼ����ʼ-----------------	 
		//������ʼ��ָ��CMD55+ACMD41
		// �����Ӧ��˵����SD�����ҳ�ʼ�����
		// û�л�Ӧ��˵����MMC�������������Ӧ��ʼ��
		retry				= 0;

		do 
		{
			//�ȷ�CMD55��Ӧ����0x01���������
			r1					= sd_SendCommand(CMD55, 0, 0);

			if (r1 != 0x01)
				return r1;

			//�õ���ȷ��Ӧ�󣬷�ACMD41��Ӧ�õ�����ֵ0x00����������400��
			r1					= sd_SendCommand(ACMD41, 0, 0);
			retry++;
		}
		while((r1 != 0x00) && (retry < 400));

		// �ж��ǳ�ʱ���ǵõ���ȷ��Ӧ
		// ���л�Ӧ����SD����û�л�Ӧ����MMC��
		//----------MMC�������ʼ��������ʼ------------
		if (retry == 400)
		{
			retry				= 0;

			//����MMC����ʼ�����û�в��ԣ�
			do 
			{
				r1					= sd_SendCommand(CMD1, 0, 0);
				retry++;
			}
			while((r1 != 0x00) && (retry < 400));

			if (retry == 400)
				return 1; //MMC����ʼ����ʱ			

			//д�뿨����
			SD_Type 			= SD_TYPE_MMC;
		}

		//----------MMC�������ʼ����������------------		
		//����SPIΪ����ģʽ
		//SPI_SetSpeed(SPI_SPEED_HIGH);   
		sd_ReadWriteByte(0xFF);

		//��ֹCRCУ��	   
		r1					= sd_SendCommand(CMD59, 0, 0x95);

		if (r1 != 0x00)
			return r1; //������󣬷���r1	   

		//����Sector Size
		r1					= sd_SendCommand(CMD16, 512, 0x95);

		if (r1 != 0x00)
			return r1; //������󣬷���r1		 

		//-----------------SD����MMC����ʼ������-----------------
	} //SD��ΪV1.0�汾�ĳ�ʼ������	 
	//������V2.0���ĳ�ʼ��
	//������Ҫ��ȡOCR���ݣ��ж���SD2.0����SD2.0HC��
	else if (r1 == 0x01)
	{
		//V2.0�Ŀ���CMD8�����ᴫ��4�ֽڵ����ݣ�Ҫ�����ٽ���������
		buff[0] 			= sd_ReadWriteByte(0xFF); //should be 0x00
		buff[1] 			= sd_ReadWriteByte(0xFF); //should be 0x00
		buff[2] 			= sd_ReadWriteByte(0xFF); //should be 0x01
		buff[3] 			= sd_ReadWriteByte(0xFF); //should be 0xAA
		printf("buff[0] = %x, buf[1] = %x,buf[2] = %x,buff[3] = %x\r\n", buff[0], buff[1], buff[2], buff[3]);
		SD_SPI_CS_DISENABLE();
		sd_ReadWriteByte(0xFF); 					//the next 8 clocks			 

		//�жϸÿ��Ƿ�֧��2.7V-3.6V�ĵ�ѹ��Χ
		if (buff[2] == 0x01 && buff[3] == 0xAA) //�粻�жϣ�����֧�ֵĿ�����
		{
			retry				= 0;

			//������ʼ��ָ��CMD55+ACMD41
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
					return r1; //��ʱ�򷵻�r1״̬ 
				}
			}
			while(r1 != 0);
			//��ʼ��ָ�����ɣ���������ȡOCR��Ϣ		   
			//-----------����SD2.0���汾��ʼ-----------
			r1					= sd_SendCommandNoDeassert(CMD58, 0, 0);

			if (r1 != 0x00)
				return r1; //�������û�з�����ȷӦ��ֱ���˳�������Ӧ��		 

			//��OCRָ����󣬽�������4�ֽڵ�OCR��Ϣ
			buff[0] 			= sd_ReadWriteByte(0xFF);
			buff[1] 			= sd_ReadWriteByte(0xFF);
			buff[2] 			= sd_ReadWriteByte(0xFF);
			buff[3] 			= sd_ReadWriteByte(0xFF);

			//OCR������ɣ�Ƭѡ�ø�
			SD_SPI_CS_DISENABLE();
			sd_ReadWriteByte(0xFF);
			printf("enter the SD_INITsssssss\r\n");

			//�����յ���OCR�е�bit30λ��CCS����ȷ����ΪSD2.0����SDHC
			//���CCS=1��SDHC   CCS=0��SD2.0
			if (buff[0] &0x40)
				SD_Type = SD_TYPE_V2HC; //���CCS	 
			else 
				SD_Type = SD_TYPE_V2;

			printf("the sd_type = %d\r\n", SD_Type);

			//-----------����SD2.0���汾����----------- 
			//����SPIΪ����ģʽ
			//SPI_SetSpeed(1);	
		}
	}
	return r1;
}




/*******************************************************************************
* Function Name  : SD_ReceiveData
* Description	 : ��SD���ж���ָ�����ȵ����ݣ������ڸ���λ��
* Input 		 : uint8_t *data(��Ŷ������ݵ��ڴ�>len)
*				   uint16_t len(���ݳ��ȣ�
*				   uint8_t release(������ɺ��Ƿ��ͷ�����CS�ø� 0�����ͷ� 1���ͷţ�
* Output		 : None
* Return		 : uint8_t 
*				   0��NO_ERR
*				   other��������Ϣ
*******************************************************************************/
uint8_t sd_ReceiveData(uint8_t * data, uint16_t len, uint8_t release)
{
	uint16_t retry;
	uint8_t r1;

	// ����һ�δ���
	SD_SPI_CS_ENABLE();

	//�ȴ�SD������������ʼ����0xFE
	retry				= 0;

	do 
	{
		r1					= sd_ReadWriteByte(0xFF);
		retry++;

		if (retry > 4000) //4000�εȴ���û��Ӧ���˳�����(����ʵ����ԣ��˴���ö��Լ���
		{
			SD_SPI_CS_DISENABLE();
			return 1;
		}
	}
	while(r1 != 0xFE);

	//��ʼ��������
	while (len--)
	{
		*data				= sd_ReadWriteByte(0xFF);
		data++;
	}

	//������2��αCRC��dummy CRC��
	sd_ReadWriteByte(0xFF);
	sd_ReadWriteByte(0xFF);

	//�����ͷ����ߣ���CS�ø�
	if (release == RELEASE)
	{
		//�������
		SD_SPI_CS_DISENABLE();
		sd_ReadWriteByte(0xFF);
	}

	return 0;
}


/*******************************************************************************
* Function Name  : SD_GetCID
* Description	 : ��ȡSD����CID��Ϣ��������������Ϣ
* Input 		 : uint8_t *cid_data(���CID���ڴ棬����16Byte��
* Output		 : None
* Return		 : uint8_t 
*				   0��NO_ERR
*				   1��TIME_OUT
*				   other��������Ϣ
*******************************************************************************/
uint8_t sd_GetCID(uint8_t * cid_data)
{
	uint8_t r1;

	//��CMD10�����CID
	r1					= sd_SendCommand(CMD10, 0, 0xFF);

	if (r1 != 0x00)
		return r1; //û������ȷӦ�����˳�������    

	//����16���ֽڵ�����
	sd_ReceiveData(cid_data, 16, RELEASE);
	return 0;
}


/*******************************************************************************
* Function Name  : SD_GetCSD
* Description	 : ��ȡSD����CSD��Ϣ�������������ٶ���Ϣ
* Input 		 : uint8_t *cid_data(���CID���ڴ棬����16Byte��
* Output		 : None
* Return		 : uint8_t 
*				   0��NO_ERR
*				   1��TIME_OUT
*				   other��������Ϣ
*******************************************************************************/
uint8_t sd_GetCSD(uint8_t * csd_data)
{
	uint8_t r1;

	//��CMD9�����CSD
	r1					= sd_SendCommand(CMD9, 0, 0xFF);

	if (r1 != 0x00)
		return r1; //û������ȷӦ�����˳�������  

	//����16���ֽڵ�����
	sd_ReceiveData(csd_data, 16, RELEASE);

	return 0;
}


/*******************************************************************************
* Function Name  : SD_GetCapacity
* Description	 : ��ȡSD�����������ֽڣ�
* Input 		 : None
* Output		 : None
* Return		 : u32 capacity 
*					0�� ȡ�������� 
*******************************************************************************/
uint32_t SD_GetCapacity(void)
{
	uint8_t csd[16];
	u32 Capacity;
	uint8_t r1;
	uint16_t i;
	uint16_t temp;

	//ȡCSD��Ϣ������ڼ��������0
	if (sd_GetCSD(csd) != 0)
		return 0;

	//���ΪSDHC�����������淽ʽ����
	if ((csd[0] &0xC0) == 0x40)
	{
		printf("��sd��ΪSDHC����\r\n");
		Capacity			= ((u32) csd[8]) << 8;
		Capacity			+= (u32)
		csd[9] +1;
		Capacity			= (Capacity) * 1024;	//�õ�������
		Capacity			*= 512; 				//�õ��ֽ���			   
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
		temp;										//�ֽ�Ϊ��λ	  
	}

	return (u32)
	Capacity;
}


/*******************************************************************************
* Function Name  : SD_ReadSingleBlock
* Description	 : ��SD����һ��block
* Input 		 : u32 sector ȡ��ַ��sectorֵ���������ַ�� 
*				   uint8_t *buffer ���ݴ洢��ַ����С����512byte�� 
* Output		 : None
* Return		 : uint8_t r1 
*					0�� �ɹ�
*					other��ʧ��
*******************************************************************************/
uint8_t sd_ReadSingleBlock(uint8_t * buffer,u32 sector,  uint32_t len)
{
	uint8_t r1;

	//����Ϊ����ģʽ
	//SPI_SetSpeed(SPI_SPEED_HIGH);
	//�������SDHC����sector��ַת��byte��ַ
	//sector				= sector << 9;

	r1					= sd_SendCommand(CMD17, sector, 1); //������

	if (r1 != 0x00)
	{
			printf("r1 ssss = %d\r\n",r1);
		  return r1;
	}
	r1					= sd_ReceiveData(buffer, len, RELEASE);
	printf("r1 ggggg  = %d\r\n",r1);
	if (r1 != 0)
		return r1; //�����ݳ���

	else 
		return 0;
}


/*******************************************************************************
* Function Name  : SD_WriteSingleBlock
* Description	 : д��SD����һ��block
* Input 		 : u32 sector ������ַ��sectorֵ���������ַ�� 
*				   uint8_t *buffer ���ݴ洢��ַ����С����512byte�� 
* Output		 : None
* Return		 : uint8_t r1 
*					0�� �ɹ�
*					other��ʧ��
*******************************************************************************/
uint8_t SD_WriteSingleBlock(u32 sector, uint8_t * data, uint32_t uiSize)
{
	uint8_t r1;
	uint16_t i;
	uint16_t retry;

	//����Ϊ����ģʽ
	//SPI_SetSpeed(SPI_SPEED_HIGH);
	//�������SDHC����������sector��ַ������ת����byte��ַ
	if (SD_Type != SD_TYPE_V2HC)
	{
		sector				= sector << 9;			//512*sector�����������ı߽�����ַ
	}

	r1					= sd_SendCommand(CMD24, sector, 0x01);
	
	if (r1 != 0x00)
	{
		printf("the r1 xxxx  = %d\r\n",r1);
		return r1; //Ӧ����ȷ��ֱ�ӷ���
	}

	//��ʼ׼�����ݴ���
	SD_SPI_CS_ENABLE();

	//�ȷ�3�������ݣ��ȴ�SD��׼����
	sd_ReadWriteByte(0xff);
	sd_ReadWriteByte(0xff);
	sd_ReadWriteByte(0xff);

	//����ʼ����0xFE
	sd_ReadWriteByte(0xFE);

	//��һ��sector������
	for (i = 0; i < uiSize; i++)
	{
		sd_ReadWriteByte(*data++);
	}

	//��2��Byte��dummy CRC
	sd_ReadWriteByte(0xff);
	sd_ReadWriteByte(0xff);

	//�ȴ�SD��Ӧ��
	r1					= sd_ReadWriteByte(0xff);

	if ((r1 & 0x1F) != 0x05)
	{
		SD_SPI_CS_DISENABLE();
		printf("the r1 xxbbbbbb  = %d\r\n",r1);
		//return r1;
	}

	//�ȴ��������
	retry				= 0;

	while (!sd_ReadWriteByte(0xff)) //���Ա��ʱ�������߱�����
	{
		retry++;

		if (retry > 65534) //�����ʱ��д��û����ɣ������˳�
		{
			SD_SPI_CS_DISENABLE();
			return 1; //д�볬ʱ����1
		}
	}

	//д����ɣ�Ƭѡ��1
	SD_SPI_CS_DISENABLE();
	sd_ReadWriteByte(0xff);

	return 0;
}


/*******************************************************************************
* Function Name  : SD_ReadMultiBlock
* Description	 : ��SD���Ķ��block
* Input 		 : u32 sector ȡ��ַ��sectorֵ���������ַ�� 
*				   uint8_t *buffer ���ݴ洢��ַ����С����512byte��
*				   uint8_t count ������count��block
* Output		 : None
* Return		 : uint8_t r1 
*					0�� �ɹ�
*					other��ʧ��
*******************************************************************************/
uint8_t SD_ReadMultiBlock(uint8_t * buffer,u32 sector,uint32_t len,  uint8_t count)
{
	uint8_t r1;

	//SPI_SetSpeed(SPI_SPEED_HIGH);//����Ϊ����ģʽ	
	sector				= sector << 9;				//�������SDHC����sector��ַת��byte��ַ

	//SD_WaitReady();
	//�����������
	r1					= sd_SendCommand(CMD18, sector, 1); //������

	if (r1 != 0x00)
		return r1;

	do //��ʼ��������
	{
		if (sd_ReceiveData(buffer, len, NO_RELEASE) != 0x00)
		{
			break;
		}

		buffer				+= len;
	}
	while(--count);

	//ȫ��������ϣ�����ֹͣ����
	sd_SendCommand(CMD12, 0, 1);

	//�ͷ�����
	SD_SPI_CS_DISENABLE();
	sd_ReadWriteByte(0xFF);

	if (count != 0)
		return count; //���û�д��꣬����ʣ�����	 

	else 
		return 0;
}


/*******************************************************************************
* Function Name  : SD_WriteMultiBlock
* Description	 : д��SD����N��block
* Input 		 : u32 sector ������ַ��sectorֵ���������ַ�� 
*				   uint8_t *buffer ���ݴ洢��ַ����С����512byte��
*				   uint8_t count д���block��Ŀ
* Output		 : None
* Return		 : uint8_t r1 
*					0�� �ɹ�
*					other��ʧ��
*******************************************************************************/
uint8_t SD_WriteMultiBlock(u32 sector, const uint8_t * data, uint32_t uiSize,uint8_t count)
{
	uint8_t r1;
	uint16_t i;

	//SPI_SetSpeed(SPI_SPEED_HIGH);//����Ϊ����ģʽ	 
	if (SD_Type != SD_TYPE_V2HC)
		sector = sector << 9; //�������SDHC����������sector��ַ������ת����byte��ַ  

	if (SD_Type != SD_TYPE_MMC)
		r1 = sd_SendCommand(ACMD23, count, 0x01); //���Ŀ�꿨����MMC��������ACMD23ָ��ʹ��Ԥ����   

	r1					= sd_SendCommand(CMD25, sector, 0x01); //�����д��ָ��

	if (r1 != 0x00)
		return r1; //Ӧ����ȷ��ֱ�ӷ���	 

	SD_SPI_CS_ENABLE(); 							//��ʼ׼�����ݴ���	 
	sd_ReadWriteByte(0xff); 						//�ȷ�3�������ݣ��ȴ�SD��׼����
	sd_ReadWriteByte(0xff);

	//--------������N��sectorд���ѭ������
	do 
	{
		//����ʼ����0xFC �����Ƕ��д��
		sd_ReadWriteByte(0xFC);

		//��һ��sector������
		for (i = 0; i < uiSize; i++)
		{
			sd_ReadWriteByte(*data++);
		}

		//��2��Byte��dummy CRC
		sd_ReadWriteByte(0xff);
		sd_ReadWriteByte(0xff);

		//�ȴ�SD��Ӧ��
		r1					= sd_ReadWriteByte(0xff);

		if ((r1 & 0x1F) != 0x05)
		{
			SD_SPI_CS_DISENABLE();					//���Ӧ��Ϊ��������������ֱ���˳�
			return r1;
		}

		//�ȴ�SD��д�����
		if (sd_WaitReady() == 1)
		{
			SD_SPI_CS_DISENABLE();					//�ȴ�SD��д����ɳ�ʱ��ֱ���˳�����
			return 1;
		}
	}
	while(--count);

	//��sector���ݴ������
	//��������������0xFD
	r1					= sd_ReadWriteByte(0xFD);

	if (r1 == 0x00)
	{
		count				= 0xfe;
	}

	if (sd_WaitReady()) //�ȴ�׼����
	{
		SD_SPI_CS_DISENABLE();
		return 1;
	}

	//д����ɣ�Ƭѡ��1
	SD_SPI_CS_DISENABLE();
	sd_ReadWriteByte(0xff);
	return count; //����countֵ�����д����count=0������count=1
}


/*******************************************************************************
* Function Name  : SD_Read_Bytes
* Description	 : ��ָ������,��offset��ʼ����bytes���ֽ�
* Input 		 : u32 address ������ַ��sectorֵ���������ַ�� 
*				   uint8_t *buf 	���ݴ洢��ַ����С<=512byte��
*				   uint16_t offset	�����������ƫ����
				   uint16_t bytes	Ҫ�������ֽ���
* Output		 : None
* Return		 : uint8_t r1 
*					0�� �ɹ�
*					other��ʧ��
*******************************************************************************/
uint8_t SD_Read_Bytes(unsigned long address, unsigned char * buf, unsigned int offset, unsigned int bytes)
{
	uint8_t r1;
	uint16_t i			= 0;
	r1					= sd_SendCommand(CMD17, address << 9, 1); //���Ͷ���������	   

	if (r1 != 0x00)
		return r1; //Ӧ����ȷ��ֱ�ӷ���

	SD_SPI_CS_ENABLE(); 								//ѡ��SD��

	while (sd_ReadWriteByte(0xff) != 0xFE) //ֱ����ȡ�������ݵĿ�ʼͷ0XFE���ż���
	{
		i++;

		if (i > 3000)
		{
			SD_SPI_CS_DISENABLE();					//�ر�SD��
			return 1; //��ȡʧ��
		}
	};

	for (i = 0; i < offset; i++)
		sd_ReadWriteByte(0xff); //����offsetλ 

	for (; i < offset + bytes; i++)
		*buf++ = sd_ReadWriteByte(0xff); //��ȡ��������	

	for (; i < 512; i++)
		sd_ReadWriteByte(0xff); //����ʣ���ֽ�

	sd_ReadWriteByte(0xff); 						//����αCRC��
	sd_ReadWriteByte(0xff);
	SD_SPI_CS_DISENABLE();							//�ر�SD��
	return 0;
}


