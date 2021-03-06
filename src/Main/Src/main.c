/**
  ******************************************************************************
  * @file    USB_Device/HID_Standalone/Src/main.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    3-July-2015
  * @brief   USB device HID demo main file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CURSOR_STEP     5
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBD_HandleTypeDef USBD_Device;
extern uint8_t UserTxBuffer[2048];
extern uint32_t UserTxBufPtrIn;
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
/* Private functions ---------------------------------------------------------*/

UART_HandleTypeDef UartHandle;
extern int spi_nor_erase(int addr ,int len);
extern int spi_nor_read(int from, int len,
			int *retlen, unsigned char *buf);
extern int spi_nor_write(int to, int len,
		int *retlen, const unsigned char *buf);
extern int spi_nor_read_id();
extern void SWO_Enable( void );
int uart_send(unsigned char data)
{
	return HAL_UART_Transmit(&UartHandle,(uint8_t *)&data,1,100);
}
int uart_read()
{
	char data;
	HAL_UART_Receive(&UartHandle,(uint8_t *)&data,1,100);
	return data;
}
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	uint8_t HID_Buffer[256],HID_Buffer1[256];
	int id,ret,i,j=0;
	/* STM32L1xx HAL library initialization:
	- Configure the Flash prefetch
	- Systick timer is configured by default as source of time base, but user 
	can eventually implement his proper time base source (a general purpose 
	timer for example or other time source), keeping in mind that Time base 
	duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
	handled in milliseconds basis.
	- Set NVIC Group Priority to 4
	- Low Level Initialization
	*/

	HAL_Init();
	/* Initialize LED2 */
	BSP_LED_Init(LED2);

	/* Configure the system clock to 32 MHz */
	SystemClock_Config();
	UartHandle.Instance		 = USARTx;

	UartHandle.Init.BaudRate   = 115200;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity	   = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode	   = UART_MODE_TX_RX;
	HAL_UART_DeInit(&UartHandle);
	HAL_UART_Init(&UartHandle);
	SWO_Enable();
	printf("in main\n");

	/* Configure Key button for remote wakeup */
	BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

	/* Init Device Library */
	//USBD_Init(&USBD_Device, &HID_Desc, 0);

	/* Register the HID class */
	//USBD_RegisterClass(&USBD_Device, USBD_HID_CLASS);

	/* Start Device Process */
	//USBD_Start(&USBD_Device);
	/* Init Device Library */
  USBD_Init(&USBD_Device, &VCP_Desc, 0);
  
  /* Add Supported Class */
  USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS);
  
  /* Add CDC Interface Class */
  USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);
  
  /* Start Device Process */
  USBD_Start(&USBD_Device);
	if((id=spi_nor_read_id())==0)
		while(1)
		{
			printf("read id failed\n");
			spi_nor_read_id();
			HAL_Delay(100);
		}
	if(spi_nor_erase(0,64*1024*64))
	{
		while(1)
		{
			printf("spi_nor_erase failed\n");
			HAL_Delay(100);
		}
	}
	for(i=0;i<256;i++)
		HID_Buffer[i]='5';
	memset(HID_Buffer1,0,256);
	for(i=0;i<4096;i=i+256)
	{
		if(spi_nor_write(i,256,&ret,HID_Buffer))
		{
			while(1)
			{
				printf("spi_nor_write failed\n");
				HAL_Delay(100);  
			}
		}
	}
	
	while(1)
	{
		memset(HID_Buffer1,0x23,256);
		spi_nor_read(j,256,&ret,HID_Buffer1);
		//printf("ret read ID %x\n",id);
		if(j<(4096-256))
			j+=256;
		else
			j=0;
		for(i=0;i<256;i++)
		{
			if(HID_Buffer1[i]!='5')
			printf("%x ",HID_Buffer1[i]);
			
		}
		
	  /* To avoid buffer overflow */
	  if(UserTxBufPtrIn+256 > 2048)
	  {	  
		memcpy(UserTxBuffer+UserTxBufPtrIn,HID_Buffer1,2048-UserTxBufPtrIn);
		memcpy(UserTxBuffer,HID_Buffer1+2048-UserTxBufPtrIn,UserTxBufPtrIn+256-2048);
	    UserTxBufPtrIn = UserTxBufPtrIn+256-2048;
	  }
	  else
	  {
	  	memcpy(UserTxBuffer+UserTxBufPtrIn,HID_Buffer1,256);
	  	UserTxBufPtrIn+=256;
	  }
		//printf("send bytes");
     
		//HAL_Delay(1000);
	}
	spi_nor_read(0,255,&ret,HID_Buffer1);
	printf("ret read %d\n",ret);
	for(i=0;i<255;i++)
		printf("%d ",HID_Buffer1[i]);
	printf("\n");
	while (1)
	{
		/* Insert delay 100 ms */
		HAL_Delay(100);  
		BSP_LED_Toggle(LED2);
		HAL_Delay(100);  
	//	GetPointerData(HID_Buffer);
//		USBD_HID_SendReport(&USBD_Device, HID_Buffer, 4);
		spi_nor_read(0,255,&ret,HID_Buffer1);
		printf("ret read ID %x\n",id);
		//for(id=0;id<255;id++)
		//printf("%d ",HID_Buffer1[id]);
		//printf("\n");
	}
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 32000000
  *            HCLK(Hz)                       = 32000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSI Frequency(Hz)              = 16000000
  *            HSE PREDIV                     = 1
  *            PLLMUL                         = 12 (if HSE) or 6 (if HSI)
  *            PLLDIV                         = 3
  *            Flash Latency(WS)              = 1
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable HSI Oscillator and Activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState            = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL          = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PLLDIV          = RCC_PLL_DIV3;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Set Voltage scale1 as MCU will run at 32MHz */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
  clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
}
void Error_Handler(void)
{
  /* Turn LED3 on */
  BSP_LED_On(1);
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
