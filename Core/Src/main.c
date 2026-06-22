/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "mdma.h"
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
#include "panel_keys.h"
#include "setpoint_input.h"
#include "uf4_power_client.h"
#include "uf4com.h"
#include <string.h>

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
typedef enum
{
  APP_COMM_START_IDLE = 0,
  APP_COMM_START_WAIT_READ_RSP,
  APP_COMM_START_WAIT_STREAM_DELAY,
  APP_COMM_START_WAIT_STREAM_RSP,
  APP_COMM_START_STREAMING
} AppCommStartState_t;

static uint32_t g_last_key_tick = 0U;
static uint32_t g_gui_tick = 0U;

#define APP_GUI_UPDATE_PERIOD_MS 20U
#define APP_COMM_CONTROL_SYNC_PERIOD_MS 100U
#define APP_COMM_READ_TO_STREAM_DELAY_MS 80U
#define APP_COMM_START_TIMEOUT_MS 1000U
#define APP_SETPOINT_APPLY_DELAY_MS 500U

static uint32_t g_comm_control_sync_tick = 0U;
static uint32_t g_comm_start_tick = 0U;
static uint32_t g_comm_start_rx_frame_count = 0U;
static AppCommStartState_t g_comm_start_state = APP_COMM_START_IDLE;
static uint8_t g_remote_controls_seeded = 0U;
static float g_comm_last_vset = -1.0F;
static float g_comm_last_iset = -1.0F;
static float g_comm_last_ovp = -1.0F;
static float g_comm_last_ocp = -1.0F;
static float g_comm_last_otp = -1.0F;
static uint8_t g_comm_last_output_enabled = 0xFFU;
static float g_pending_vset = -1.0F;
static float g_pending_iset = -1.0F;
static uint32_t g_pending_vset_tick = 0U;
static uint32_t g_pending_iset_tick = 0U;

static void App_InitUf4ControlBaseline(void);

static uint16_t App_FloatToMilliU16(float value)
{
  uint32_t scaled;

  if (value <= 0.0F)
  {
    return 0U;
  }

  scaled = (uint32_t)(value * 1000.0F + 0.5F);
  return (scaled > 0xFFFFU) ? 0xFFFFU : (uint16_t)scaled;
}

static uint16_t App_FloatToCentiU16(float value)
{
  uint32_t scaled;

  if (value <= 0.0F)
  {
    return 0U;
  }

  scaled = (uint32_t)(value * 100.0F + 0.5F);
  return (scaled > 0xFFFFU) ? 0xFFFFU : (uint16_t)scaled;
}

static float App_ReadUf4Milli(uint8_t id)
{
  uint16_t raw;

  if (UF4PowerClient_GetU16(id, &raw))
  {
    return (float)raw / 1000.0F;
  }

  return 0.0F;
}

static float App_ReadUf4Centi(uint8_t id)
{
  uint16_t raw;

  if (UF4PowerClient_GetU16(id, &raw))
  {
    return (float)raw / 100.0F;
  }

  return 0.0F;
}

static float App_ReadUf4Permille(uint8_t id)
{
  uint16_t raw;

  if (UF4PowerClient_GetU16(id, &raw))
  {
    return (float)raw / 10.0F;
  }

  return 0.0F;
}

static uint8_t App_ReadUf4Bool(uint8_t id, uint8_t fallback)
{
  uint16_t raw;

  if (UF4PowerClient_GetU16(id, &raw))
  {
    return (uint8_t)(raw != 0U ? 1U : 0U);
  }

  return fallback;
}

static uint16_t App_ReadUf4U16(uint8_t id, uint16_t fallback)
{
  uint16_t raw;

  if (UF4PowerClient_GetU16(id, &raw))
  {
    return raw;
  }

  return fallback;
}

static uint8_t App_TryReadUf4U16(uint8_t id, uint16_t *value)
{
  return (uint8_t)(UF4PowerClient_GetU16(id, value) ? 1U : 0U);
}

static float App_ReadUf4MilliFallback(uint8_t id, float fallback)
{
  uint16_t raw;

  if (UF4PowerClient_GetU16(id, &raw))
  {
    return (float)raw / 1000.0F;
  }

  return fallback;
}

static float App_ReadUf4CentiFallback(uint8_t id, float fallback)
{
  uint16_t raw;

  if (UF4PowerClient_GetU16(id, &raw))
  {
    return (float)raw / 100.0F;
  }

  return fallback;
}

