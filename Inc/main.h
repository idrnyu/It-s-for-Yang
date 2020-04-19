/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DHT22_Pin GPIO_PIN_13
#define DHT22_GPIO_Port GPIOC
#define switch_out_Pin GPIO_PIN_15
#define switch_out_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOD
#define LED2_Pin GPIO_PIN_1
#define LED2_GPIO_Port GPIOD
#define RF24L01_CE_Pin GPIO_PIN_3
#define RF24L01_CE_GPIO_Port GPIOA
#define RF24L01_CNS_Pin GPIO_PIN_4
#define RF24L01_CNS_GPIO_Port GPIOA
#define NRF24L01_IRQ_Pin GPIO_PIN_0
#define NRF24L01_IRQ_GPIO_Port GPIOB
#define model_Pin GPIO_PIN_11
#define model_GPIO_Port GPIOB
#define KEY_OFF_Pin GPIO_PIN_11
#define KEY_OFF_GPIO_Port GPIOA
#define KEY_ON_Pin GPIO_PIN_12
#define KEY_ON_GPIO_Port GPIOA
#define encoder_Z_Pin GPIO_PIN_4
#define encoder_Z_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
