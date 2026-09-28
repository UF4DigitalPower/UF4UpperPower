/* User CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32f4xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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
#include "main.h"
/* User CODE BEGIN Includes */

/* User CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* User CODE BEGIN TD */

/* User CODE END TD */

/* Private define ------------------------------------------------------------*/
/* User CODE BEGIN Define */

/* User CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* User CODE BEGIN Macro */

/* User CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* User CODE BEGIN PV */

/* User CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* User CODE BEGIN PFP */

/* User CODE END PFP */

/* External functions --------------------------------------------------------*/
/* User CODE BEGIN ExternalFunctions */

/* User CODE END ExternalFunctions */

/* User CODE BEGIN 0 */

/* User CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{

  /* User CODE BEGIN MspInit 0 */

  /* User CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);

  /* User CODE BEGIN MspInit 1 */

  /* User CODE END MspInit 1 */
}

/* User CODE BEGIN 1 */

/* User CODE END 1 */
