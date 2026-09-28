/* User CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    crc.c
  * @brief   This file provides code for the configuration
  *          of the CRC instances.
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
/* Includes ------------------------------------------------------------------*/
#include "crc.h"

/* User CODE BEGIN 0 */

/* User CODE END 0 */

CRC_HandleTypeDef hcrc;

/* CRC init function */
void MX_CRC_Init(void)
{

  /* User CODE BEGIN CRC_Init 0 */

  /* User CODE END CRC_Init 0 */

  /* User CODE BEGIN CRC_Init 1 */

  /* User CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* User CODE BEGIN CRC_Init 2 */

  /* User CODE END CRC_Init 2 */

}

void HAL_CRC_MspInit(CRC_HandleTypeDef* crcHandle)
{

  if(crcHandle->Instance==CRC)
  {
  /* User CODE BEGIN CRC_MspInit 0 */

  /* User CODE END CRC_MspInit 0 */
    /* CRC clock enable */
    __HAL_RCC_CRC_CLK_ENABLE();
  /* User CODE BEGIN CRC_MspInit 1 */

  /* User CODE END CRC_MspInit 1 */
  }
}

void HAL_CRC_MspDeInit(CRC_HandleTypeDef* crcHandle)
{

  if(crcHandle->Instance==CRC)
  {
  /* User CODE BEGIN CRC_MspDeInit 0 */

  /* User CODE END CRC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CRC_CLK_DISABLE();
  /* User CODE BEGIN CRC_MspDeInit 1 */

  /* User CODE END CRC_MspDeInit 1 */
  }
}

/* User CODE BEGIN 1 */

/* User CODE END 1 */
