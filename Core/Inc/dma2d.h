/* User CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dma2d.h
  * @brief   This file contains all the function prototypes for
  *          the dma2d.c file
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
#ifndef __DMA2D_H__
#define __DMA2D_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* User CODE BEGIN Includes */

/* User CODE END Includes */

extern DMA2D_HandleTypeDef hdma2d;

/* User CODE BEGIN Private defines */

/* User CODE END Private defines */

void MX_DMA2D_Init(void);

/* User CODE BEGIN Prototypes */

/* User CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __DMA2D_H__ */

