/**
  ******************************************************************************
  * @file    app_controller.c
  * @brief   Application coordination for UF4 power UI and telemetry.
  ******************************************************************************
  */
#include "app_controller.h"

#include "bsp_lcd.h"
#include "bsp_st7701.h"
#include "gui.h"
#include "panel_keys.h"
#include "setpoint_input.h"
#include "uf4_mem.h"
#include "uf4_power_client.h"
#include "uf4com.h"

#include "stm32h7xx_hal.h"

#include <stdbool.h>
#include <string.h>

#define APP_GUI_UPDATE_PERIOD_MS            20U
#define APP_SCOPE_GUI_UPDATE_PERIOD_MS      120U
#define APP_COMM_CONTROL_SYNC_PERIOD_MS     100U
#define APP_COMM_READ_TO_STREAM_DELAY_MS    80U
#define APP_COMM_START_TIMEOUT_MS           1000U
#define APP_SETPOINT_APPLY_DELAY_MS         500U
#define APP_FLOAT_EFFICIENCY_MIN_INPUT_W    0.01F

typedef enum
{
  APP_COMM_START_IDLE = 0,
  APP_COMM_START_WAIT_READ_RSP,
  APP_COMM_START_WAIT_STREAM_DELAY,
  APP_COMM_START_WAIT_STREAM_RSP,
  APP_COMM_START_STREAMING
} AppCommStartState_t;

static uint32_t g_gui_tick;
static uint32_t g_comm_control_sync_tick;
static uint32_t g_comm_start_tick;
static uint32_t g_comm_start_rx_frame_count;
static AppCommStartState_t g_comm_start_state = APP_COMM_START_IDLE;
static uint8_t g_remote_controls_seeded;
static float g_comm_last_vset = -1.0F;
static float g_comm_last_iset = -1.0F;
static float g_comm_last_ovp = -1.0F;
static float g_comm_last_ocp = -1.0F;
static float g_comm_last_otp = -1.0F;
static uint8_t g_comm_last_output_enabled = 0xFFU;
static float g_pending_vset = -1.0F;
static float g_pending_iset = -1.0F;
static uint32_t g_pending_vset_tick;
static uint32_t g_pending_iset_tick;
static GUI_Data_t g_gui_data;

static void AppController_InitDisplay(void);
static void AppController_InitServices(void);
static void AppController_RunCommunication(uint32_t now_tick);
static void AppController_RunGui(uint32_t now_tick);
static void AppController_HandlePanelApplyEvent(void);
static void AppController_RunCommStartTask(uint32_t now_tick);
static void AppController_SyncUf4Controls(uint32_t now_tick);
static void AppController_TrySeedControlsFromUf4(void);
static bool AppController_RequestInitialStatusRead(void);
static void AppController_FillGuiFromUf4(GUI_Data_t *gui_data);
static void AppController_InitUf4ControlBaseline(void);

/**
  * @brief Convert a positive float to a saturated uint16 value in mill units.
  */
UF4_FORCE_INLINE uint16_t AppController_FloatToMilliU16(float value)
{
  uint32_t scaled;

  if (value <= 0.0F)
  {
    return 0U;
  }

  scaled = (uint32_t)(value * 1000.0F + 0.5F);
  return (scaled > 0xFFFFU) ? 0xFFFFU : (uint16_t)scaled;
}

/**
  * @brief Convert a positive float to a saturated uint16 value in centi units.
  */
UF4_FORCE_INLINE uint16_t AppController_FloatToCentiU16(float value)
{
  uint32_t scaled;

  if (value <= 0.0F)
  {
    return 0U;
  }

  scaled = (uint32_t)(value * 100.0F + 0.5F);
  return (scaled > 0xFFFFU) ? 0xFFFFU : (uint16_t)scaled;
}

/**
  * @brief Read an UF4 register and scale it by a fixed divisor.
  */
UF4_FORCE_INLINE float AppController_ReadUf4Scaled(uint8_t id, float divisor, float fallback)
{
  uint16_t raw;

  if (UF4PowerClient_GetU16(id, &raw))
  {
    return (float)raw / divisor;
  }

  return fallback;
}

/**
  * @brief Read an UF4 boolean register with fallback.
  */
UF4_FORCE_INLINE uint8_t AppController_ReadUf4Bool(uint8_t id, uint8_t fallback)
{
  uint16_t raw;

  if (UF4PowerClient_GetU16(id, &raw))
  {
    return (uint8_t)(raw != 0U ? 1U : 0U);
  }

  return fallback;
}

/**
  * @brief Read an UF4 U16 register with fallback.
  */
UF4_FORCE_INLINE uint16_t AppController_ReadUf4U16(uint8_t id, uint16_t fallback)
{
  uint16_t raw;

  if (UF4PowerClient_GetU16(id, &raw))
  {
    return raw;
  }

  return fallback;
}