static uint8_t App_FloatChanged(float a, float b, float threshold)
{
  float diff = a - b;

  if (diff < 0.0F)
  {
    diff = -diff;
  }

  return (uint8_t)(diff >= threshold);
}

static void App_TrySeedControlsFromUf4(void)
{
  uint16_t raw_vset;
  uint16_t raw_iset;
  uint16_t raw_ovp;
  uint16_t raw_ocp;
  uint16_t raw_otp;
  uint16_t raw_power;

  if (g_remote_controls_seeded != 0U)
  {
    return;
  }

  if (!UF4PowerClient_GetU16(UF4_ID_SET_VOLTAGE_LIMIT, &raw_vset) ||
      !UF4PowerClient_GetU16(UF4_ID_SET_CURRENT_LIMIT, &raw_iset) ||
      !UF4PowerClient_GetU16(UF4_ID_OVP_SET_VALUE, &raw_ovp) ||
      !UF4PowerClient_GetU16(UF4_ID_OCP_SET_VALUE, &raw_ocp) ||
      !UF4PowerClient_GetU16(UF4_ID_OTP_SET_VALUE, &raw_otp) ||
      !UF4PowerClient_GetU16(UF4_ID_POWER_STATE, &raw_power))
  {
    return;
  }

  SetpointInput_SetVset((float)raw_vset / 1000.0F);
  SetpointInput_SetIset((float)raw_iset / 1000.0F);
  PanelKeys_SetProtectionState(
      (float)raw_ovp / 1000.0F,
      (float)raw_ocp / 1000.0F,
      (float)raw_otp / 100.0F,
      (uint8_t)(raw_ovp != 0U ? 1U : 0U),
      (uint8_t)(raw_ocp != 0U ? 1U : 0U),
      (uint8_t)(raw_otp != 0U ? 1U : 0U));
  PanelKeys_SetOutputEnabled((uint8_t)(raw_power != 0U ? 1U : 0U));
  App_InitUf4ControlBaseline();
  g_remote_controls_seeded = 1U;
}

static bool App_RequestInitialStatusRead(void)
{
  static const uint8_t ids[] =
  {
    UF4_ID_INPUT_VOLTAGE,
    UF4_ID_INPUT_CURRENT,
    UF4_ID_OUTPUT_VOLTAGE,
    UF4_ID_OUTPUT_CURRENT,
    UF4_ID_CORE_TEMPERATURE,
    UF4_ID_TEMP1_TEMPERATURE,
    UF4_ID_TEMP2_TEMPERATURE,
    UF4_ID_SET_VOLTAGE_LIMIT,
    UF4_ID_SET_CURRENT_LIMIT,
    UF4_ID_CC_CV_MODE,
    UF4_ID_POWER_STATE,
    UF4_ID_FAULT_STATE,
    UF4_ID_STATE_MACHINE_FLAG_BITS,
    UF4_ID_STATE_MACHINE_STATE,
    UF4_ID_OTP_VALUE,
    UF4_ID_OTP_SET_VALUE,
    UF4_ID_OVP_VALUE,
    UF4_ID_OVP_SET_VALUE,
    UF4_ID_OCP_VALUE,
    UF4_ID_OCP_SET_VALUE,
    UF4_ID_DUTY_CMD,
    UF4_ID_PWM_A_COMPARE,
    UF4_ID_PWM_D_COMPARE,
    UF4_ID_FAN_SPEED,
    UF4_ID_FAN_SET_VALUE
  };

  return UF4PowerClient_ReadU16Pairs(ids, (uint8_t)(sizeof(ids) / sizeof(ids[0])));
}

static void App_HandlePanelApplyEvent(void)
{
  switch (PanelKeys_TakeApplyEvent())
  {
    case PANEL_APPLY_COMM_START:
      if (g_comm_start_state == APP_COMM_START_STREAMING || UF4PowerClient_IsStreamEnabled())
      {
        g_comm_start_state = APP_COMM_START_STREAMING;
        break;
      }
      g_comm_start_rx_frame_count = UF4PowerClient_RxFrameCount();
      g_comm_start_tick = HAL_GetTick();
      if (App_RequestInitialStatusRead())
      {
        g_comm_start_state = APP_COMM_START_WAIT_READ_RSP;
      }
      else
      {
        g_comm_start_state = APP_COMM_START_IDLE;
      }
      break;

    default:
      break;
  }
}

