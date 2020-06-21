/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "delay.h"
#include <stdlib.h>
#include "oled12864.h"
#include "inside_flash.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t USART_RxBuffer[1];
uint8_t USART_TxBuffer[] = "ok";

uint32_t AD_DMA_1 = 0; // 保存ADC1数据
char AD_STR[4];
// rtc 时间
char time_[] = "00:00:00";
RTC_TimeTypeDef stimestructure;
RTC_AlarmTypeDef alarmValue;
// crc 测试用
// static const uint32_t aDataBuffer[114] =
// {
//   0x00001021, 0x20423063, 0x408450a5, 0x60c670e7, 0x9129a14a, 0xb16bc18c,
//   0xd1ade1ce, 0xf1ef1231, 0x32732252, 0x52b54294, 0x72f762d6, 0x93398318,
//   0xa35ad3bd, 0xc39cf3ff, 0xe3de2462, 0x34430420, 0x64e674c7, 0x44a45485,
//   0xa56ab54b, 0x85289509, 0xf5cfc5ac, 0xd58d3653, 0x26721611, 0x063076d7,
//   0x569546b4, 0xb75ba77a, 0x97198738, 0xf7dfe7fe, 0xc7bc48c4, 0x58e56886,
//   0x78a70840, 0x18612802, 0xc9ccd9ed, 0xe98ef9af, 0x89489969, 0xa90ab92b,
//   0x4ad47ab7, 0x6a961a71, 0x0a503a33, 0x2a12dbfd, 0xfbbfeb9e, 0x9b798b58,
//   0xbb3bab1a, 0x6ca67c87, 0x5cc52c22, 0x3c030c60, 0x1c41edae, 0xfd8fcdec,
//   0xad2abd0b, 0x8d689d49, 0x7e976eb6, 0x5ed54ef4, 0x2e321e51, 0x0e70ff9f,
//   0xefbedfdd, 0xcffcbf1b, 0x9f598f78, 0x918881a9, 0xb1caa1eb, 0xd10cc12d,
//   0xe16f1080, 0x00a130c2, 0x20e35004, 0x40257046, 0x83b99398, 0xa3fbb3da,
//   0xc33dd31c, 0xe37ff35e, 0x129022f3, 0x32d24235, 0x52146277, 0x7256b5ea,
//   0x95a88589, 0xf56ee54f, 0xd52cc50d, 0x34e224c3, 0x04817466, 0x64475424,
//   0x4405a7db, 0xb7fa8799, 0xe75ff77e, 0xc71dd73c, 0x26d336f2, 0x069116b0,
//   0x76764615, 0x5634d94c, 0xc96df90e, 0xe92f99c8, 0xb98aa9ab, 0x58444865,
//   0x78066827, 0x18c008e1, 0x28a3cb7d, 0xdb5ceb3f, 0xfb1e8bf9, 0x9bd8abbb,
//   0x4a755a54, 0x6a377a16, 0x0af11ad0, 0x2ab33a92, 0xed0fdd6c, 0xcd4dbdaa,
//   0xad8b9de8, 0x8dc97c26, 0x5c644c45, 0x3ca22c83, 0x1ce00cc1, 0xef1fff3e,
//   0xdf7caf9b, 0xbfba8fd9, 0x9ff86e17, 0x7e364e55, 0x2e933eb2, 0x0ed11ef0
// };
// uint32_t uwExpectedCRCValue = 0x379E9F06;

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
  Delay_Init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_DMA(&huart1, USART_RxBuffer, 1); // 打开串口1 DMA的接收使能
                                                    // HAL_UART_Transmit_DMA(&huart1,USART_TxBuffer,sizeof(USART_TxBuffer));  // DMA发送数据
  printf("本系统由耶稣基督教徒龚宇开发\r\n");
  printf("2020年4月5日开始设计\r\n");
  printf("串口1初始化完成\r\n");

  SSD1306_Init();                      // OLED12864 初始化
  HAL_ADCEx_Calibration_Start(&hadc1); // 开启ADC校准

  HAL_RTCEx_SetSecond_IT(&hrtc);  // 开启秒中断

  // HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);  // 在初始化中添加打开定时器的encoder模式

  // SSD1306_DrawLine(0, 0, 128, 0, SSD1306_COLOR_WHITE);
  // SSD1306_DrawLine2(0, 0, 128, 64, SSD1306_COLOR_WHITE);
  // SSD1306_DrawRectangle(0, 2, 20, 20, SSD1306_COLOR_WHITE);
  // SSD1306_DrawFilledRectangle(22, 2, 20, 20, SSD1306_COLOR_WHITE);
  // SSD1306_GotoXY(0, 0);
  // SSD1306_Puts("Christian idrnyu", &Font_6x8, SSD1306_COLOR_WHITE, initial);
  // SSD1306_GotoXY(0, 8);
  // SSD1306_Puts("Christian YU", &Font_8x16, SSD1306_COLOR_WHITE, bold);
  // SSD1306_GotoXY(0, 24);
  // SSD1306_Puts("! 0 1 2 3 4 5", &Font_8x16, SSD1306_COLOR_WHITE, initial);
  // SSD1306_GotoXY(0, 0);
  // OLED_ShowText("我爱耶稣", SSD1306_COLOR_WHITE, initial);
  // SSD1306_GotoXY(0, 16);
  // OLED_ShowText("我爱耶稣", SSD1306_COLOR_WHITE, bold);
  // SSD1306_UpdateScreen(); // 更新显示
  // OLED_Scroll_Display(0, 7, LEFT);

  // writeFlash(0x08007000, 0x55555555);
	// readFlash(0x08007000);
  // while(1);

  // crc 校验
  // if(HAL_CRC_Accumulate(&hcrc, (uint32_t *)aDataBuffer, 114) != uwExpectedCRCValue)
  //     printf("\n\r CRC wrong value  crc校验不成立 \n\r");
  // else
  //     printf("\n\r CRC right value  crc校验成功 \n\r");
  // while(1);

  setAlarm(0, 0, 10);  // 多少时分秒后中断

  SSD1306_GotoXY(90, 0);
  SSD1306_Puts("M", &Font_6x8, SSD1306_COLOR_WHITE, initial);
  // SSD1306_Puts("A", &Font_6x8, SSD1306_COLOR_WHITE, initial);

  SSD1306_DrawLine2(0, 9, 128, 9, SSD1306_COLOR_WHITE);  // 画线

  SSD1306_DrawPixel(51, 8, SSD1306_COLOR_WHITE);  // 打点1
  SSD1306_DrawPixel(51, 7, SSD1306_COLOR_WHITE);  // 打点1
  SSD1306_DrawPixel(51, 6, SSD1306_COLOR_WHITE);  // 打点1

  SSD1306_DrawPixel(86, 8, SSD1306_COLOR_WHITE);  // 打点2
  SSD1306_DrawPixel(86, 7, SSD1306_COLOR_WHITE);  // 打点2
  SSD1306_DrawPixel(86, 6, SSD1306_COLOR_WHITE);  // 打点2

  SSD1306_DrawPixel(100, 8, SSD1306_COLOR_WHITE);  // 打点3
  SSD1306_DrawPixel(100, 7, SSD1306_COLOR_WHITE);  // 打点3
  SSD1306_DrawPixel(100, 6, SSD1306_COLOR_WHITE);  // 打点3

  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // printf("编码器：%d\r\n", (uint32_t)(__HAL_TIM_GET_COUNTER(&htim2)));
    // Delay_ms(2000);

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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_BYPASS;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// 最后加上一个串口接收函数的回调函数，把接收到的数据再发出去。
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  HAL_UART_Transmit(&huart1, USART_RxBuffer, 1, 0); // 串口发送数据
}

//闹钟中断回调函数
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{ 
  printf("闹钟中断回调!!\r\n");
}
// rtc 秒中断回调函数
void HAL_RTCEx_RTCEventCallback (RTC_HandleTypeDef *hrtc)
{
  // 获取RTC当前时间，必须先获取时间
  // HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
  HAL_RTC_GetTime(hrtc, &stimestructure, RTC_FORMAT_BIN);
  sprintf(time_, "%02d:%02d:%02d", stimestructure.Hours, stimestructure.Minutes, stimestructure.Seconds);
  SSD1306_GotoXY(0, 0);
  SSD1306_Puts(time_, &Font_6x8, SSD1306_COLOR_WHITE, initial);

  // 使用DMA 单次转换  连续转换模式
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&AD_DMA_1, 1); //启用DMA的ADC转换，AD_DMA 0
  sprintf(AD_STR, "%.2f'", (float)(AD_DMA_1 *3.3/4096));
  SSD1306_GotoXY(54, 0);
  SSD1306_Puts(AD_STR, &Font_6x8, SSD1306_COLOR_WHITE, initial);  

  SSD1306_UpdateScreen();            // 更新显示
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
