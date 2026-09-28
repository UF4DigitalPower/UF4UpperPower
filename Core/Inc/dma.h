/* User CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dma.h
  * @brief   This file contains all the function prototypes for
  *          the dma.c file
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
#ifndef __DMA_H__
#define __DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* DMA memory to memory transfer handles -------------------------------------*/
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream0;

/* User CODE BEGIN Includes */

/* User CODE END Includes */

/* User CODE BEGIN Private defines */

/* User CODE END Private defines */

void MX_DMA_Init(void);

/* User CODE BEGIN Prototypes */

/* User CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __DMA_H__ */

