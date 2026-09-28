/* User CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* User CODE BEGIN Includes */

#include "bsp_lcd.h"
#include "boot_animation.h"
#include "gui.h"
#include "g474_remote.h"
#include "parameter_manager.h"
#include "uf4com.h"
#include "uf4com_usrt.h"
#include "usart.h"
#include "w25qxx_port.h"

/* User CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* User CODE BEGIN PTD */

/* User CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* User CODE BEGIN PD */

/* User CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* User CODE BEGIN PM */

/* User CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* User CODE BEGIN Variables */

/* User CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 2048,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* User CODE BEGIN FunctionPrototypes */

/* User CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* User CODE BEGIN Init */

  /* User CODE END Init */

  /* User CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* User CODE END RTOS_MUTEX */

  /* User CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* User CODE END RTOS_SEMAPHORES */

  /* User CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* User CODE END RTOS_TIMERS */

  /* User CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* User CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* User CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* User CODE END RTOS_THREADS */

  /* User CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* User CODE END RTOS_EVENTS */

}

/* User CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* User CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* User CODE BEGIN StartDefaultTask */
  uint32_t last_gui_tick;

  LCD_Init();
  BootAnimation_Play();
  (void)W25Qxx_Init();
  UF4_ParameterManager_Init();
  UF4_Init(UF4_UART_Tx, &huart6);
  G474_Remote_Init();
  UF4_UART_Init();
  GUI_Init();
  last_gui_tick = HAL_GetTick();
  for(;;)
  {
    uint32_t now = HAL_GetTick();
    while ((uint32_t)(now - last_gui_tick) >= 10U)
    {
      GUI_Tick();
      last_gui_tick += 10U;
    }
    UF4_UART_Process();
    UF4_ProcessMs(now);
    G474_Remote_Process();
    UF4_ParameterManager_Process();
    GUI_Handler();
    osDelay(1);
  }
  /* User CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* User CODE BEGIN Application */

/* User CODE END Application */

