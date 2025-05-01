/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Serial.h"
#include "dfminiplayer.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
struct DFPlayerMiniData dfpl;
#define UART_BUFFER 10
uint8_t Rx3Data[UART_BUFFER];
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart5;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_uart5_tx;
DMA_HandleTypeDef hdma_uart5_rx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM3_Init(void);
static void MX_UART5_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void printDetail(uint8_t type, int value);
void initmodules();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart )
{
  Serial_pushData(Rx3Data, UART_BUFFER);
	Serial_print(&huart2," %s","R: " );
	for( int i = 0; i < UART_BUFFER; i++ ){
		Serial_print(&huart2,"%1X ",Rx3Data[i] );
	}
	Serial_print(&huart2," %s","\n" );
	HAL_UART_Receive_DMA(huart, Rx3Data,UART_BUFFER);
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial_print(&huart2, "%s\n","Time Out!");
      break;
    case WrongStack:
      Serial_print(&huart2, "%s\n","Stack Wrong!");
      break;
    case DFPlayerCardInserted:
      Serial_print(&huart2, "%s\n","Card Inserted!");
      break;
    case DFPlayerCardRemoved:
      Serial_print(&huart2, "%s\n","Card Removed!");
      break;
    case DFPlayerCardOnline:
      Serial_print(&huart2, "%s\n","Card Online!");
      break;
    case DFPlayerUSBInserted:
      Serial_print(&huart2, "%s\n","USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
    Serial_print(&huart2, "%s\n","USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial_print(&huart2, "Number %d Play Finished!\n", value);
      break;
    case DFPlayerError:
      Serial_print(&huart2, "%s\n","DFPlayerError:");
      switch (value) {
        case Busy:
          Serial_print(&huart2, "%s\n","Card not found");
          break;
        case Sleeping:
          Serial_print(&huart2, "%s\n","Sleeping");
          break;
        case SerialWrongStack:
          Serial_print(&huart2, "%s\n","Get Wrong Stack");
          break;
        case CheckSumNotMatch:
          Serial_print(&huart2, "%s\n","Check Sum Not Match");
          break;
        case FileIndexOut:
          Serial_print(&huart2, "%s\n","File Index Out of Bound");
          break;
        case FileMismatch:
          Serial_print(&huart2, "%s\n","Cannot Find File");
          break;
        case Advertise:
          Serial_print(&huart2, "%s\n", "In Advertise");
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  
}

void initmodules()
{
      dfpl._timeOutDuration = 500;
      dfpl._sending[0] = 0x7E; //{0x7E, 0xFF, 06, 00, 01, 00, 00, 00, 00, 0xEF};
      dfpl._sending[1] = 0xFF;
      dfpl._sending[2] = 06;
      dfpl._sending[3] = 00;
      dfpl._sending[4] = 01;
      dfpl._sending[5] = 00;
      dfpl._sending[6] = 00;
      dfpl._sending[7] = 00;
      dfpl._sending[8] = 00;
      dfpl._sending[9] = 0xEF;
      dfpl._receivedIndex = 0;
      dfpl.device = DFPLAYER_DEVICE_SD;
      dfpl._isAvailable = false;
      dfpl._isSending = false;
      dfpl.df_uart = huart5;
      dfpl.debug_uart = huart2;
      dfpl.dfBusyPin = BUSY_PIN_Pin;
      dfpl.dfBusyPort = BUSY_PIN_GPIO_Port;

      Serial_print(&huart2, "%s","\n");
      Serial_print(&huart2, "%s","DFRobot DFPlayer Mini Demo on STM32F\n");
      Serial_print(&huart2, "%s","Initializing DFPlayer\n");
  
  if (!DFPLayerMini_begin(&dfpl,true,true)) {  //Use serial to communicate with mp3.
    Serial_print(&huart2, "%s","Unable to begin:\n");
    Serial_print(&huart2, "%s","1.Please recheck the connection!\n");
    Serial_print(&huart2, "%s","2.Please insert the SD card!\n");
    while(true);
  }
  Serial_print(&huart2, "%s","DFPlayer Mini online.");
  
  DFPLayerMini_setTimeOut(&dfpl,500); //Set serial communictaion time out 500ms
  
  //----Set volume----
  DFPLayerMini_volume(&dfpl,10);  //Set volume value (0~30).
  DFPLayerMini_volumeUp(&dfpl); //Volume Up
  DFPLayerMini_volumeDown(&dfpl); //Volume Down
  
  //----Set different EQ----
  DFPLayerMini_EQ(&dfpl,DFPLAYER_EQ_NORMAL);
//  DFPLayerMini_EQ(&dfpl,DFPLAYER_EQ_POP);
//  DFPLayerMini_EQ(&dfpl,DFPLAYER_EQ_ROCK);
//  DFPLayerMini_EQ(&dfpl,DFPLAYER_EQ_JAZZ);
//  DFPLayerMini_EQ(&dfpl,DFPLAYER_EQ_CLASSIC);
//  DFPLayerMini_EQ(&dfpl,DFPLAYER_EQ_BASS);
  
  //----Set device we use SD as default----
//  DFPLayerMini_outputDevice(DFPLAYER_DEVICE_U_DISK);
  DFPLayerMini_outputDevice(&dfpl,DFPLAYER_DEVICE_SD);
//  DFPLayerMini_outputDevice(DFPLAYER_DEVICE_AUX);
//  DFPLayerMini_outputDevice(DFPLAYER_DEVICE_SLEEP);
//  DFPLayerMini_outputDevice(DFPLAYER_DEVICE_FLASH);
  
  //----Mp3 control----
//  DFPLayerMini_sleep();     //sleep
//  DFPLayerMini_reset();     //Reset the module
//  DFPLayerMini_enableDAC();  //Enable On-chip DAC
//  DFPLayerMini_disableDAC();  //Disable On-chip DAC
//  DFPLayerMini_outputSetting(true, 15); //output setting, enable the output and set the gain to 15
  
  //----Mp3 play----
  DFPLayerMini_next(&dfpl);  //Play next mp3
  HAL_Delay(1000);
  DFPLayerMini_previous(&dfpl);  //Play previous mp3
  HAL_Delay(1000);
  DFPLayerMini_play(&dfpl,1);  //Play the first mp3
  HAL_Delay(1000);
  DFPLayerMini_loop(&dfpl, 1);  //Loop the first mp3
  HAL_Delay(1000);
  DFPLayerMini_pause(&dfpl);  //pause the mp3
  HAL_Delay(1000);
  DFPLayerMini_start(&dfpl);  //start the mp3 from the pause
  HAL_Delay(1000);
  //DFPLayerMini_playFolder(1, 1);  //play specific mp3 in SD:/1/001.mp3; Folder Name(1~99); File Name(1~255)
  //HAL_Delay(1000);
  DFPLayerMini_enableLoopAll(&dfpl); //loop all mp3 files.
  HAL_Delay(1000);
  DFPLayerMini_disableLoopAll(&dfpl); //stop loop all mp3 files.
  HAL_Delay(1000);
  DFPLayerMini_playMp3Folder(&dfpl,4); //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
  HAL_Delay(1000);
  DFPLayerMini_advertise(&dfpl,3); //advertise specific mp3 in SD:/ADVERT/0003.mp3; File Name(0~65535)
  HAL_Delay(1000);
  DFPLayerMini_stopAdvertise(&dfpl); //stop advertise
  HAL_Delay(1000);
  //DFPLayerMini_playLargeFolder(&dfpl,2, 999); //play specific mp3 in SD:/02/004.mp3; Folder Name(1~10); File Name(1~1000)
  //HAL_Delay(1000);
  DFPLayerMini_loopFolder(&dfpl, 5); //loop all mp3 files in folder SD:/05.
  HAL_Delay(1000);
  DFPLayerMini_randomAll(&dfpl); //Random play all the mp3.
  HAL_Delay(1000);
  DFPLayerMini_enableLoop(&dfpl); //enable loop.
  HAL_Delay(1000);
  DFPLayerMini_disableLoop(&dfpl); //disable loop.
  HAL_Delay(1000);

  //----Read imformation----
  Serial_print(&huart2, "read State: %d\n", DFPLayerMini_readState(&dfpl)); //read mp3 state
  Serial_print(&huart2, "read Volume: %d\n", DFPLayerMini_readVolume(&dfpl)); //read current volume
  Serial_print(&huart2, "read EQ: %d\n", DFPLayerMini_readEQ(&dfpl)); //read EQ setting
  Serial_print(&huart2, "read File Counts: %d\n", DFPLayerMini_readFileCounts(&dfpl)); //read all file counts in SD card
  Serial_print(&huart2, "read Current File Number: %d\n", DFPLayerMini_readCurrentFileNumber(&dfpl)); //read current play file number
  //Serial_print(&huart2, "read File Counts in Folder: %d\n", DFPLayerMini_readFileCountsInFolder(&dfpl,3)); //read file counts in folder SD:/03

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_DMA_Init();
  MX_TIM3_Init();
  MX_UART5_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim3);
  HAL_UART_Receive_DMA(&huart5, Rx3Data, UART_BUFFER);
  initmodules();
  uint32_t timer = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
  
    if (HAL_GetTick() - timer > 10000) {
      timer = HAL_GetTick();
      DFPLayerMini_next(&dfpl);  //Play next mp3 every 10 second.
    }
    
    if (DFPLayerMini_available(&dfpl)) {
      printDetail(DFPLayerMini_readType(&dfpl), DFPLayerMini_read(&dfpl)); //Print the detail message from DFPlayer to handle different errors and states.
    }
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 180-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535 -1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 9600;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BUSY_PIN_Pin */
  GPIO_InitStruct.Pin = BUSY_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUSY_PIN_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
