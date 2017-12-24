

/*命令应答
R1:（置1有效），用于应答大部分命令的
7  6  5  4	3  2  1  0
0  |  |  |	|  |  |  |
   |  |  |	|  |  |  |______禁止状态（正在执行处理）
   |  |  |	|  |  |_________檫除复位
   |  |  |	|  |____________非法命令
   |  |  |	|_______________CRC错误
   |  |  |__________________檫除序列错误
   |  |_____________________地址错误
   |________________________参数错误

R1b：和R1有相同的格式，但添加了附加的忙信号。忙信号时连续8位为一组，全0时为忙，非
	全零时为准备好

写数据应答:
7  6  5  4	3  2  1  0
x  x  x  0	Status	 1
Status="010":数据确认  "101":由于CRC错误被抛弃  "110":由于写入错误被抛弃

发送和接收的数据格式：（1）对于单块读、单块写和多块读：StartBlock（0xfe）+UserData（最?
	??512字节）+CRC（2Bytes）
							多块读通过发送CMD12来停止读取数据
					  （2）对于多块写：0xfc+UserData（最大512字节）+CRC（2Bytes)。发送StopTran（
	0xfd）来停止写入数据

读数据失败时的错误数据标识：（1Byte）7	6  5  4  3	2  1  0
									 0	0  0  0  |	|  |  |___执行错误
												 |	|  |______卡内控制器错误
												 |	|_________卡内ECC校验错误
												 |____________地址超出卡的范围
*/
#ifndef _BSP_SPISD_
#define _BSP_SPISD_

#include "bsp.h"


typedef enum
{
/**
  * @brief  SDIO specific error defines
  */
  SD_CMD_CRC_FAIL                    = (1), /*!< Command response received (but CRC check failed) */
  SD_DATA_CRC_FAIL                   = (2), /*!< Data bock sent/received (CRC check Failed) */
  SD_CMD_RSP_TIMEOUT                 = (3), /*!< Command response timeout */
  SD_DATA_TIMEOUT                    = (4), /*!< Data time out */
  SD_TX_UNDERRUN                     = (5), /*!< Transmit FIFO under-run */
  SD_RX_OVERRUN                      = (6), /*!< Receive FIFO over-run */
  SD_START_BIT_ERR                   = (7), /*!< Start bit not detected on all data signals in widE bus mode */
  SD_CMD_OUT_OF_RANGE                = (8), /*!< CMD's argument was out of range.*/
  SD_ADDR_MISALIGNED                 = (9), /*!< Misaligned address */
  SD_BLOCK_LEN_ERR                   = (10), /*!< Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length */
  SD_ERASE_SEQ_ERR                   = (11), /*!< An error in the sequence of erase command occurs.*/
  SD_BAD_ERASE_PARAM                 = (12), /*!< An Invalid selection for erase groups */
  SD_WRITE_PROT_VIOLATION            = (13), /*!< Attempt to program a write protect block */
  SD_LOCK_UNLOCK_FAILED              = (14), /*!< Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card */
  SD_COM_CRC_FAILED                  = (15), /*!< CRC check of the previous command failed */
  SD_ILLEGAL_CMD                     = (16), /*!< Command is not legal for the card state */
  SD_CARD_ECC_FAILED                 = (17), /*!< Card internal ECC was applied but failed to correct the data */
  SD_CC_ERROR                        = (18), /*!< Internal card controller error */
  SD_GENERAL_UNKNOWN_ERROR           = (19), /*!< General or Unknown error */
  SD_STREAM_READ_UNDERRUN            = (20), /*!< The card could not sustain data transfer in stream read operation. */
  SD_STREAM_WRITE_OVERRUN            = (21), /*!< The card could not sustain data programming in stream mode */
  SD_CID_CSD_OVERWRITE               = (22), /*!< CID/CSD overwrite error */
  SD_WP_ERASE_SKIP                   = (23), /*!< only partial address space was erased */
  SD_CARD_ECC_DISABLED               = (24), /*!< Command has been executed without using internal ECC */
  SD_ERASE_RESET                     = (25), /*!< Erase sequence was cleared before executing because an out of erase sequence command was received */
  SD_AKE_SEQ_ERROR                   = (26), /*!< Error in sequence of authentication. */
  SD_INVALID_VOLTRANGE               = (27),
  SD_ADDR_OUT_OF_RANGE               = (28),
  SD_SWITCH_ERROR                    = (29),
  SD_SDIO_DISABLED                   = (30),
  SD_SDIO_FUNCTION_BUSY              = (31),
  SD_SDIO_FUNCTION_FAILED            = (32),
  SD_SDIO_UNKNOWN_FUNCTION           = (33),

/**
  * @brief  Standard error defines
  */
  SD_INTERNAL_ERROR,
  SD_NOT_CONFIGURED,
  SD_REQUEST_PENDING,
  SD_REQUEST_NOT_APPLICABLE,
  SD_INVALID_PARAMETER,
  SD_UNSUPPORTED_FEATURE,
  SD_UNSUPPORTED_HW,
  SD_ERROR,
  SD_OK = 0
} SD_Error;

