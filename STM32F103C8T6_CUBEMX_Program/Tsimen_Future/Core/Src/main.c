/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sht30_i2c_driver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

//----- Miscellaneous variable definitions
int CRC_j = 0, CRC_i = 0, WHILEA = 0;
uint16_t crc = 0xFFFF, CRC16 = 0;
uint8_t CRC_DATA[8] = {0};
int PWM_DARK = 1700, PWM_REFERENCE = 2050, PWM_SAMPLE = 2450;


//----- Master computer communication command (11 instructions)
uint8_t USART1_Tsimen_Reset[]     = {0x01,0x01,0x00,0x00,0x00,0x00,0x0A,0x3C};    // Global reset
uint8_t USART1_Tsimen_Ver[]       = {0x01,0x02,0x00,0x00,0x00,0x00,0x0A,0x78};    // Read the sensor version number
uint8_t USART1_Tsimen_Int[]       = {0x01,0x03,0x00,0x00,0x01,0xF4,0xDD,0x45};    // Set the spectrometer integration time (example)
uint8_t USART1_Tsimen_CheckInt[]  = {0x01,0x04,0x00,0x00,0x00,0x00,0x0A,0xF0};    // Check the current integration time of the spectrometer
uint8_t USART1_Tsimen_Ave[]       = {0x01,0x05,0x00,0x32,0x00,0x00,0x05,0x6C};    // Set the spectrometer average Times (example)
uint8_t USART1_Tsimen_CheckAve[]  = {0x01,0x06,0x00,0x00,0x00,0x00,0xCA,0x89};    // Check the current average times of the spectrometer
uint8_t USART1_Tsimen_DarSignal[] = {0x01,0x07,0x00,0x00,0x00,0x00,0x0A,0xB4};    // Read the spectrometer data under dark current conditions
uint8_t USART1_Tsimen_RefSignal[] = {0x01,0x08,0x00,0x00,0x00,0x00,0x0B,0xE0};    // Read the spectrometer data under reference signal conditions
uint8_t USART1_Tsimen_SamSignal[] = {0x01,0x09,0x00,0x00,0x00,0x00,0xCB,0xDD};    // Read the spectrometer data under sample signal conditions
uint8_t USART1_Tsimen_FullData[]  = {0x01,0x0A,0x00,0x00,0x00,0x00,0xCB,0x99};    // One-click acquisition of dark, parametric and sample spectrometer data
uint8_t USART1_Tsimen_TempData[]  = {0x01,0x0B,0x00,0x00,0x00,0x00,0x0B,0xA4};    // Read temperature and humidity data
uint8_t USART1_Tsimen_OK[]        = {0x01,0x80,0x7E};                             // OK code
uint8_t USART1_Tsimen_ERROR[]     = {0xFF,0x00,0xFF};                             // ERROR code
uint8_t USART1_Tsimen_SpecERROR[] = {0xFF,0XFF,0XFF,0xFF,0xFF,0XFF,0XFF,0xFF};    // Spectrometer has some problems, maybe it is broken

//----- Spectrometer communication command ()
uint8_t USART2_Spec_Reset[]       = {0x52,0xDB,0x3E};                             // Spec reset
uint8_t USART2_Spec_Int[]         = {0x69,0x00,0x00,0x01,0xF4,0x1E,0x78};         // Set spec int time
uint8_t USART2_Spec_CheckInt[]    = {0x3F,0x69,0x6E,0xD0};                        // Check spec int time
uint8_t USART2_Spec_Ave[]         = {0x41,0x00,0x32,0x01,0xA0};                   // Set spec average
uint8_t USART2_Spec_CheckAve[]    = {0x3F,0x41,0x70,0xD0};                        // Check spec average
uint8_t USART2_Spec_XenonOff[]    = {0x31,0x00,0x20,0x14};                        // Turn the xenon off
uint8_t USART2_Spec_XenonOnCon[]  = {0x31,0x01,0xE0,0xD5};                        // Turn the xenon on under continuous mode
uint8_t USART2_Spec_XenonOnOne[]  = {0x31,0x81,0x40,0xD4};                        // Turn the xenon on under signal mode
uint8_t USART2_Spec_Data[]        = {0x53,0x7D,0xFF};                             // Read spec data
uint8_t USART2_Spec_OK[]          = {0x06,0x42,0x3F};                             // OK code
uint8_t USART2_Spec_ERROR[]       = {0x15,0x8F,0x7E};                             // ERROR code



























