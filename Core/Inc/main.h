/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LCD_BLK_Pin GPIO_PIN_3
#define LCD_BLK_GPIO_Port GPIOA
#define FLSAH_SCK_Pin GPIO_PIN_5
#define FLSAH_SCK_GPIO_Port GPIOA
#define FLASH_MISO_Pin GPIO_PIN_6
#define FLASH_MISO_GPIO_Port GPIOA
#define FLASH_MOSI_Pin GPIO_PIN_7
#define FLASH_MOSI_GPIO_Port GPIOA
#define FLASH_CS_Pin GPIO_PIN_4
#define FLASH_CS_GPIO_Port GPIOC
#define LCD_NSS_Pin GPIO_PIN_15
#define LCD_NSS_GPIO_Port GPIOA
#define LCD_SCK_Pin GPIO_PIN_10
#define LCD_SCK_GPIO_Port GPIOC
#define LCD_RST_Pin GPIO_PIN_11
#define LCD_RST_GPIO_Port GPIOC
#define LCD_SDA_Pin GPIO_PIN_12
#define LCD_SDA_GPIO_Port GPIOC
#define WIFI_HS_Pin GPIO_PIN_7
#define WIFI_HS_GPIO_Port GPIOD
#define WIFI_HS_EXTI_IRQn EXTI9_5_IRQn
#define WIFI_DR_Pin GPIO_PIN_9
#define WIFI_DR_GPIO_Port GPIOG
#define WIFI_DR_EXTI_IRQn EXTI9_5_IRQn
#define WIFI_NSS_Pin GPIO_PIN_10
#define WIFI_NSS_GPIO_Port GPIOG
#define WIFI_MISO_Pin GPIO_PIN_12
#define WIFI_MISO_GPIO_Port GPIOG
#define WIFI_SCK_Pin GPIO_PIN_13
#define WIFI_SCK_GPIO_Port GPIOG
#define WIFI_MOSI_Pin GPIO_PIN_14
#define WIFI_MOSI_GPIO_Port GPIOG
#define TOUCH_INT_Pin GPIO_PIN_4
#define TOUCH_INT_GPIO_Port GPIOB
#define TOUCH_INT_EXTI_IRQn EXTI4_IRQn
#define TPUCH_RST_Pin GPIO_PIN_5
#define TPUCH_RST_GPIO_Port GPIOB
#define TOUCH_SCL_Pin GPIO_PIN_6
#define TOUCH_SCL_GPIO_Port GPIOB
#define TOUCH_SDA_Pin GPIO_PIN_7
#define TOUCH_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
