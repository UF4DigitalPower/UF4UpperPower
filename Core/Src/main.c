/**
  ******************************************************************************
  * @file    main.c
  * @author  UF4
  * @date    26-6-20 下午6:22
  * @brief   STM32H743 application entry.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 UF4.
  * All rights reserved.
  *
  * This software is provided "as is", without warranty of any kind.
  *
  ******************************************************************************
  */
/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "memorymap.h"
#include "quadspi.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_lcd.h"
#include "bsp_st7701.h"
#include "gui.h"
#include "setpoint_input.h"

/* USER CODE END Includes */
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static uint32_t g_last_key_tick = 0U;
static uint16_t g_perf_fps = 0U;
static uint8_t g_perf_cpu_usage = 0U;
static uint32_t g_perf_window_tick = 0U;
static uint32_t g_perf_frame_count = 0U;
static uint32_t g_perf_busy_ticks = 0U;
static uint32_t g_gui_tick = 0U;
static uint32_t g_sim_tick = 0U;
static uint32_t g_sim_rand = 0x12345678UL;
static float g_sim_vin = 48.0F;
static float g_sim_iin = 6.10F;
static float g_sim_efficiency = 91.0F;
static float g_sim_fan = 38.0F;
static float g_sim_vout = 21.33F;
static float g_sim_iout = 12.21F;
static float g_sim_cpu_temp = 45.0F;
static float g_sim_buck_temp = 52.0F;
static float g_sim_boost_temp = 49.0F;

#define APP_GUI_UPDATE_PERIOD_MS 33U
#define APP_TELEMETRY_UPDATE_PERIOD_MS 200U

static uint32_t App_NextRandom(void)
{
  g_sim_rand = g_sim_rand * 1664525UL + 1013904223UL;
  return g_sim_rand;
}

static float App_RandomRange(float min_value, float max_value)
{
  uint32_t sample = (App_NextRandom() >> 8) & 0xFFFFUL;
  float ratio = (float)sample / 65535.0F;
  return min_value + ((max_value - min_value) * ratio);
}

static void App_UpdateSimulatedTelemetry(void)
{
  uint32_t now = HAL_GetTick();

  if (now - g_sim_tick < APP_TELEMETRY_UPDATE_PERIOD_MS)
  {
    return;
  }

  g_sim_tick = now;
  g_sim_vin = App_RandomRange(38.40F, 47.60F);
  g_sim_iin = App_RandomRange(5.70F, 16.50F);
  g_sim_efficiency = App_RandomRange(79.5F, 93.5F);
  g_sim_fan = App_RandomRange(28.0F, 62.0F);
  g_sim_vout = App_RandomRange(10.90F, 21.70F);
  g_sim_iout = App_RandomRange(1.60F, 12.80F);
  g_sim_cpu_temp = App_RandomRange(42.0F, 51.0F);
  g_sim_buck_temp = App_RandomRange(48.0F, 60.0F);
  g_sim_boost_temp = App_RandomRange(46.0F, 58.0F);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_I2C3_Init();
  MX_LTDC_Init();
  MX_QUADSPI_Init();
  MX_SPI6_Init();
  MX_TIM6_Init();
  MX_UART5_Init();
  MX_UART4_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();
  ST7701Init();
  GUI_Data_t gui_data;
  g_perf_window_tick = HAL_GetTick();
  g_gui_tick = HAL_GetTick();
  g_sim_tick = HAL_GetTick();

  SetpointInput_Init();
  HAL_TIM_Base_Start_IT(&htim6);
  GUI_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    uint32_t loop_start_tick = HAL_GetTick();
    uint32_t now_tick;

    App_UpdateSimulatedTelemetry();

    now_tick = HAL_GetTick();
    if (now_tick - g_gui_tick >= APP_GUI_UPDATE_PERIOD_MS)
    {
      g_gui_tick += APP_GUI_UPDATE_PERIOD_MS;
      if (now_tick - g_gui_tick >= APP_GUI_UPDATE_PERIOD_MS)
      {
        g_gui_tick = now_tick;
      }

      gui_data.vin = g_sim_vin;
      gui_data.iin = g_sim_iin;
      gui_data.pin = gui_data.vin * gui_data.iin;
      gui_data.efficiency = g_sim_efficiency;
      gui_data.fan = g_sim_fan;

      gui_data.vout = g_sim_vout;
      gui_data.iout = g_sim_iout;
      gui_data.pout = gui_data.vout * gui_data.iout;
      gui_data.power = gui_data.pout;

      gui_data.vset = SetpointInput_GetVset();
      gui_data.iset = SetpointInput_GetIset();
      gui_data.vset_digit = SetpointInput_GetVsetDigit();
      gui_data.iset_digit = SetpointInput_GetIsetDigit();
      gui_data.cpu_temp = g_sim_cpu_temp;
      gui_data.buck_temp = g_sim_buck_temp;
      gui_data.boost_temp = g_sim_boost_temp;
      gui_data.fps = g_perf_fps;
      gui_data.cpu_usage = g_perf_cpu_usage;

      GUI_Update(
              &gui_data);

      ++g_perf_frame_count;
    }

    g_perf_busy_ticks += HAL_GetTick() - loop_start_tick;

    if (HAL_GetTick() - g_perf_window_tick >= 1000U)
    {
      uint32_t window_ticks = HAL_GetTick() - g_perf_window_tick;
      g_perf_fps = (uint16_t)((g_perf_frame_count * 1000U + (window_ticks / 2U)) / window_ticks);
      g_perf_cpu_usage = (uint8_t)((g_perf_busy_ticks * 100U + (window_ticks / 2U)) / window_ticks);
      if (g_perf_cpu_usage > 100U)
      {
        g_perf_cpu_usage = 100U;
      }
      g_perf_frame_count = 0U;
      g_perf_busy_ticks = 0U;
      g_perf_window_tick = HAL_GetTick();
    }

    HAL_Delay(1);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 15;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6)
  {
    SetpointInput_Update();
  }
}

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0xC0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0xC0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4MB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {

  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