static void App_RunCommStartTask(uint32_t now_tick)
{
  switch (g_comm_start_state)
  {
    case APP_COMM_START_WAIT_READ_RSP:
      if (UF4PowerClient_RxFrameCount() != g_comm_start_rx_frame_count ||
          (now_tick - g_comm_start_tick) >= APP_COMM_READ_TO_STREAM_DELAY_MS)
      {
        g_comm_start_tick = now_tick;
        g_comm_start_state = APP_COMM_START_WAIT_STREAM_DELAY;
      }
      break;

    case APP_COMM_START_WAIT_STREAM_DELAY:
      if ((now_tick - g_comm_start_tick) >= APP_COMM_READ_TO_STREAM_DELAY_MS)
      {
        g_comm_start_tick = now_tick;
        if (UF4PowerClient_StartStreamAll())
        {
          g_comm_start_state = APP_COMM_START_WAIT_STREAM_RSP;
        }
        else
        {
          g_comm_start_state = APP_COMM_START_WAIT_STREAM_DELAY;
        }
      }
      break;

    case APP_COMM_START_WAIT_STREAM_RSP:
      if (UF4PowerClient_IsStreamEnabled())
      {
        g_comm_start_state = APP_COMM_START_STREAMING;
      }
      else if ((now_tick - g_comm_start_tick) >= APP_COMM_START_TIMEOUT_MS)
      {
        g_comm_start_tick = now_tick;
        g_comm_start_state = APP_COMM_START_WAIT_STREAM_DELAY;
      }
      break;

    default:
      break;
  }
}

static void App_SyncUf4Controls(uint32_t now_tick)
{
  uint8_t ids[6];
  uint16_t values[6];
  uint8_t count = 0U;
  const float vset = SetpointInput_GetVset();
  const float iset = SetpointInput_GetIset();
  const float ovp = PanelKeys_GetOvp();
  const float ocp = PanelKeys_GetOcp();
  const float otp = PanelKeys_GetOtp();
  const uint8_t output_enabled = PanelKeys_GetOutputEnabled();

  if (g_comm_start_state == APP_COMM_START_WAIT_READ_RSP ||
      g_comm_start_state == APP_COMM_START_WAIT_STREAM_DELAY ||
      g_comm_start_state == APP_COMM_START_WAIT_STREAM_RSP)
  {
    return;
  }

  if (now_tick - g_comm_control_sync_tick < APP_COMM_CONTROL_SYNC_PERIOD_MS)
  {
    return;
  }
  g_comm_control_sync_tick = now_tick;

  if (App_FloatChanged(vset, g_pending_vset, 0.005F) != 0U)
  {
    g_pending_vset = vset;
    g_pending_vset_tick = now_tick;
  }
  if (App_FloatChanged(iset, g_pending_iset, 0.005F) != 0U)
  {
    g_pending_iset = iset;
    g_pending_iset_tick = now_tick;
  }

  if (App_FloatChanged(vset, g_comm_last_vset, 0.005F) != 0U &&
      (now_tick - g_pending_vset_tick) >= APP_SETPOINT_APPLY_DELAY_MS)
  {
    ids[count] = UF4_ID_SET_VOLTAGE_LIMIT;
    values[count] = App_FloatToMilliU16(vset);
    ++count;
    g_comm_last_vset = vset;
  }
  if (App_FloatChanged(iset, g_comm_last_iset, 0.005F) != 0U &&
      (now_tick - g_pending_iset_tick) >= APP_SETPOINT_APPLY_DELAY_MS)
  {
    ids[count] = UF4_ID_SET_CURRENT_LIMIT;
    values[count] = App_FloatToMilliU16(iset);
    ++count;
    g_comm_last_iset = iset;
  }
  if (App_FloatChanged(ovp, g_comm_last_ovp, 0.05F) != 0U)
  {
    ids[count] = UF4_ID_OVP_SET_VALUE;
    values[count] = App_FloatToMilliU16(ovp);
    ++count;
    g_comm_last_ovp = ovp;
  }
  if (App_FloatChanged(ocp, g_comm_last_ocp, 0.05F) != 0U)
  {
    ids[count] = UF4_ID_OCP_SET_VALUE;
    values[count] = App_FloatToMilliU16(ocp);
    ++count;
    g_comm_last_ocp = ocp;
  }
  if (App_FloatChanged(otp, g_comm_last_otp, 0.5F) != 0U)
  {
    ids[count] = UF4_ID_OTP_SET_VALUE;
    values[count] = App_FloatToCentiU16(otp);
    ++count;
    g_comm_last_otp = otp;
  }
  if (output_enabled != g_comm_last_output_enabled)
  {
    ids[count] = UF4_ID_POWER_STATE;
    values[count] = (uint16_t)(output_enabled != 0U ? 1U : 0U);
    ++count;
    g_comm_last_output_enabled = output_enabled;
  }

  if (count > 0U)
  {
    (void)UF4PowerClient_WriteU16Pairs(ids, values, count);
  }
}

