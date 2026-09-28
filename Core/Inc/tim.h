/* User CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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
/* User CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* User CODE BEGIN Includes */

/* User CODE END Includes */

extern TIM_HandleTypeDef htim2;

extern TIM_HandleTypeDef htim6;

extern TIM_HandleTypeDef htim7;

extern TIM_HandleTypeDef htim10;

extern TIM_HandleTypeDef htim11;

extern TIM_HandleTypeDef htim13;

extern TIM_HandleTypeDef htim14;

/* User CODE BEGIN Private defines */

/* User CODE END Private defines */

void MX_TIM2_Init(void);
void MX_TIM6_Init(void);
void MX_TIM7_Init(void);
void MX_TIM10_Init(void);
void MX_TIM11_Init(void);
void MX_TIM13_Init(void);
void MX_TIM14_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* User CODE BEGIN Prototypes */

/* User CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

