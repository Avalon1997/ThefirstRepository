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
#include <stdio.h>
#include <string.h>
#include "sht30_i2c_driver.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
extern uint8_t I2CRXBuffer[6];
// uint8_t dataone[7] = {0x01,0x06,0x00,0x00,0x08,0x88,0x0c};
uint8_t rebuffer[10];
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int i = 0 ,x = 0;

//To the system
uint8_t U1_Spec_reset[] = {0x01,0x06,0x00,0x00,0x00,0x00,0x19,0x6F};      //reset the spectrometer
uint8_t U1_Spec_mac[] = {0x01,0x03,0x00,0x00,0x00,0x00,0x64,0x31};        //receive the MAC version
uint8_t U1_Spec_integ[] = {0x01,0x64};                                    //configure the integration time
uint8_t U1_Spec_vinteg[] = {0x01,0x65,0x00,0x00,0x00,0x00,0x19,0x6F};     //view the integration time
uint8_t U1_Spec_pul1[] = {0x01,0x68};                                     //configure the lamp pulse(command1)
uint8_t U1_Spec_pul2[] = {0x01,0x69};                                     //configure the lamp pulse(command2)
uint8_t U1_Spec_vpul[] = {0x01,0x6A,0x00,0x00,0x00,0x00,0x19,0x6F};       //view the lamp pulse
uint8_t U1_Spec_pix[] = {0x01,0x6B,0x00,0x00,0x07,0xFF,0x77,0x35};        //configure the pixel range
uint8_t U1_Spec_vpix[] = {0x01,0x6C,0x00,0x00,0x00,0x00,0x19,0x6F};       //view the pixel range
uint8_t U1_Spec_ave[] = {0x01,0x6E,0x00,0x14,0x00,0x00,0x19,0x6F};        //configure the average times
uint8_t U1_Spec_vave[] = {0x01,0x6F,0x00,0x00,0x00,0x00,0x19,0x6F};       //view the average times
uint8_t U1_Spec_get_par[] = {0x01,0x67,0x00,0x00,0x00,0x00,0x19,0x6F};    //get the calibration parameters
uint8_t U1_Spec_vwavelength[] = {0x01,0x6D,0x00,0x00,0x00,0x00,0x19,0x6F};//view all wavelength values corresponding to the current pixel range
uint8_t U1_Spec_darkdata[] = {0x01,0x86,0x00,0x00,0x00,0x00,0x19,0x6F};   //get the spectrometer data under dark current
uint8_t U1_Spec_referdata[] = {0x01,0x87,0x00,0x00,0x00,0x00,0x19,0x6F};  //get the spectrometer data under reference signal
uint8_t U1_Spec_samdata[] = {0x01,0x88,0x00,0x00,0x00,0x00,0x19,0x6F};    //get the spectrometer data under sample signal

//To the spectrometer
uint8_t U2_Spec_reset[] = {0x52,0xBD,0x3E};                                         //usart2 to reset the spectrometer
uint8_t U2_Spec_mac[] = {0x56,0x7E,0x3F};                                           //usart2 to receive the MAC version
uint8_t U2_Spec_integ[] = {0x69,0x00,0x00,0x01,0xF4,0x1E,0x78};                     //usart2 to Configure the integration time
uint8_t U2_Spec_vinteg[] = {0x3F,0x69,0x6E,0xD0};                                   //usart2 to view the integration time
uint8_t U2_Spec_pul[] = {0x30,0x00,0x00,0x27,0x10,0x00,0x04,0x93,0xE0,0x5C,0xB5};   //usart2 to configure the lamp pulse
uint8_t U2_Spec_vpul[] = {0x3F,0x30,0x54,0x10};                                     //usart2 to view the lamp pulse
uint8_t U2_Spec_pix[] = {0x50,0x00,0x03,0x00,0x00,0x07,0xFF,0x00,0x01,0x77,0x35};   //usart2 to configure the pixel range
uint8_t U2_Spec_vpix[] = {0x3F,0x50,0x7C,0x10};                                     //usart2 to view the pixel range
uint8_t U2_Spec_ave[] = {0x41,0x00,0x00,0xCA,0x21};                                 //usart2 to configure the average times
uint8_t U2_Spec_vave[] = {0x3F,0x41,0x70,0xD0};                                     //usart2 to view the average times
uint8_t U2_Spec_get_par[] = {0x78,0x62,0xBF};                                       //usart2 to get the calibration parameters
uint8_t U2_Spec_vwavelength[] = {0x3F,0x53,0x7D,0x50};                              //usart2 to view all wavelength values corresponding to the current pixel range
uint8_t U2_Spec_xenonoff[] = {0x31,0x00,0x20,0x14};                                 //usart2 to turn off the xenon
uint8_t U2_Spec_xenonon[] = {0x31,0x01,0xE0,0xD5};                                  //usart2 to turn of the xenon
uint8_t U2_Spec_getdata[] = {0x53,0x7D,0xFF};                                       //usart2 to get the spectrometer data