static void App_FillGuiFromUf4(GUI_Data_t *gui_data)
{
  uint16_t raw_value;

  memset(gui_data, 0, sizeof(*gui_data));

  gui_data->vin = App_ReadUf4Milli(UF4_ID_INPUT_VOLTAGE);
  gui_data->iin = App_ReadUf4Milli(UF4_ID_INPUT_CURRENT);
  gui_data->pin = gui_data->vin * gui_data->iin;
  gui_data->fan = App_ReadUf4Permille(UF4_ID_FAN_SPEED);

  gui_data->vout = App_ReadUf4Milli(UF4_ID_OUTPUT_VOLTAGE);
  gui_data->iout = App_ReadUf4Milli(UF4_ID_OUTPUT_CURRENT);
  gui_data->pout = gui_data->vout * gui_data->iout;
  gui_data->power = gui_data->pout;

  if (gui_data->pin > 0.01F)
  {
    gui_data->efficiency = (gui_data->pout * 100.0F) / gui_data->pin;
  }
  else
  {
    gui_data->efficiency = 0.0F;
  }

  gui_data->vset = App_ReadUf4MilliFallback(UF4_ID_SET_VOLTAGE_LIMIT, SetpointInput_GetVset());
  gui_data->iset = App_ReadUf4MilliFallback(UF4_ID_SET_CURRENT_LIMIT, SetpointInput_GetIset());
  gui_data->vset_digit = SetpointInput_GetVsetDigit();
  gui_data->iset_digit = SetpointInput_GetIsetDigit();
  gui_data->ovp = App_ReadUf4MilliFallback(UF4_ID_OVP_SET_VALUE, PanelKeys_GetOvp());
  gui_data->ocp = App_ReadUf4MilliFallback(UF4_ID_OCP_SET_VALUE, PanelKeys_GetOcp());
  gui_data->otp = App_ReadUf4CentiFallback(UF4_ID_OTP_SET_VALUE, PanelKeys_GetOtp());
  gui_data->ovp_enabled = PanelKeys_GetOvpEnabled();
  gui_data->ocp_enabled = PanelKeys_GetOcpEnabled();
  gui_data->otp_enabled = PanelKeys_GetOtpEnabled();
  gui_data->output_enabled = App_ReadUf4Bool(UF4_ID_POWER_STATE, PanelKeys_GetOutputEnabled());
  gui_data->ble_state = (uint8_t)PanelKeys_GetBleState();
  gui_data->page = (uint8_t)PanelKeys_GetPage();
  gui_data->panel_field = (uint8_t)PanelKeys_GetSelectedField();
  gui_data->comm_state = (uint8_t)g_comm_start_state;
  gui_data->comm_stream_enabled = (uint8_t)(UF4PowerClient_IsStreamEnabled() ? 1U : 0U);
  gui_data->comm_last_tx_cmd = UF4PowerClient_LastTxCmd();
  gui_data->comm_last_tx_ok = (uint8_t)(UF4PowerClient_LastTxOk() ? 1U : 0U);
  gui_data->comm_tx_ok_count = UF4PowerClient_TxOkCount();
  gui_data->comm_tx_fail_count = UF4PowerClient_TxFailCount();
  gui_data->comm_rx_frame_count = UF4PowerClient_RxFrameCount();
  gui_data->comm_rx_error_count = UF4PowerClient_RxErrorCount();
  gui_data->regulation_mode = (uint8_t)App_ReadUf4U16(UF4_ID_CC_CV_MODE, 0U);
  if (App_TryReadUf4U16(UF4_ID_CC_CV_MODE, &raw_value) != 0U)
  {
    gui_data->valid_flags |= GUI_VALID_CC_CV_MODE;
    gui_data->regulation_mode = (uint8_t)raw_value;
  }
  if (App_TryReadUf4U16(UF4_ID_POWER_STATE, &raw_value) != 0U)
  {
    gui_data->valid_flags |= GUI_VALID_POWER_STATE;
    gui_data->output_enabled = (uint8_t)(raw_value != 0U ? 1U : 0U);
  }
  gui_data->fault_state = (uint8_t)App_ReadUf4U16(UF4_ID_FAULT_STATE, 0U);
  if (App_TryReadUf4U16(UF4_ID_FAULT_STATE, &raw_value) != 0U)
  {
    gui_data->valid_flags |= GUI_VALID_FAULT_STATE;
    gui_data->fault_state = (uint8_t)raw_value;
  }
  gui_data->state_machine_flags = App_ReadUf4U16(UF4_ID_STATE_MACHINE_FLAG_BITS, 0U);
  if (App_TryReadUf4U16(UF4_ID_STATE_MACHINE_FLAG_BITS, &raw_value) != 0U)
  {
    gui_data->valid_flags |= GUI_VALID_STATE_MACHINE_FLAGS;
    gui_data->state_machine_flags = raw_value;
  }
  gui_data->state_machine_state = (uint8_t)App_ReadUf4U16(UF4_ID_STATE_MACHINE_STATE, 0U);
  if (App_TryReadUf4U16(UF4_ID_STATE_MACHINE_STATE, &raw_value) != 0U)
  {
    gui_data->valid_flags |= GUI_VALID_STATE_MACHINE_STATE;
    gui_data->state_machine_state = (uint8_t)raw_value;
  }
  gui_data->duty_cmd = App_ReadUf4U16(UF4_ID_DUTY_CMD, 0U);
  gui_data->pwm_a_compare = App_ReadUf4U16(UF4_ID_PWM_A_COMPARE, 0U);
  if (App_TryReadUf4U16(UF4_ID_PWM_A_COMPARE, &raw_value) != 0U)
  {
    gui_data->valid_flags |= GUI_VALID_PWM_A_COMPARE;
    gui_data->pwm_a_compare = raw_value;
  }
  gui_data->pwm_d_compare = App_ReadUf4U16(UF4_ID_PWM_D_COMPARE, 0U);
  if (App_TryReadUf4U16(UF4_ID_PWM_D_COMPARE, &raw_value) != 0U)
  {
    gui_data->valid_flags |= GUI_VALID_PWM_D_COMPARE;
    gui_data->pwm_d_compare = raw_value;
  }
  gui_data->cpu_temp = App_ReadUf4Centi(UF4_ID_CORE_TEMPERATURE);
  gui_data->buck_temp = App_ReadUf4Centi(UF4_ID_TEMP1_TEMPERATURE);
  gui_data->boost_temp = App_ReadUf4Centi(UF4_ID_TEMP2_TEMPERATURE);
}

