/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f4xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BTN_MOVE_Pin GPIO_PIN_2
#define BTN_MOVE_GPIO_Port GPIOE
#define BTN_SELECT_Pin GPIO_PIN_3
#define BTN_SELECT_GPIO_Port GPIOE
#define WIFI_TX_Pin GPIO_PIN_2
#define WIFI_TX_GPIO_Port GPIOA
#define WIFI_RX_Pin GPIO_PIN_3
#define WIFI_RX_GPIO_Port GPIOA
#define GPS_TX_Pin GPIO_PIN_9
#define GPS_TX_GPIO_Port GPIOA
#define GPS_RX_Pin GPIO_PIN_10
#define GPS_RX_GPIO_Port GPIOA
#define LCD_DC_Pin GPIO_PIN_5
#define LCD_DC_GPIO_Port GPIOB
#define LCD_RST_Pin GPIO_PIN_6
#define LCD_RST_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_7
#define LCD_CS_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

//for LCD
#define DC_Pin LCD_DC_Pin
#define DC_GPIO_Port LCD_DC_GPIO_Port
#define RST_Pin LCD_RST_Pin
#define RST_GPIO_Port LCD_RST_GPIO_Port
#define CS_Pin LCD_CS_Pin
#define CS_GPIO_Port LCD_CS_GPIO_Port

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
