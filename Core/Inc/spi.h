/* User CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
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
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* User CODE BEGIN Includes */

/* User CODE END Includes */

extern SPI_HandleTypeDef hspi1;

extern SPI_HandleTypeDef hspi3;

extern SPI_HandleTypeDef hspi6;

/* User CODE BEGIN Private defines */

/* User CODE END Private defines */

void MX_SPI1_Init(void);
void MX_SPI3_Init(void);
void MX_SPI6_Init(void);

/* User CODE BEGIN Prototypes */

/* User CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

