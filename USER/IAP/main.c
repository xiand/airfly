

/**
  ******************************************************************************
  * @file	 STM32F4xx_IAP/src/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date	 10-October-2011
  * @brief	 Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/** @addtogroup STM32F4xx_IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "menu.h"
#include "bsp.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;

/* Private function prototypes -----------------------------------------------*/
static void IAP_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief	Main program.
  * @param	None
  * @retval None
  */
int main(void)
{
	uint8_t ucKeyCode = 0;
	uint8_t ucFlag = 100;
	/* Unlock the Flash Program Erase controller */
	FLASH_If_Init();
	bsp_Init();
	//定义一个500ms的定时器
	bsp_StartAutoTimer(0, 2000);

	while(1)
	{
		if (bsp_CheckTimer(0))
		{
			LED_TOGGLE();
		}

		ucKeyCode			= bsp_GetKey();

		if (ucKeyCode != KEY_NONE)
		{
			switch (ucKeyCode)
			{
				case KEY_1_SHORTPRESS:
					LED(OFF);
					ucFlag = 1;
					break;

				case KEY_1_LONGPRESS:
					LED(ON);
					ucFlag = 2;
					break;

				case KEY_1_DOUBLEPRESS:
					ucFlag = 100;
					break;

				default:
					
					break;
			}
		}
		if(ucFlag != 100)
		{
			break;
		}
	}
	/* Test if Key push-button on STM324xG-EVAL Board is pressed */
	if(2 == ucFlag)
	{
		/* Execute the IAP driver in order to reprogram the Flash */
		IAP_Init();

		/* Display main menu */
		Main_Menu();
	}

	/* Keep the user application running */
	else 
	{
		/* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
		if (((* (__IO uint32_t *) APPLICATION_ADDRESS) & 0x2FFE0000) == 0x20000000)
		{
			/* Jump to user application */
			JumpAddress 		= * (__IO uint32_t *) (APPLICATION_ADDRESS + 4);
			Jump_To_Application = (pFunction)
			JumpAddress;

			/* Initialize user application's Stack Pointer */
			__set_MSP(* (__IO uint32_t *) APPLICATION_ADDRESS);
			Jump_To_Application();
		}
	}

	while (1)
	{
	}
}


/**
  * @brief	Initialize the IAP: Configure USART.
  * @param	None
  * @retval None
  */
void IAP_Init(void)
{
	bsp_iap_uart_init();
}


#ifdef USE_FULL_ASSERT

/**
  * @brief	Reports the name of the source file and the source line number
  * 		where the assert_param error has occurred.
  * @param	file: pointer to the source file name
  * @param	line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t * file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}


#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