/**
  * @brief  SDIO Transfer state
  */
typedef enum
{
  SD_TRANSFER_OK  = 0,
  SD_TRANSFER_BUSY = 1,
  SD_TRANSFER_ERROR
} SDTransferState;

/**
  * @brief  SD Card States
  */
typedef enum
{
  SD_CARD_READY                  = ((uint32_t)0x00000001),
  SD_CARD_IDENTIFICATION         = ((uint32_t)0x00000002),
  SD_CARD_STANDBY                = ((uint32_t)0x00000003),
  SD_CARD_TRANSFER               = ((uint32_t)0x00000004),
  SD_CARD_SENDING                = ((uint32_t)0x00000005),
  SD_CARD_RECEIVING              = ((uint32_t)0x00000006),
  SD_CARD_PROGRAMMING            = ((uint32_t)0x00000007),
  SD_CARD_DISCONNECTED           = ((uint32_t)0x00000008),
  SD_CARD_ERROR                  = ((uint32_t)0x000000FF)
}SDCardState;


/**
  * @brief  Card Specific Data: CSD Register
  */
typedef struct
{
  __IO uint8_t  CSDStruct;            /*!< CSD structure */
  __IO uint8_t  SysSpecVersion;       /*!< System specification version */
  __IO uint8_t  Reserved1;            /*!< Reserved */
  __IO uint8_t  TAAC;                 /*!< Data read access-time 1 */
  __IO uint8_t  NSAC;                 /*!< Data read access-time 2 in CLK cycles */
  __IO uint8_t  MaxBusClkFrec;        /*!< Max. bus clock frequency */
  __IO uint16_t CardComdClasses;      /*!< Card command classes */
  __IO uint8_t  RdBlockLen;           /*!< Max. read data block length */
  __IO uint8_t  PartBlockRead;        /*!< Partial blocks for read allowed */
  __IO uint8_t  WrBlockMisalign;      /*!< Write block misalignment */
  __IO uint8_t  RdBlockMisalign;      /*!< Read block misalignment */
  __IO uint8_t  DSRImpl;              /*!< DSR implemented */
  __IO uint8_t  Reserved2;            /*!< Reserved */
  __IO uint32_t DeviceSize;           /*!< Device Size */
  __IO uint8_t  MaxRdCurrentVDDMin;   /*!< Max. read current @ VDD min */
  __IO uint8_t  MaxRdCurrentVDDMax;   /*!< Max. read current @ VDD max */
  __IO uint8_t  MaxWrCurrentVDDMin;   /*!< Max. write current @ VDD min */
  __IO uint8_t  MaxWrCurrentVDDMax;   /*!< Max. write current @ VDD max */
  __IO uint8_t  DeviceSizeMul;        /*!< Device size multiplier */
  __IO uint8_t  EraseGrSize;          /*!< Erase group size */
  __IO uint8_t  EraseGrMul;           /*!< Erase group size multiplier */
  __IO uint8_t  WrProtectGrSize;      /*!< Write protect group size */
  __IO uint8_t  WrProtectGrEnable;    /*!< Write protect group enable */
  __IO uint8_t  ManDeflECC;           /*!< Manufacturer default ECC */
  __IO uint8_t  WrSpeedFact;          /*!< Write speed factor */
  __IO uint8_t  MaxWrBlockLen;        /*!< Max. write data block length */
  __IO uint8_t  WriteBlockPaPartial;  /*!< Partial blocks for write allowed */
  __IO uint8_t  Reserved3;            /*!< Reserded */
  __IO uint8_t  ContentProtectAppli;  /*!< Content protection application */
  __IO uint8_t  FileFormatGrouop;     /*!< File format group */
  __IO uint8_t  CopyFlag;             /*!< Copy flag (OTP) */
  __IO uint8_t  PermWrProtect;        /*!< Permanent write protection */
  __IO uint8_t  TempWrProtect;        /*!< Temporary write protection */
  __IO uint8_t  FileFormat;           /*!< File Format */
  __IO uint8_t  ECC;                  /*!< ECC code */
  __IO uint8_t  CSD_CRC;              /*!< CSD CRC */
  __IO uint8_t  Reserved4;            /*!< always 1*/
} SD_CSD;