//
uint8_t ErrorUsart1[] = {0xFF,0XFF,0xFF,0XFF,0xFF,0XFF,0xFF,0XFF};      //USART1's data error.

uint8_t Spec_OK[] = {0x06,0x42,0x3F};                                     //spectrometer return data when the message is right.

uint8_t testmotor[] = {0xFF};


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
  memset(I2CRXBuffer,0,sizeof(I2CRXBuffer));

  // #ifdef __GNUC__
  // #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
  // #else
  // #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
  // #endif
  // PUTCHAR_PROTOTYPE
  // {
  //    //具体哪个串口可以更改USART1为其它串�?
  //    while ((USART1->SR & 0X40) == 0); //循环发�??,直到发�?�完�?
  //    USART1->DR = (uint8_t) ch;
  //    return ch;
  // }
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  //??????DMA
  HAL_UART_Receive_DMA(&huart1,Rx1_Buffer,Rx1BufferSize);
  HAL_UART_Receive_DMA(&huart2,Rx2_Buffer,Rx2BufferSize);
  //??PWM??
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,2500);
  HAL_Delay(500);
  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,1500);
  HAL_Delay(500);
  HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_GPIO_WritePin(GPIOC,LED_Pin,GPIO_PIN_SET);
    HAL_Delay(2000);

    // InsideTemperature();


    // //To reset the spectrometer ?? command one
    if (memcmp(Dec,U1_Spec_reset,sizeof(U1_Spec_reset)) == 0)
      {
        //
        HAL_UART_Transmit(&huart1,U2_Spec_reset,3,0xFFFF);

        while (memcmp(Data,Spec_OK,3) != 0)
        {
          HAL_UART_Transmit(&huart2,U2_Spec_reset,3,0xFFFF);
          HAL_Delay(50);
          HAL_GPIO_TogglePin(GPIOC,LED_Pin);
        }
        HAL_UART_Transmit(&huart1,Data,Rx2_lendemo,0xFFFF);
        Rx2_lendemo = 0;
        memset(Data,0,sizeof(Data));
        memset(Dec,0,sizeof(Dec));
      }
    
    //To view the MAC version ?? command two
    else if (memcmp(Dec,U1_Spec_mac,sizeof(U1_Spec_mac)) == 0)
      {
        HAL_UART_Transmit(&huart1,U2_Spec_mac,sizeof(U2_Spec_mac),0xFFFF);
        HAL_UART_Transmit(&huart2,U2_Spec_mac,sizeof(U2_Spec_mac),0xFFFF);
        // while (memcmp(Data,Spec_mac,23) != 0)
        // {        
        //   HAL_UART_Transmit(&huart2,U2_Spec_mac,3,0xFFFF);
        //   HAL_Delay(50);
        //   HAL_GPIO_TogglePin(GPIOC,LED_Pin);
        // }
        HAL_Delay(50);
        HAL_UART_Transmit(&huart1,Data,Rx2_lendemo,0xFFFF);
        Rx2_lendemo = 0;
        memset(Data,0,sizeof(Data));
        memset(Dec,0,sizeof(Dec));
      }
    
    //To configure the integration time ?? command three
    else if (memcmp(Dec,U1_Spec_integ,sizeof(U1_Spec_integ)) == 0)
      {
        U2_Spec_integ[1] = Dec[2];
        U2_Spec_integ[2] = Dec[3];
        U2_Spec_integ[3] = Dec[4];
        U2_Spec_integ[4] = Dec[5];
        HAL_UART_Transmit(&huart1,U2_Spec_integ,sizeof(U2_Spec_integ),0xFFFF);
        HAL_UART_Transmit(&huart2,U2_Spec_integ,sizeof(U2_Spec_integ),0xFFFF);
        // while(memcmp(Data,CRC,sizeof(CRC) != 0)
        // {}
        HAL_Delay(50);
        HAL_UART_Transmit(&huart1,Data,Rx2_lendemo,0xFFFF);
        Rx2_lendemo = 0;
        memset(Data,0,sizeof(Data));
        memset(Dec,0,sizeof(Dec));
      }

    //To view the integration time ?? command four
    else if (memcmp(Dec,U1_Spec_vinteg,sizeof(U1_Spec_vinteg)) == 0)
      {
        HAL_UART_Transmit(&huart1,U2_Spec_vinteg,sizeof(U2_Spec_vinteg),0xFFFF);
        HAL_UART_Transmit(&huart2,U2_Spec_vinteg,sizeof(U2_Spec_vinteg),0xFFFF);
        //while()
        //{}
        HAL_Delay(50);
        HAL_UART_Transmit(&huart1,Data,Rx2_lendemo,0xFFFF);
        Rx2_lendemo = 0;
        memset(Data,0,sizeof(Data));
        memset(Dec,0,sizeof(Dec));
      }

    //To configure the lamp pulse ?? command five
    else if (memcmp(Dec,U1_Spec_pul1,2) == 0)
      {
        if (Dec[8] == 0x01 && Dec[9] == 0x69)
        {
          U2_Spec_pul[1] = Dec[2];
          U2_Spec_pul[2] = Dec[3];
          U2_Spec_pul[3] = Dec[4];
          U2_Spec_pul[4] = Dec[5];
          U2_Spec_pul[5] = Dec[10];
          U2_Spec_pul[6] = Dec[11];
          U2_Spec_pul[7] = Dec[12];
          U2_Spec_pul[8] = Dec[13];
          HAL_UART_Transmit(&huart1,U2_Spec_pul,sizeof(U2_Spec_pul),0xFFFF);
          HAL_UART_Transmit(&huart2,U2_Spec_pul,sizeof(U2_Spec_pul),0xFFFF);
          //while()
          //{}
          HAL_Delay(50);
          HAL_UART_Transmit(&huart1,Data,Rx2_lendemo,0xFFFF);
          Rx2_lendemo = 0;
          memset(Data,0,sizeof(Data));
          memset(Dec,0,sizeof(Dec));
        }
      }    

    //To view the lamp pulse ?? command six
    else if (memcmp(Dec,U1_Spec_vpul,sizeof(U1_Spec_vpul)) == 0)
      {
        HAL_UART_Transmit(&huart1,U2_Spec_vpul,sizeof(U2_Spec_vpul),0xFFFF);
        HAL_UART_Transmit(&huart2,U2_Spec_vpul,sizeof(U2_Spec_vpul),0xFFFF);
        //while()
        //{}
        HAL_Delay(50);
        HAL_UART_Transmit(&huart1,Data,Rx2_lendemo,0xFFFF);
        Rx2_lendemo = 0;
        memset(Data,0,sizeof(Data));
        memset(Dec,0,sizeof(Dec));
      }

    //To configure the pixel range ?? command seven
    else if (memcmp(Dec,U1_Spec_pix,2) == 0)
      {
        U2_Spec_pix[3] = Dec[2];
        U2_Spec_pix[4] = Dec[3];
        U2_Spec_pix[5] = Dec[4];
        U2_Spec_pix[6] = Dec[5];
        HAL_UART_Transmit(&huart1,U2_Spec_pix,sizeof(U2_Spec_pix),0xFFFF);
        HAL_UART_Transmit(&huart2,U2_Spec_pix,sizeof(U2_Spec_pix),0xFFFF);
        //while()
        //{}
        HAL_Delay(50);
        HAL_UART_Transmit(&huart1,Data,Rx2_lendemo,0xFFFF);
        Rx2_lendemo = 0;
        memset(Data,0,sizeof(Data));
        memset(Dec,0,sizeof(Dec));
      }
    //To view the pixel range ?? command eight
    else if (memcmp(Dec,U1_Spec_vpix,sizeof(U1_Spec_vpix)) == 0)
      {
        HAL_UART_Transmit(&huart1,U2_Spec_vpix,sizeof(U2_Spec_vpix),0xFFFF);
        HAL_UART_Transmit(&huart2,U2_Spec_vpix,sizeof(U2_Spec_vpix),0xFFFF);
        //while()
        //{}
        HAL_Delay(50);
        HAL_UART_Transmit(&huart1,Data,Rx2_lendemo,0xFFFF);
        Rx2_lendemo = 0;
        memset(Data,0,sizeof(Data));
        memset(Dec,0,sizeof(Dec));
      }
    //To configure the average times ?? command nine
    else if (memcmp(Dec,U1_Spec_ave,2) == 0)
      {
        U2_Spec_ave[1] = Dec[2];
        U2_Spec_ave[2] = Dec[3];
        HAL_UART_Transmit(&huart1,U2_Spec_ave,sizeof(U2_Spec_ave),0xFFFF);
        HAL_UART_Transmit(&huart2,U2_Spec_ave,sizeof(U2_Spec_ave),0xFFFF);
        //while()
        //{}
        HAL_Delay(50);
        HAL_UART_Transmit(&huart1,Data,Rx2_lendemo,0xFFFF);
        Rx2_lendemo = 0;
        memset(Data,0,sizeof(Data));
        memset(Dec,0,sizeof(Dec));
      }
    //To view the average times ?? command ten
    else if (memcmp(Dec,U1_Spec_vave,sizeof(U1_Spec_vave)) == 0)
      {
        HAL_UART_Transmit(&huart1,U2_Spec_vave,sizeof(U2_Spec_vave),0xFFFF);
        HAL_UART_Transmit(&huart2,U2_Spec_vave,sizeof(U2_Spec_vave),0xFFFF);
        //while()
        //{}
        HAL_Delay(50);
        HAL_UART_Transmit(&huart1,Data,Rx2_lendemo,0xFFFF);
        Rx2_lendemo = 0;
        memset(Data,0,sizeof(Data));
        memset(Dec,0,sizeof(Dec));
      }
    //To get the calibration parameters ?? command eleven
    else if (memcmp(Dec,U1_Spec_get_par,sizeof(U1_Spec_get_par)) == 0)
      {
        HAL_UART_Transmit(&huart1,U2_Spec_get_par,sizeof(U2_Spec_get_par),0xFFFF);
        HAL_UART_Transmit(&huart2,U2_Spec_get_par,sizeof(U2_Spec_get_par),0xFFFF);
        //while()
        //{}
        HAL_Delay(50);
        HAL_UART_Transmit(&huart1,Data,Rx2_lendemo,0xFFFF);
        Rx2_lendemo = 0;
        memset(Data,0,sizeof(Data));
        memset(Dec,0,sizeof(Dec));
      }
    //To view all wavelength values corresponding to the current pixel range ?? command twelve
    else if (memcmp(Dec,U1_Spec_vwavelength,sizeof(U1_Spec_vwavelength)) == 0)
      {
        HAL_UART_Transmit(&huart1,U2_Spec_vwavelength,sizeof(U2_Spec_vwavelength),0xFFFF);
        HAL_UART_Transmit(&huart2,U2_Spec_vwavelength,sizeof(U2_Spec_vwavelength),0xFFFF);
        //while()
        //{}
        HAL_UART_DMAStop(&huart2);
        __HAL_UART_DISABLE_IT(&huart2,UART_IT_IDLE);
        HAL_UART_Receive(&huart2,Data,Rx2BufferSize,0x28A);
        //If the wavelength of two adjacent pixels is 0, then jump out of the loop to send data
        for (x=0;x<Rx2BufferSize;x++)
        {
          if (Data[x] == 0x00)
          {
            x += 1;
            if (Data[x] == 0x00)
            { x -= 1;
            break;
            }
          }
        }
        HAL_UART_Transmit(&huart1,Data,x,0xFFFF);
        Rx2_lendemo = 0;
        memset(Data,0,sizeof(Data));
        memset(Dec,0,sizeof(Dec));
        __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
        HAL_UART_Receive_DMA(&huart2,Data,Rx2BufferSize);
      }
    //To get the spectrometer data under dark current ?? command thirteen
    else if (memcmp(Dec,U1_Spec_darkdata,sizeof(U1_Spec_darkdata)) == 0)
      {
        HAL_UART_Transmit(&huart1,U2_Spec_xenonoff,sizeof(U2_Spec_xenonoff),0xFFFF);
        HAL_UART_Transmit(&huart2,U2_Spec_xenonoff,sizeof(U2_Spec_xenonoff),0xFFFF);
        HAL_Delay(20);
        HAL_UART_Transmit(&huart1,Data,Rx2_lendemo,0xFFFF);
        
        if (memcmp(Data,Spec_OK,sizeof(Spec_OK)) == 0)
        {
          Rx2_lendemo = 0;
          memset(Data,0,sizeof(Data));
          memset(Dec,0,sizeof(Dec));
          HAL_Delay(50);
          HAL_UART_DMAStop(&huart2);
          __HAL_UART_DISABLE_IT(&huart2,UART_IT_IDLE);
          HAL_UART_Transmit(&huart1,U2_Spec_getdata,sizeof(U2_Spec_getdata),0xFFFF);
          HAL_UART_Transmit(&huart2,U2_Spec_getdata,sizeof(U2_Spec_getdata),0xFFFF);


        HAL_UART_Receive(&huart2,Data,Rx2BufferSize,3000);
        //If the spectrometer data is 0 twice in a row, jump out of the loop and send the data
        for (x=0;x<Rx2BufferSize;x++)
        {
          if (Data[x] == 0x00)
          {
            x += 1;
            if (Data[x] == 0x00)
            { x -= 1;
            break;
            }
          }
        }
        HAL_UART_Transmit(&huart1,Data,x,0xFFFF);
        Rx2_lendemo = 0;
        memset(Data,0,sizeof(Data));
        memset(Dec,0,sizeof(Dec));
        __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
        HAL_UART_Receive_DMA(&huart2,Data,Rx2BufferSize);
        }
      }
    //To get the spectrometer data under reference signal ?? command fourteen
    else if (memcmp(Dec,U1_Spec_referdata,sizeof(U1_Spec_referdata)) == 0)
      {
        HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
        PWM_Reference();
        HAL_Delay(500);
        HAL_UART_Transmit(&huart1,U2_Spec_xenonon,sizeof(U2_Spec_xenonon),0xFFFF);
        HAL_UART_Transmit(&huart2,U2_Spec_xenonon,sizeof(U2_Spec_xenonon),0xFFFF);
        HAL_Delay(20);
        HAL_UART_Transmit(&huart1,Data,Rx2_lendemo,0xFFFF);

        if (memcmp(Data,Spec_OK,sizeof(Spec_OK)) == 0)
        {
          Rx2_lendemo = 0;
          memset(Data,0,sizeof(Data));
          memset(Dec,0,sizeof(Dec));
          HAL_Delay(50);
          HAL_UART_Transmit(&huart1,U2_Spec_getdata,sizeof(U2_Spec_getdata),0xFFFF);
          HAL_UART_Transmit(&huart2,U2_Spec_getdata,sizeof(U2_Spec_getdata),0xFFFF);

        HAL_UART_DMAStop(&huart2);
        __HAL_UART_DISABLE_IT(&huart2,UART_IT_IDLE);
        HAL_UART_Receive(&huart2,Data,Rx2BufferSize,3000);
        //If the spectrometer data is 0 twice in a row, jump out of the loop and send the data
        for (x=0;x<Rx2BufferSize;x++)
        {
          if (Data[x] == 0x00)
          {
            x += 1;
            if (Data[x] == 0x00)
            { x -= 1;
            break;
            }
          }
        }
        HAL_UART_Transmit(&huart1,Data,x,0xFFFF);
        Rx2_lendemo = 0;
        memset(Data,0,sizeof(Data));
        memset(Dec,0,sizeof(Dec));
        __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
        HAL_UART_Receive_DMA(&huart2,Data,Rx2BufferSize);
        }
        HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);
      }
    //To get the spectrometer data under reference signal ?? command fourteen
    else if (memcmp(Dec,U1_Spec_samdata,sizeof(U1_Spec_samdata)) == 0)
      {
        HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
        PWM_Sample();
        HAL_Delay(500);
        HAL_UART_Transmit(&huart1,U2_Spec_xenonon,sizeof(U2_Spec_xenonon),0xFFFF);
        HAL_UART_Transmit(&huart2,U2_Spec_xenonon,sizeof(U2_Spec_xenonon),0xFFFF);
        HAL_Delay(20);
        HAL_UART_Transmit(&huart1,Data,Rx2_lendemo,0xFFFF);

        if (memcmp(Data,Spec_OK,sizeof(Spec_OK)) == 0)
        {
          Rx2_lendemo = 0;
          memset(Data,0,sizeof(Data));
          memset(Dec,0,sizeof(Dec));
          HAL_Delay(50);
          HAL_UART_Transmit(&huart1,U2_Spec_getdata,sizeof(U2_Spec_getdata),0xFFFF);
          HAL_UART_Transmit(&huart2,U2_Spec_getdata,sizeof(U2_Spec_getdata),0xFFFF);

        HAL_UART_DMAStop(&huart2);
        __HAL_UART_DISABLE_IT(&huart2,UART_IT_IDLE);
        HAL_UART_Receive(&huart2,Data,Rx2BufferSize,3000);
        //If the spectrometer data is 0 twice in a row, jump out of the loop and send the data
        for (x=0;x<Rx2BufferSize;x++)
        {
          if (Data[x] == 0x00)
          {
            x += 1;
            if (Data[x] == 0x00)
            { x -= 1;
            break;
            }
          }
        }
        HAL_UART_Transmit(&huart1,Data,x,0xFFFF);
        Rx2_lendemo = 0;
        memset(Data,0,sizeof(Data));
        memset(Dec,0,sizeof(Dec));
        __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
        HAL_UART_Receive_DMA(&huart2,Data,Rx2BufferSize);
        }
        HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);
      }








    for (i=0;i<20;i++)
    {
      HAL_GPIO_TogglePin(GPIOC,LED_Pin);
      HAL_Delay(50);
    }
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