/**
  * @brief Return 1 when two floats differ by at least threshold.
  */
UF4_FORCE_INLINE uint8_t AppController_FloatChanged(float a, float b, float threshold)
{
  float diff = a - b;

  if (diff < 0.0F)
  {
    diff = -diff;
  }

  return (uint8_t)(diff >= threshold);
}

void AppController_Init(void)
{
  AppController_InitDisplay();
  AppController_InitServices();
}

void AppController_Run(void)
{
  const uint32_t now_tick = HAL_GetTick();

  AppController_RunCommunication(now_tick);
  AppController_RunGui(HAL_GetTick());
  HAL_Delay(1);
}

void AppController_TimerTick(void)
{
  SetpointInput_Update();
  PanelKeys_Update();
}

/**
  * @brief Bring up LCD controller and initial GUI surface.
  */
static void AppController_InitDisplay(void)
{
  LCD_Init();
  ST7701Init();
}

/**
  * @brief Initialize input, telemetry and control baselines.
  */
static void AppController_InitServices(void)
{
  g_gui_tick = HAL_GetTick();
  g_comm_control_sync_tick = HAL_GetTick() - APP_COMM_CONTROL_SYNC_PERIOD_MS;

  SetpointInput_Init();
  PanelKeys_Init();
  AppController_InitUf4ControlBaseline();
  UF4PowerClient_Init();
  GUI_Init();
}

/**
  * @brief Run all UF4 foreground communication tasks once.
  */
static void AppController_RunCommunication(uint32_t now_tick)
{
  UF4PowerClient_Tick();
  AppController_TrySeedControlsFromUf4();
  AppController_HandlePanelApplyEvent();
  AppController_RunCommStartTask(now_tick);
  AppController_SyncUf4Controls(now_tick);
}

/**
  * @brief Refresh GUI at a fixed period and absorb occasional loop jitter.
  */
static void AppController_RunGui(uint32_t now_tick)
{
  const uint32_t update_period = (PanelKeys_GetPage() == PANEL_PAGE_SCOPE)
      ? APP_SCOPE_GUI_UPDATE_PERIOD_MS
      : APP_GUI_UPDATE_PERIOD_MS;

  if (now_tick - g_gui_tick < update_period)
  {
    return;
  }

  g_gui_tick += update_period;
  if (now_tick - g_gui_tick >= update_period)
  {
    g_gui_tick = now_tick;
  }

  AppController_FillGuiFromUf4(&g_gui_data);
  GUI_Update(&g_gui_data);
}

/**
  * @brief Start the initial UF4 read/stream handshake when the panel requests it.
  */
static void AppController_HandlePanelApplyEvent(void)
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
      g_comm_start_state = AppController_RequestInitialStatusRead()
          ? APP_COMM_START_WAIT_READ_RSP
          : APP_COMM_START_IDLE;
      break;

    default:
      break;
  }
}

/**
  * @brief Advance the read-before-stream startup state machine.
  */
static void AppController_RunCommStartTask(uint32_t now_tick)
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
        g_comm_start_state = UF4PowerClient_StartStreamAll()
            ? APP_COMM_START_WAIT_STREAM_RSP
            : APP_COMM_START_WAIT_STREAM_DELAY;
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

/**
  * @brief Push changed panel control values to the remote UF4 power board.
  */
static void AppController_SyncUf4Controls(uint32_t now_tick)
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
      g_comm_start_state == APP_COMM_START_WAIT_STREAM_RSP ||
      now_tick - g_comm_control_sync_tick < APP_COMM_CONTROL_SYNC_PERIOD_MS)
  {
    return;
  }
  g_comm_control_sync_tick = now_tick;

  if (AppController_FloatChanged(vset, g_pending_vset, 0.005F) != 0U)
  {
    g_pending_vset = vset;
    g_pending_vset_tick = now_tick;
  }
  if (AppController_FloatChanged(iset, g_pending_iset, 0.005F) != 0U)
  {
    g_pending_iset = iset;
    g_pending_iset_tick = now_tick;
  }

  if (AppController_FloatChanged(vset, g_comm_last_vset, 0.005F) != 0U &&
      (now_tick - g_pending_vset_tick) >= APP_SETPOINT_APPLY_DELAY_MS)
  {
    ids[count] = UF4_ID_SET_VOLTAGE_LIMIT;
    values[count] = AppController_FloatToMilliU16(vset);
    ++count;
    g_comm_last_vset = vset;
  }
  if (AppController_FloatChanged(iset, g_comm_last_iset, 0.005F) != 0U &&
      (now_tick - g_pending_iset_tick) >= APP_SETPOINT_APPLY_DELAY_MS)
  {
    ids[count] = UF4_ID_SET_CURRENT_LIMIT;
    values[count] = AppController_FloatToMilliU16(iset);
    ++count;
    g_comm_last_iset = iset;
  }
  if (AppController_FloatChanged(ovp, g_comm_last_ovp, 0.05F) != 0U)
  {
    ids[count] = UF4_ID_OVP_SET_VALUE;
    values[count] = AppController_FloatToMilliU16(ovp);
    ++count;
    g_comm_last_ovp = ovp;
  }
  if (AppController_FloatChanged(ocp, g_comm_last_ocp, 0.05F) != 0U)
  {
    ids[count] = UF4_ID_OCP_SET_VALUE;
    values[count] = AppController_FloatToMilliU16(ocp);
    ++count;
    g_comm_last_ocp = ocp;
  }
  if (AppController_FloatChanged(otp, g_comm_last_otp, 0.5F) != 0U)
  {
    ids[count] = UF4_ID_OTP_SET_VALUE;
    values[count] = AppController_FloatToCentiU16(otp);
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

/**
  * @brief Seed local panel values once all required remote controls are known.
  */
static void AppController_TrySeedControlsFromUf4(void)
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
  AppController_InitUf4ControlBaseline();
  g_remote_controls_seeded = 1U;
}