static void App_InitUf4ControlBaseline(void)
{
  uint32_t now_tick = HAL_GetTick();

  g_comm_last_vset = SetpointInput_GetVset();
  g_comm_last_iset = SetpointInput_GetIset();
  g_comm_last_ovp = PanelKeys_GetOvp();
  g_comm_last_ocp = PanelKeys_GetOcp();
  g_comm_last_otp = PanelKeys_GetOtp();
  g_comm_last_output_enabled = PanelKeys_GetOutputEnabled();
  g_pending_vset = g_comm_last_vset;
  g_pending_iset = g_comm_last_iset;
  g_pending_vset_tick = now_tick;
  g_pending_iset_tick = now_tick;
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
  MX_MDMA_Init();
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
  g_gui_tick = HAL_GetTick();
  g_comm_control_sync_tick = HAL_GetTick() - APP_COMM_CONTROL_SYNC_PERIOD_MS;

  SetpointInput_Init();
  PanelKeys_Init();
  App_InitUf4ControlBaseline();
  UF4PowerClient_Init();
  HAL_TIM_Base_Start_IT(&htim6);
  GUI_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    uint32_t loop_start_tick = HAL_GetTick();
    uint32_t now_tick;

    UF4PowerClient_Tick();
    App_TrySeedControlsFromUf4();
    App_HandlePanelApplyEvent();
    App_RunCommStartTask(loop_start_tick);
    App_SyncUf4Controls(loop_start_tick);

    now_tick = HAL_GetTick();
    if (now_tick - g_gui_tick >= APP_GUI_UPDATE_PERIOD_MS)
    {
      g_gui_tick += APP_GUI_UPDATE_PERIOD_MS;
      if (now_tick - g_gui_tick >= APP_GUI_UPDATE_PERIOD_MS)
      {
        g_gui_tick = now_tick;
      }

      App_FillGuiFromUf4(&gui_data);

      GUI_Update(
              &gui_data);
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
    PanelKeys_Update();
  }
}

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Framebuffers: DMA2D/LTDC visible, keep non-cacheable to avoid per-rect cache maintenance. */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
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

  /** SDRAM scratch/cache buffers: CPU-heavy font and temporary buffers remain cacheable. */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0xC0400000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
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
