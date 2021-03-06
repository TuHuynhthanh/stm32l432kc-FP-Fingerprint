
/**

  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_hal.h"
#include "usb_device.h"

/* USER CODE BEGIN Includes */

#include "VCPusb_device.h"
#include "usbd_cdc_if.h"
#include "USB_HID_KEYS.h"
#include "FPC1020/FPC1020_Communication.h"
#include "FPC1020/FPC1020Application.h"
#include "string.h"
#include "stdlib.h"

#define APP_RX_DATA_SIZE 1024
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim15;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_tx;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t rxBuff[1024]={0};volatile int rxIndex=0;
char st[100];
int g_nMaxFpCount=500;
UART_HandleTypeDef *debugTerminal=&huart2;
bool volatile rx2Flag;
uint32_t BuffLength;
int8_t configFlag;//select HID =0or VCP=1
uint32_t UserTxBufPtrIn = 0;/* Increment this pointer or roll it back to
                               start address when data are received over USART */
uint32_t UserTxBufPtrOut = 0; /* Increment this pointer or roll it back to
                                start address when data are sent over USB */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM15_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void sendHIDKey(uint8_t Key);
uint8_t convertNumbertoKeycode(uint8_t number);
extern uint8_t USBD_HID_SendReport(USBD_HandleTypeDef  *pdev, uint8_t *report, uint16_t len);
void playBeep(uint16_t interval);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */


int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  HAL_Delay(1000);
  MX_DMA_Init();
  MX_TIM15_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();

  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,1);
  if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)){
	  configFlag=0; // =0, hid
	  playBeep(250);
	  HAL_Delay(500);
	  MX_USB_DEVICE_Init();

  }
  else{

	  configFlag=1; //=1, VCP for config
	  MX_VCP_USB_DEVICE_Init();

  }


  /* USER CODE BEGIN 2 */




  /*char stDebug[50];
      int res;
      while(1)
      {

    	  res=Run_TestConnection();
    	  sprintf(stDebug,"res=%d\r\n",res);
    	  DEBUG(stDebug);
    	  HAL_Delay(500);
      }*/

  if(configFlag==1){ //HID mode
	  HAL_UART_Receive_IT(&huart1,&rxBuff[0],1);
	  HAL_TIM_Base_Start_IT(&htim15);
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	 /* uint8_t st[20];
	  sprintf(st,"PB3=%d",HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4));
	  HAL_UART_Transmit(&huart2,st,strlen(st),1000);
	  HAL_Delay(500);
*/
	  uint8_t stSo[20];

	  if(configFlag==0){
		  int kq;
		  kq=fingerIdentify();
		  if(kq>0){
			  playBeep(50);
			  sprintf(stSo,"%04i",kq);

			  sendHIDKey(stSo[0]);
			  sendHIDKey(stSo[1]);
			  sendHIDKey(stSo[2]);
			  sendHIDKey(stSo[3]);
			  sendHIDKey(10);
		  }
		  if(kq==-1)
		  {
			  playBeep(25);
			  HAL_Delay(100);
			  playBeep(25);
		  }
	  }

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_USB;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the main internal regulator output voltage 
    */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* TIM15 init function */
static void MX_TIM15_Init(void)
{

  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim15.Instance = TIM15;
  htim15.Init.Prescaler = 39999;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 50;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_ITR0;
  if (HAL_TIM_SlaveConfigSynchronization(&htim15, &sSlaveConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);

  /*Configure GPIO pin : PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void playBeep(uint16_t Interval){
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,0);
	HAL_Delay(Interval);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,1);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	UserTxBufPtrIn+=1;

	if(UserTxBufPtrIn==APP_RX_DATA_SIZE) UserTxBufPtrIn=0;

	HAL_UART_Receive_IT(huart,(uint8_t*)(rxBuff+UserTxBufPtrIn),1);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	uint32_t buffptr;
	  uint32_t buffsize;

	  if(UserTxBufPtrOut != UserTxBufPtrIn)
	  {
	    if(UserTxBufPtrOut > UserTxBufPtrIn) /* rollback */
	    {
	      buffsize = APP_RX_DATA_SIZE - UserTxBufPtrOut;
	    }
	    else
	    {
	      buffsize = UserTxBufPtrIn - UserTxBufPtrOut;
	    }

	    buffptr = UserTxBufPtrOut;



	    if(CDC_Transmit_FS(&rxBuff[buffptr],buffsize) == USBD_OK)
	    {
	      UserTxBufPtrOut += buffsize;
	      if (UserTxBufPtrOut == APP_RX_DATA_SIZE)
	      {
	        UserTxBufPtrOut = 0;
	      }
	    }
	  }
}

/*send one key on usb
 * key value=10 to send Enter key.
 * */

void sendHIDKey(uint8_t Key)
{
	uint8_t buffkey[8]={0};
	buffkey[0]=0;buffkey[1]=0;buffkey[2]=0;buffkey[3]=0;buffkey[4]=0;buffkey[5]=0;buffkey[6]=0;buffkey[7]=0;
	if(Key==10) buffkey[3]=KEY_ENTER; //key==10: enter key
	else buffkey[3]=convertNumbertoKeycode(Key);
	USBD_HID_SendReport(&hUsbDeviceFS,buffkey,8);
	HAL_Delay(50);
	buffkey[0]=0;buffkey[1]=0;buffkey[2]=0;buffkey[3]=0;buffkey[4]=0;buffkey[5]=0;buffkey[6]=0;buffkey[7]=0;
	USBD_HID_SendReport(&hUsbDeviceFS,buffkey,8);
	HAL_Delay(50);
}
/*
 * Convert number to Keycode
 * */
uint8_t convertNumbertoKeycode(uint8_t number)
{
	uint8_t keyCode=255;
	switch(number)
	{
	case 0:
	case 0x30:
		keyCode=KEY_0;break;
	case 1:
	case 0x31:
		keyCode=KEY_1;break;
	case 2:
	case 0x32:
		keyCode=KEY_2;break;
	case 3:
	case 0x33:
		keyCode=KEY_3;break;
	case 4:
	case 0x34:
		keyCode=KEY_4;break;
	case 5:
	case 0x35:
		keyCode=KEY_5;break;
	case 6:
	case 0x36:
		keyCode=KEY_6;break;
	case 7:
	case 0x37:
		keyCode=KEY_7;break;
	case 8:
	case 0x38:
		keyCode=KEY_8;break;
	case 9:
	case 0x39:
		keyCode=KEY_9;break;
	}
	return keyCode;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