/**
  * @brief Request a compact first snapshot before enabling streaming updates.
  */
static bool AppController_RequestInitialStatusRead(void)
{
  static const uint8_t ids[] UF4_FAST_CONST =
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
    UF4_ID_FAN_SPEED
  };

  return UF4PowerClient_ReadU16Pairs(ids, (uint8_t)(sizeof(ids) / sizeof(ids[0])));
}

/**
  * @brief Copy UF4 register cache and local panel state into the GUI model.
  */
static void AppController_FillGuiFromUf4(GUI_Data_t *gui_data)
{
  uint16_t raw_value;

  memset(gui_data, 0, sizeof(*gui_data));

  gui_data->vin = AppController_ReadUf4Scaled(UF4_ID_INPUT_VOLTAGE, 1000.0F, 0.0F);
  gui_data->iin = AppController_ReadUf4Scaled(UF4_ID_INPUT_CURRENT, 1000.0F, 0.0F);
  gui_data->pin = gui_data->vin * gui_data->iin;
  gui_data->fan = AppController_ReadUf4Scaled(UF4_ID_FAN_SPEED, 10.0F, 0.0F);

  gui_data->vout = AppController_ReadUf4Scaled(UF4_ID_OUTPUT_VOLTAGE, 1000.0F, 0.0F);
  gui_data->iout = AppController_ReadUf4Scaled(UF4_ID_OUTPUT_CURRENT, 1000.0F, 0.0F);
  gui_data->pout = gui_data->vout * gui_data->iout;
  gui_data->power = gui_data->pout;

  gui_data->efficiency = (gui_data->pin > APP_FLOAT_EFFICIENCY_MIN_INPUT_W)
      ? (gui_data->pout * 100.0F) / gui_data->pin
      : 0.0F;

  gui_data->vset = SetpointInput_GetVset();
  gui_data->iset = SetpointInput_GetIset();
  gui_data->vset_digit = SetpointInput_GetVsetDigit();
  gui_data->iset_digit = SetpointInput_GetIsetDigit();
  gui_data->ovp = AppController_ReadUf4Scaled(UF4_ID_OVP_SET_VALUE, 1000.0F, PanelKeys_GetOvp());
  gui_data->ocp = AppController_ReadUf4Scaled(UF4_ID_OCP_SET_VALUE, 1000.0F, PanelKeys_GetOcp());
  gui_data->otp = AppController_ReadUf4Scaled(UF4_ID_OTP_SET_VALUE, 100.0F, PanelKeys_GetOtp());
  gui_data->ovp_enabled = PanelKeys_GetOvpEnabled();
  gui_data->ocp_enabled = PanelKeys_GetOcpEnabled();
  gui_data->otp_enabled = PanelKeys_GetOtpEnabled();
  gui_data->output_enabled = AppController_ReadUf4Bool(UF4_ID_POWER_STATE, PanelKeys_GetOutputEnabled());
  gui_data->ble_state = (uint8_t)PanelKeys_GetBleState();
  gui_data->page = (uint8_t)PanelKeys_GetPage();
  gui_data->panel_field = (uint8_t)PanelKeys_GetSelectedField();
  gui_data->scope_field = (uint8_t)PanelKeys_GetScopeField();
  gui_data->scope_timebase = PanelKeys_GetScopeTimebase();
  gui_data->scope_ch1_enabled = PanelKeys_GetScopeCh1Enabled();
  gui_data->scope_ch2_enabled = PanelKeys_GetScopeCh2Enabled();
  gui_data->scope_hold = PanelKeys_GetScopeHold();
  gui_data->scope_trigger = (uint8_t)PanelKeys_GetScopeTrigger();
  gui_data->scope_ch1_source = (uint8_t)PanelKeys_GetScopeCh1Source();
  gui_data->scope_ch2_source = (uint8_t)PanelKeys_GetScopeCh2Source();
  gui_data->scope_ch1_scale = PanelKeys_GetScopeCh1Scale();
  gui_data->scope_ch2_scale = PanelKeys_GetScopeCh2Scale();
  gui_data->comm_state = (uint8_t)g_comm_start_state;
  gui_data->comm_stream_enabled = (uint8_t)(UF4PowerClient_IsStreamEnabled() ? 1U : 0U);
  gui_data->comm_last_tx_cmd = UF4PowerClient_LastTxCmd();
  gui_data->comm_last_tx_ok = (uint8_t)(UF4PowerClient_LastTxOk() ? 1U : 0U);
  gui_data->comm_tx_ok_count = UF4PowerClient_TxOkCount();
  gui_data->comm_tx_fail_count = UF4PowerClient_TxFailCount();
  gui_data->comm_rx_frame_count = UF4PowerClient_RxFrameCount();
  gui_data->comm_rx_error_count = UF4PowerClient_RxErrorCount();

  gui_data->regulation_mode = (uint8_t)AppController_ReadUf4U16(UF4_ID_CC_CV_MODE, 0U);
  if (UF4PowerClient_GetU16(UF4_ID_CC_CV_MODE, &raw_value))
  {
    gui_data->valid_flags |= GUI_VALID_CC_CV_MODE;
    gui_data->regulation_mode = (uint8_t)raw_value;
  }
  if (UF4PowerClient_GetU16(UF4_ID_POWER_STATE, &raw_value))
  {
    gui_data->valid_flags |= GUI_VALID_POWER_STATE;
    gui_data->output_enabled = (uint8_t)(raw_value != 0U ? 1U : 0U);
  }
  gui_data->fault_state = (uint8_t)AppController_ReadUf4U16(UF4_ID_FAULT_STATE, 0U);
  if (UF4PowerClient_GetU16(UF4_ID_FAULT_STATE, &raw_value))
  {
    gui_data->valid_flags |= GUI_VALID_FAULT_STATE;
    gui_data->fault_state = (uint8_t)raw_value;
  }
  gui_data->state_machine_flags = AppController_ReadUf4U16(UF4_ID_STATE_MACHINE_FLAG_BITS, 0U);
  if (UF4PowerClient_GetU16(UF4_ID_STATE_MACHINE_FLAG_BITS, &raw_value))
  {
    gui_data->valid_flags |= GUI_VALID_STATE_MACHINE_FLAGS;
    gui_data->state_machine_flags = raw_value;
  }
  gui_data->state_machine_state = (uint8_t)AppController_ReadUf4U16(UF4_ID_STATE_MACHINE_STATE, 0U);
  if (UF4PowerClient_GetU16(UF4_ID_STATE_MACHINE_STATE, &raw_value))
  {
    gui_data->valid_flags |= GUI_VALID_STATE_MACHINE_STATE;
    gui_data->state_machine_state = (uint8_t)raw_value;
  }
  gui_data->duty_cmd = AppController_ReadUf4U16(UF4_ID_DUTY_CMD, 0U);
  gui_data->pwm_a_compare = AppController_ReadUf4U16(UF4_ID_PWM_A_COMPARE, 0U);
  if (UF4PowerClient_GetU16(UF4_ID_PWM_A_COMPARE, &raw_value))
  {
    gui_data->valid_flags |= GUI_VALID_PWM_A_COMPARE;
    gui_data->pwm_a_compare = raw_value;
  }
  gui_data->pwm_d_compare = AppController_ReadUf4U16(UF4_ID_PWM_D_COMPARE, 0U);
  if (UF4PowerClient_GetU16(UF4_ID_PWM_D_COMPARE, &raw_value))
  {
    gui_data->valid_flags |= GUI_VALID_PWM_D_COMPARE;
    gui_data->pwm_d_compare = raw_value;
  }
  gui_data->cpu_temp = AppController_ReadUf4Scaled(UF4_ID_CORE_TEMPERATURE, 100.0F, 0.0F);
  gui_data->buck_temp = AppController_ReadUf4Scaled(UF4_ID_TEMP1_TEMPERATURE, 100.0F, 0.0F);
  gui_data->boost_temp = AppController_ReadUf4Scaled(UF4_ID_TEMP2_TEMPERATURE, 100.0F, 0.0F);
}

/**
  * @brief Capture the current panel values as the last synchronized baseline.
  */
static void AppController_InitUf4ControlBaseline(void)
{
  const uint32_t now_tick = HAL_GetTick();

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
