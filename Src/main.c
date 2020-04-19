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
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "delay.h"
#include "dht11.h"
#include "nrf24l01.h"
#include <stdlib.h>
#include "oled12864.h"
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
uint8_t USART_TxBuffer[]="ok";
uint8_t DHT22Status = 0;  // DHT22״̬
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  float temp = 0, humi = 0;  // ��ʪ�ȱ���
	uint32_t AD_DMA[3];   // ����adcת����ֵ ����ͨ������
  uint8_t NRF24L01_Buffer[32];  // ����ģ��һ�����շ��������
  char t[4];
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
  MX_SPI1_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_DMA(&huart1,USART_RxBuffer,1);  // �򿪴���1 DMA�Ľ���ʹ��
	printf("��ϵͳ��Ү�ջ�����ͽ�����\r\n");
	printf("2020��4��5�տ�ʼ���\r\n");
	printf("����1��ʼ�����\r\n");
	Delay_ms(3000);

  SSD1306_Init();  // OLED12864 ��ʼ��
  // HAL_UART_Transmit_DMA(&huart1,USART_TxBuffer,sizeof(USART_TxBuffer));  // DMA��������
	DHT22Status = DHT11_Init();  // ��ʼ��DHT22��ʪ�ȴ�����
	HAL_ADCEx_Calibration_Start(&hadc1);  // ����ADCУ׼
	HAL_SPI_Init(&hspi1);  // ʹ��SPI
  NRF24L01_Init();    //��ʼ��NRF24L01
	
	
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
  SSD1306_GotoXY(0, 0);
  OLED_ShowText("��str ��123", SSD1306_COLOR_WHITE, initial);
  SSD1306_GotoXY(0, 16);
  OLED_ShowText("��str ��123", SSD1306_COLOR_WHITE, bold);
	SSD1306_UpdateScreen();            // ������ʾ
	// OLED_Scroll_Display(0, 7, LEFT);

  // OLED_Show_16x16HZ(0, 0, "��");

  // OLED_ShowChar(0,0,'2',&Font_6x7);
	
  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  return 1;
  while (1)
  {
		// Delay_ms(100);
		// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		// Delay_ms(100);
		// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		
		// ת������ͨ��  ����DMA ɨ��ģʽ  ����ת��  ���ģʽ 1
		// for(char i=0; i<3; i++)
		// {
    //   HAL_ADC_Start(&hadc1);  // ����ADC1ת��
    //   HAL_ADC_PollForConversion(&hadc1, 50);  // �ȴ�ADC1ת�����
    //   // �ж�ADC1ת����ɱ�־λ�Ƿ����á�
    //   if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
    //   {
    //     AD_DMA[i] = HAL_ADC_GetValue(&hadc1);  // ��ȡADCֵ
    //     printf("%.2f \r\n \r\n", (float)(AD_DMA[i] *3.3/4096));
    //   }
		// }
		// HAL_ADC_Stop(&hadc1);  // ֹͣת��

    // ʹ��DMA ɨ��ģʽ  ����ת��ģʽ
    // HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&AD_DMA, 3); //����DMA��ADCת����AD_DMA 0~2
		// printf("AD_DMA_0 = %.2f\r\n",(float)(AD_DMA[0] *3.3/4096));
		// printf("AD_DMA_1 = %.2f\r\n",(float)(AD_DMA[1] *3.3/4096));
		// printf("AD_DMA_2 = %.2f\r\n\r\n",(float)(AD_DMA[2] *3.3/4096));

		if(!DHT22Status)
		{
			DHT11_Read_Data_Float(&temp, &humi);
			Delay_ms(1000);
			printf("��ǰ�¶ȣ�%f \r\n��ǰʪ�ȣ�%f \r\n\r\n", temp, humi);
			char t1 = (char)temp;
			char t2 = (char)((temp - t1) * 100.0);
			char h1 = (char)humi;
			char h2 = (char)((humi - h1) * 100.0);
			NRF24L01_Buffer[0] = t1;  // �¶ȵ�����
			NRF24L01_Buffer[1] = t2;  // �¶ȵ�С��
			NRF24L01_Buffer[2] = h1;	// ʪ�ȵ�����
			NRF24L01_Buffer[3] = h2;	// ʪ�ȵ�С��
			if(NRF24L01_TxPacket(NRF24L01_Buffer)==TX_OK)
			{
				printf("������� \r\n");
			}
      sprintf(t, "%.1f", temp);
			SSD1306_GotoXY(0, 24);
      SSD1306_Puts(t, &Font_8x16, SSD1306_COLOR_WHITE, initial);
	    SSD1306_UpdateScreen();            // ������ʾ
		}
		Delay_ms(3000);

    // if(NRF24L01_RxPacket(NRF24L01_Buffer)==0)
    // {
    //   float RX_temp = NRF24L01_Buffer[0] + NRF24L01_Buffer[1] / 100.0;
    //   float RX_humi = NRF24L01_Buffer[2] + NRF24L01_Buffer[3] / 100.0;
    //   printf("�յ����ݣ�\r\n��ǰ�¶ȣ�%f \r\n��ǰʪ�ȣ�%f \r\n\r\n", RX_temp, RX_humi);
    // }

    // while(NRF24L01_Check())  // ��ⲻ��24L01
    // {
		// 	Delay_ms(2000);
    // }
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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// ������һ�����ڽ��պ����Ļص��������ѽ��յ��������ٷ���ȥ��
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	HAL_UART_Transmit(&huart1,USART_RxBuffer,1,0);  // ���ڷ�������
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