/**
  * @brief  Card Identification Data: CID Register
  */
typedef struct
{
  __IO uint8_t  ManufacturerID;       /*!< ManufacturerID */
  __IO uint16_t OEM_AppliID;          /*!< OEM/Application ID */
  __IO uint32_t ProdName1;            /*!< Product Name part1 */
  __IO uint8_t  ProdName2;            /*!< Product Name part2*/
  __IO uint8_t  ProdRev;              /*!< Product Revision */
  __IO uint32_t ProdSN;               /*!< Product Serial Number */
  __IO uint8_t  Reserved1;            /*!< Reserved1 */
  __IO uint16_t ManufactDate;         /*!< Manufacturing Date */
  __IO uint8_t  CID_CRC;              /*!< CID CRC */
  __IO uint8_t  Reserved2;            /*!< always 1 */
} SD_CID;

/**
  * @brief SD Card Status
  */
typedef struct
{
  __IO uint8_t DAT_BUS_WIDTH;
  __IO uint8_t SECURED_MODE;
  __IO uint16_t SD_CARD_TYPE;
  __IO uint32_t SIZE_OF_PROTECTED_AREA;
  __IO uint8_t SPEED_CLASS;
  __IO uint8_t PERFORMANCE_MOVE;
  __IO uint8_t AU_SIZE;
  __IO uint16_t ERASE_SIZE;
  __IO uint8_t ERASE_TIMEOUT;
  __IO uint8_t ERASE_OFFSET;
} SD_CardStatus;


/**
  * @brief SD Card information
  */
typedef struct
{
  SD_CSD SD_csd;
  SD_CID SD_cid;
  uint64_t CardCapacity;  /*!< Card Capacity */
  uint32_t CardBlockSize; /*!< Card Block Size */
  uint16_t RCA;
  uint8_t CardType;
} SD_CardInfo;




/* SD卡类型定义 */
#define SD_TYPE_MMC 			0
#define SD_TYPE_V1				1
#define SD_TYPE_V2				2
#define SD_TYPE_V2HC			4

/* SPI总线速度设置*/
#define SPI_SPEED_LOW			0
#define SPI_SPEED_HIGH			1

/* SD传输数据结束后是否释放总线宏定义 */
#define NO_RELEASE				0
#define RELEASE 				1


/* SD卡指令表 */
#define CMD0					0	   //卡复位	(应答格式：R1)
#define CMD1					1	   //MMC卡开始初始化
#define CMD8					8	  //识别卡的版本
#define CMD9					9	  //命令9 ，读CSD数据   (应答格式：R1)
#define CMD10					10	   //命令10，读CID数据   (应答格式：R1)
#define CMD12					12		//命令12，停止数据传输    (应答格式：R1b)
#define CMD16					16		//命令16，设置SectorSize 应返回0x00   (应答格式：R1)
#define CMD17					17		//命令17，读sector	  (应答格式：R1)
#define CMD18					18		//命令18，读Multi sector	(应答格式：R1)
#define ACMD23					23		//命令23，设置多sector写入前预先擦除N个block	  (应答格式：R1)
#define CMD24					24		//命令24，写sector	  (应答格式：R1)
#define CMD25					25	   //命令25，写Multi sector    (应答格式：R1)
#define ACMD41					41		//命令41，应返回0x00	  (应答格式：R1)
#define CMD55					55		//命令55，应返回0x01	  (应答格式：R1)
#define CMD58					58		//命令58，读OCR信息	  (应答格式：R1)
#define CMD59					59		//命令59，使能/禁止CRC，应返回0x00	 (应答格式：R1)

uint8_t SD_Init(void);
uint8_t sd_GetCSD(uint8_t * csd_data);
uint8_t sd_GetCID(uint8_t * cid_data);
void bsp_sdInit(void);
uint8_t sd_ReadWriteByte(uint8_t ucData);

uint32_t SD_GetCapacity(void);
uint8_t SD_ReadMultiBlock(uint8_t * buffer,u32 sector,uint32_t len,  uint8_t count);

uint8_t sd_ReadSingleBlock(uint8_t * buffer,u32 sector,  uint32_t len);

uint8_t SD_WriteSingleBlock(u32 sector, uint8_t * data, uint32_t uiSize);
uint8_t SD_WriteMultiBlock(u32 sector, const uint8_t * data, uint32_t uiSize,uint8_t count);
uint8_t SD_Read_Bytes(unsigned long address, unsigned char * buf, unsigned int offset, unsigned int bytes);





#endif

