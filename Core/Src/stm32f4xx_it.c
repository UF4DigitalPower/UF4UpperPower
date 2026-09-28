/* User CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* User CODE BEGIN Includes */
/* User CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* User CODE BEGIN TD */

/* User CODE END TD */

/* Private define ------------------------------------------------------------*/
/* User CODE BEGIN PD */

/* User CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* User CODE BEGIN PM */

/* User CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* User CODE BEGIN PV */

/* User CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* User CODE BEGIN PFP */

/* User CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* User CODE BEGIN 0 */

/* User CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA2D_HandleTypeDef hdma2d;
extern LTDC_HandleTypeDef hltdc;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim10;
extern TIM_HandleTypeDef htim11;
extern TIM_HandleTypeDef htim13;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim1;
extern UART_HandleTypeDef huart6;

/* User CODE BEGIN EV */

/* User CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* User CODE BEGIN NonMaskableInt_IRQn 0 */

  /* User CODE END NonMaskableInt_IRQn 0 */
  /* User CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* User CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* User CODE BEGIN HardFault_IRQn 0 */

  /* User CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* User CODE BEGIN W1_HardFault_IRQn 0 */
    /* User CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* User CODE BEGIN MemoryManagement_IRQn 0 */

  /* User CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* User CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* User CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* User CODE BEGIN BusFault_IRQn 0 */

  /* User CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* User CODE BEGIN W1_BusFault_IRQn 0 */
    /* User CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* User CODE BEGIN UsageFault_IRQn 0 */

  /* User CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* User CODE BEGIN W1_UsageFault_IRQn 0 */
    /* User CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* User CODE BEGIN DebugMonitor_IRQn 0 */

  /* User CODE END DebugMonitor_IRQn 0 */
  /* User CODE BEGIN DebugMonitor_IRQn 1 */

  /* User CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line4 interrupt.
  */
void EXTI4_IRQHandler(void)
{
  /* User CODE BEGIN EXTI4_IRQn 0 */

  /* User CODE END EXTI4_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(TOUCH_INT_Pin);
  /* User CODE BEGIN EXTI4_IRQn 1 */

  /* User CODE END EXTI4_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* User CODE BEGIN EXTI9_5_IRQn 0 */

  /* User CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(WIFI_HS_Pin);
  HAL_GPIO_EXTI_IRQHandler(WIFI_DR_Pin);
  /* User CODE BEGIN EXTI9_5_IRQn 1 */

  /* User CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt and TIM10 global interrupt.
  */
void TIM1_UP_TIM10_IRQHandler(void)
{
  /* User CODE BEGIN TIM1_UP_TIM10_IRQn 0 */

  /* User CODE END TIM1_UP_TIM10_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  HAL_TIM_IRQHandler(&htim10);
  /* User CODE BEGIN TIM1_UP_TIM10_IRQn 1 */

  /* User CODE END TIM1_UP_TIM10_IRQn 1 */
}

/**
  * @brief This function handles TIM1 trigger and commutation interrupts and TIM11 global interrupt.
  */
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
  /* User CODE BEGIN TIM1_TRG_COM_TIM11_IRQn 0 */

  /* User CODE END TIM1_TRG_COM_TIM11_IRQn 0 */
  HAL_TIM_IRQHandler(&htim11);
  /* User CODE BEGIN TIM1_TRG_COM_TIM11_IRQn 1 */

  /* User CODE END TIM1_TRG_COM_TIM11_IRQn 1 */
}

/**
  * @brief This function handles TIM8 update interrupt and TIM13 global interrupt.
  */
void TIM8_UP_TIM13_IRQHandler(void)
{
  /* User CODE BEGIN TIM8_UP_TIM13_IRQn 0 */

  /* User CODE END TIM8_UP_TIM13_IRQn 0 */
  HAL_TIM_IRQHandler(&htim13);
  /* User CODE BEGIN TIM8_UP_TIM13_IRQn 1 */

  /* User CODE END TIM8_UP_TIM13_IRQn 1 */
}

/**
  * @brief This function handles TIM8 trigger and commutation interrupts and TIM14 global interrupt.
  */
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
  /* User CODE BEGIN TIM8_TRG_COM_TIM14_IRQn 0 */

  /* User CODE END TIM8_TRG_COM_TIM14_IRQn 0 */
  HAL_TIM_IRQHandler(&htim14);
  /* User CODE BEGIN TIM8_TRG_COM_TIM14_IRQn 1 */

  /* User CODE END TIM8_TRG_COM_TIM14_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt, DAC1 and DAC2 underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  /* User CODE BEGIN TIM6_DAC_IRQn 0 */

  /* User CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* User CODE BEGIN TIM6_DAC_IRQn 1 */

  /* User CODE END TIM6_DAC_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* User CODE BEGIN TIM7_IRQn 0 */

  /* User CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* User CODE BEGIN TIM7_IRQn 1 */

  /* User CODE END TIM7_IRQn 1 */
}

/**
  * @brief This function handles LTDC global interrupt.
  */
void LTDC_IRQHandler(void)
{
  /* User CODE BEGIN LTDC_IRQn 0 */

  /* User CODE END LTDC_IRQn 0 */
  HAL_LTDC_IRQHandler(&hltdc);
  /* User CODE BEGIN LTDC_IRQn 1 */

  /* User CODE END LTDC_IRQn 1 */
}

/**
  * @brief This function handles LTDC global error interrupt.
  */
void LTDC_ER_IRQHandler(void)
{
  /* User CODE BEGIN LTDC_ER_IRQn 0 */

  /* User CODE END LTDC_ER_IRQn 0 */
  HAL_LTDC_IRQHandler(&hltdc);
  /* User CODE BEGIN LTDC_ER_IRQn 1 */

  /* User CODE END LTDC_ER_IRQn 1 */
}

/**
  * @brief This function handles DMA2D global interrupt.
  */
void DMA2D_IRQHandler(void)
{
  /* User CODE BEGIN DMA2D_IRQn 0 */

  /* User CODE END DMA2D_IRQn 0 */
  HAL_DMA2D_IRQHandler(&hdma2d);
  /* User CODE BEGIN DMA2D_IRQn 1 */

  /* User CODE END DMA2D_IRQn 1 */
}

/* User CODE BEGIN 1 */

void USART6_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart6);
}

/* User CODE END 1 */