/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

/*--------------------------------------------------Start the usart DMA-------------------------------------------------------*/
  HAL_UART_Receive_DMA(&huart1,USART_RX1_BUFFER,RX1BUFFERSIZE);
  HAL_UART_Receive_DMA(&huart2,USART_RX2_BUFFER,RX2BUFFERSIZE);


  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
  // __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,500);
  // HAL_Delay(1500);
  // __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,2500);
  // HAL_Delay(1500);

  HAL_TIM_Base_Start_IT(&htim3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    //----- Working LED
    HAL_GPIO_TogglePin(GPIOC,LED_Pin);
    HAL_Delay(100);


    /*--------------------------------------------------Main Part-------------------------------------------------------*/

    //----- command one: Global reset
    if (memcmp(DATA_CACHE1,USART1_Tsimen_Reset,sizeof(USART1_Tsimen_Reset)) == RESET)
    {
      CRC16 = ModBus_CRC16(DATA_CACHE1,6);
      if (DATA_CACHE1[7]==(uint8_t)CRC16&0xFF && DATA_CACHE1[6]==(uint8_t)(CRC16>>8)&0xFF)
      {
        CRC16 = 0;
        
        PWM_PulseWidth(PWM_DARK);
        
        HAL_UART_Transmit(&huart2,USART2_Spec_Reset,sizeof(USART2_Spec_Reset),0xFFFF);
        
        HAL_Delay(1500);
        while (WHILEA < 10)
        {
          if (memcmp(DATA_CACHE2,USART2_Spec_OK,3) ==  RESET)
          {
            HAL_UART_Transmit(&huart1,USART1_Tsimen_OK,sizeof(USART1_Tsimen_OK),0xFFFF);
            break;
          }
          else if (memcmp(DATA_CACHE2,USART2_Spec_ERROR,3) == RESET)
          {
            HAL_UART_Transmit(&huart1,USART1_Tsimen_ERROR,sizeof(USART1_Tsimen_ERROR),0xFFFF);
            break;
          }
          WHILEA++;
        }

        if (WHILEA == 10)
        {
          HAL_UART_Transmit(&huart1,USART1_Tsimen_SpecERROR,sizeof(USART1_Tsimen_SpecERROR),0xFFFF);
        }
        
        WHILEA = 0;
        USART_RX2_LENDEMO = 0;
        memset(DATA_CACHE1,0,RX1BUFFERSIZE);
        memset(DATA_CACHE2,0,RX2BUFFERSIZE);

      }

    }








    // __set_PRIMASK(1);
    // HAL_NVIC_SystemReset();












    /* USER CODE END WHILE */

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
 * @brief 
 * 
 * @param htim 
 */
void HAL_TIM_PeiodElapsedCallback(TIM_HandleTypeDef *htim)
{
  printf("Callback test OK !\r\n");
  // Measure_TR_Test();
  // InsideTemperature_Test();
}



/**
 * @brief CRC16 check code. The function is used for data verification when communicating with PC.
 *  
 * @param pdata 
 * @param len 
 * @return uint16_t 
 */
uint16_t ModBus_CRC16( uint8_t *pdata, int len)     //polynomial: 8005
{
  crc = 0xFFFF;
  for ( CRC_j=0; CRC_j<len;CRC_j++)
  {
    crc=crc^pdata[CRC_j];
    for ( CRC_i=0; CRC_i<8; CRC_i++)
    {
      if( ( crc&0x0001) >0)
      {
        crc=crc>>1;
        crc=crc^ 0xA001;                            //We take 8005 in reverse.
      }
    else
    crc=crc>>1;
    }
  }
return crc;
}








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

