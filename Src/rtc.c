/**
  ******************************************************************************
  * File Name          : RTC.c
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
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

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
  // HAL_RTCEx_SetSmoothCalib(&hrtc, NULL, NULL, 6);
  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) == 0x5050)//是否第一次配置
  {
    return;
  }
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x5050);
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date 
  */
  sTime.Hours = 12;
  sTime.Minutes = 0;
  sTime.Seconds = 0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_SATURDAY;
  DateToUpdate.Month = RTC_MONTH_JUNE;
  DateToUpdate.Date = 20;
  DateToUpdate.Year = 20;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();

    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_IRQn);
    HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
// 设置时分秒
HAL_StatusTypeDef setTime(uint8_t hours, uint8_t mins, uint8_t secs)
{
  RTC_TimeTypeDef stime;

  stime.Hours=hours;
  stime.Minutes=mins;
  stime.Seconds=secs;
  // 注意此处要用RTC_FORMAT_BIN十进制
  return HAL_RTC_SetTime(&hrtc, &stime, RTC_FORMAT_BIN);
}
// 设置年月日
HAL_StatusTypeDef setDate(uint8_t years, uint8_t mons, uint8_t dates, uint8_t weeds)
{
  RTC_DateTypeDef sdate;
  
  sdate.Year=years;
  sdate.Month=mons;
  sdate.Date=dates;
  sdate.WeekDay=weeds;
  // 注意此处要用RTC_FORMAT_BIN十进制
  return HAL_RTC_SetDate(&hrtc, &sdate, RTC_FORMAT_BIN);
}
//设置闹钟,记得写中断回调函数
HAL_StatusTypeDef setAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
  RTC_AlarmTypeDef sAlarm ;
  RTC_TimeTypeDef stime;
  /* 填写报警结构体变量 */
	HAL_RTC_GetTime(&hrtc, &stime, RTC_FORMAT_BIN);   //获取设置中断时的时间
	sAlarm.Alarm=RTC_ALARM_A;
	sAlarm.AlarmTime.Hours=hours+stime.Hours;
	sAlarm.AlarmTime.Minutes=minutes+stime.Minutes;
	sAlarm.AlarmTime.Seconds=seconds+stime.Seconds;
  // 注意此处要用RTC_FORMAT_BIN十进制
	return HAL_RTC_SetAlarm_IT(&hrtc,&sAlarm, RTC_FORMAT_BIN);
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
