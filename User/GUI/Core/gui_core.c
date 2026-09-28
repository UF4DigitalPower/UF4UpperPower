/**   ******************************************************************************
 *   * @file    gui_core.c
 *   * @brief   UF4 GUI module.
 *   ******************************************************************************
 * @attention
  *
  * Copyright (c) 2026 UF4.
  * All rights reserved.
  *
  * This software is provided "as is", without warranty of any kind.
  *
 */
#include "gui_internal.h"
#include "parameter_manager.h"
#include "g474_remote.h"
#include "esp32c6_bus.h"


ui_state_t g_ui =
{
  .page = UI_PAGE_HOME,
  .output_on = 0U,
  .step_index = 0U,
  .voltage_set = 5.0f,
  .current_limit = 2.0f,
  .output_voltage = 0.00f,
  .output_current = 0.00f,
  .param_page = 0U,
  .param_selected = 0U,
  .setting_page = 0U,
  .setting_selected = 0U,
  .hover_target = UI_HOVER_NONE
};

uint16_t g_ui_color_bg = UI_COLOR_BG_DEFAULT;
uint16_t g_ui_color_ink = UI_COLOR_INK_DEFAULT;
uint16_t g_ui_color_accent = UI_COLOR_ACCENT_DEFAULT;
uint16_t g_ui_color_panel = UI_COLOR_PANEL_DEFAULT;
uint16_t g_ui_color_muted = UI_COLOR_MUTED_DEFAULT;
uint16_t g_ui_color_disabled = UI_COLOR_DISABLED_DEFAULT;
uint16_t g_ui_color_border = UI_COLOR_BORDER_DEFAULT;
uint16_t g_ui_color_text = UI_COLOR_TEXT_DEFAULT;

ui_param_t g_params[UI_PARAM_COUNT] =
{
  { UF4_ID_OCP_SET_VALUE, "OCP", ACCESS_RW, 10.0f, 0.01f, 10.0f, "A", 2U },
  { UF4_ID_OTP_SET_VALUE, "OTP", ACCESS_RW, 120.0f, 40.0f, 150.0f, "C", 2U },
  { UF4_ID_CFG_VIN_OVP, "VIN_OVP", ACCESS_RW, 75.0f, 0.01f, 100.0f, "V", 2U },
  { UF4_ID_CFG_VIN_UVP, "VIN_UVP", ACCESS_RW, 5.0f, 0.0f, 100.0f, "V", 2U },
  { UF4_ID_CFG_VOUT_OVP, "VOUT_OVP", ACCESS_RW, 75.0f, 0.01f, 100.0f, "V", 2U },
  { UF4_ID_CFG_POWER_DIRECTION_MODE, "DIR_MODE", ACCESS_RW, 1.0f, 0.0f, 1.0f, "BOOL", 0U },
  { UF4_ID_CFG_ADC_VREF, "ADC_VREF", ACCESS_RW, 3.3f, 2.5f, 3.6f, "V", 2U },
  { UF4_ID_CFG_VIN_R_UPPER, "VIN_R_UP", ACCESS_RW, 750.0f, 1.0f, 65535.0f, "100R", 0U },
  { UF4_ID_CFG_VIN_R_LOWER, "VIN_R_LOW", ACCESS_RW, 30.0f, 1.0f, 65535.0f, "100R", 0U },
  { UF4_ID_CFG_VOUT_R_UPPER, "VOUT_R_UP", ACCESS_RW, 750.0f, 1.0f, 65535.0f, "100R", 0U },
  { UF4_ID_CFG_VOUT_R_LOWER, "VOUT_R_LOW", ACCESS_RW, 30.0f, 1.0f, 65535.0f, "100R", 0U },
  { UF4_ID_CFG_SHUNT_IN, "SHUNT_IN", ACCESS_RW, 800.0f, 1.0f, 65535.0f, "10uR", 0U },
  { UF4_ID_CFG_SHUNT_OUT, "SHUNT_OUT", ACCESS_RW, 800.0f, 1.0f, 65535.0f, "10uR", 0U },
  { UF4_ID_CFG_GAIN_IN, "GAIN_IN", ACCESS_RW, 1.0f, 0.01f, 10.0f, "X", 2U },
  { UF4_ID_CFG_GAIN_OUT, "GAIN_OUT", ACCESS_RW, 1.0f, 0.01f, 10.0f, "X", 2U },
  { UF4_ID_CFG_CURRENT_OFFSET, "I_OFFSET", ACCESS_RW, 0.0f, -327.68f, 327.67f, "A", 2U },
  { UF4_ID_CFG_VIN_GAIN, "VIN_CAL_GAIN", ACCESS_RW, 1.0f, 0.10f, 3.0f, "X", 2U },
  { UF4_ID_CFG_VIN_OFFSET, "VIN_CAL_OFF", ACCESS_RW, 0.0f, -327.68f, 327.67f, "V", 2U },
  { UF4_ID_CFG_VOUT_GAIN, "VOUT_CAL_GAIN", ACCESS_RW, 1.0f, 0.10f, 3.0f, "X", 2U },
  { UF4_ID_CFG_VOUT_OFFSET, "VOUT_CAL_OFF", ACCESS_RW, 0.0f, -327.68f, 327.67f, "V", 2U },
  { UF4_ID_CFG_IIN_GAIN, "IIN_CAL_GAIN", ACCESS_RW, 1.0f, 0.10f, 3.0f, "X", 2U },
  { UF4_ID_CFG_IIN_OFFSET, "IIN_CAL_OFF", ACCESS_RW, 0.0f, -327.68f, 327.67f, "A", 2U },
  { UF4_ID_CFG_IOUT_GAIN, "IOUT_CAL_GAIN", ACCESS_RW, 1.0f, 0.10f, 3.0f, "X", 2U },
  { UF4_ID_CFG_IOUT_OFFSET, "IOUT_CAL_OFF", ACCESS_RW, 0.0f, -327.68f, 327.67f, "A", 2U },
  { UF4_ID_CFG_CURRENT_KP, "CURRENT_KP", ACCESS_RW, 0.0f, 0.0f, 6.5535f, "GAIN", 4U },
  { UF4_ID_CFG_CURRENT_KI, "CURRENT_KI", ACCESS_RW, 0.0f, 0.0f, 6.5535f, "GAIN", 4U },
  { UF4_ID_CFG_CURRENT_KD, "CURRENT_KD", ACCESS_RW, 0.0f, 0.0f, 0.0100f, "GAIN", 4U },
  { UF4_ID_CFG_VOLTAGE_KP, "VOLTAGE_KP", ACCESS_RW, 1.0f, 0.0f, 6.5535f, "GAIN", 4U },
  { UF4_ID_CFG_VOLTAGE_KI, "VOLTAGE_KI", ACCESS_RW, 0.0f, 0.0f, 6.5535f, "GAIN", 4U },
  { UF4_ID_CFG_VOLTAGE_KD, "VOLTAGE_KD", ACCESS_RW, 0.0f, 0.0f, 0.0100f, "GAIN", 4U },
  { UF4_ID_FAN_SET_VALUE, "FAN_SET", ACCESS_RW, 100.0f, 0.0f, 100.0f, "%", 0U }
};

static const char * const opt_control_source[] = { "LOCAL", "CAN", "UART" };
static const char * const opt_off_on[] = { "OFF", "ON" };
static const char * const opt_startup[] = { "OFF", "LAST", "ON" };
static const char * const opt_power_key[] = { "TAP", "HOLD" };
static const char * const opt_can_baud[] = { "250K", "500K", "1M" };
static const char * const opt_can_id[] = { "01", "02", "03", "04" };
static const char * const opt_uart_baud[] = { "9600", "115200", "921600" };
static const char * const opt_heartbeat[] = { "1S", "5S", "NEVER" };
static const char * const opt_brightness[] = { "15%", "30%", "50%", "65%", "80%", "100%" };
static const char * const opt_sleep[] = { "30S", "2MIN", "10MIN", "NEVER" };
static const char * const opt_latch_retry[] = { "LATCH", "RETRY" };
static const char * const opt_manual_auto[] = { "MANUAL", "AUTO" };
static const char * const opt_fan_mode[] = { "OFF", "AUTO", "FULL" };
static const char * const opt_fan_pwm[] = { "15%", "25%", "40%" };
static const char * const opt_popup_test[] = { "TEST" };
static const char * const opt_idle_run[] = { "IDLE", "RUN" };
static const char * const opt_adc_cal[] = { "READY", "START" };
static const char * const opt_idle_save[] = { "IDLE", "SAVE" };
static const char * const opt_idle_load[] = { "IDLE", "LOAD" };
static const char * const opt_color_preset[] = { "CLASSIC", "LIGHT", "DARK", "LIME", "OCEAN", "SUNSET", "MINT", "AMBER" };
static const char * const opt_esp32_start_mode[] = { "NONE", "WIFI", "BLE" };
static const char * const opt_hw_rev[] = { "H743-A2" };
static const char * const opt_fw_ver[] = { "1.8.0" };
static const char * const opt_boot_ver[] = { "0.6.2" };
static const char * const opt_serial[] = { "UF4-2608" };

ui_setting_t g_settings[UI_SETTING_COUNT] =
{
  {  8U, "SYSTEM",  "HEARTBEAT",     ACCESS_RW, 0U, opt_heartbeat,      3U, "F429 USART1 runtime log period." },
  { UI_SETTING_ID_POPUP_TEST, "UI", "POPUP_TEST", ACCESS_RW, 0U, opt_popup_test, 1U,
    "Show a local notification test message." },
  {  9U, "DISPLAY", "BRIGHTNESS",     ACCESS_RW, 5U, opt_brightness,     6U, "LCD backlight brightness." },
  { 11U, "DISPLAY", "SLEEP_TIME",     ACCESS_RW, 3U, opt_sleep,          4U, "Screen sleep timeout." },
  { 29U, "DISPLAY", "COLOR_PRESET",   ACCESS_RW, 0U, opt_color_preset,   UI_COLOR_PRESET_COUNT, "Apply a preset color scheme." },
  { UI_SETTING_ID_ESP32_START_MODE, "ESP32", "START_MODE", ACCESS_RW, UI_ESP32_START_NONE,
    opt_esp32_start_mode, 3U, "Select ESP32-S3 service and lock local settings." }
};

const float g_steps[3] = { 0.02f, 0.2f, 2.0f };

typedef struct
{
  uint16_t bg;
  uint16_t ink;
  uint16_t accent;
  uint16_t panel;
  uint16_t muted;
  uint16_t disabled;
  uint16_t border;
  uint16_t text;
} ui_color_scheme_t;

static const ui_color_scheme_t g_color_presets[] =
{
  { 0xF7BDU, 0x1082U, 0xD7E8U, 0xFFFFU, 0x6B4CU, 0xE73BU, 0x1082U, 0x1082U },
  { 0xFBFFU, 0x0861U, 0x9E6DU, 0xFFFFU, 0x7BEFU, 0xE73BU, 0x0861U, 0x0861U },
  { 0x1082U, 0xF7BDU, 0xD7E8U, 0x2104U, 0xC638U, 0x39E7U, 0xF7BDU, 0xF7BDU },
  { 0x1A69U, 0xFFFFU, 0x07FFU, 0x294AU, 0x6B4DU, 0x4208U, 0x07FFU, 0xFFFFU },
  { 0xD6FCU, 0x3186U, 0xFBE0U, 0xFFDFU, 0x8C71U, 0xC618U, 0x3186U, 0x3186U },
  { 0xDFF3U, 0x0468U, 0x07E0U, 0xFFFFU, 0x6C8DU, 0xCE7BU, 0x0468U, 0x0468U },
  { 0xE72AU, 0x2A08U, 0xFD20U, 0xFFF7U, 0xA534U, 0xD69AU, 0x2A08U, 0x2A08U },
  { 0xFEF5U, 0x2124U, 0xFD60U, 0xFFFFU, 0x8C51U, 0xDEDBU, 0x2124U, 0x2124U }
};

_Static_assert((sizeof(g_color_presets) / sizeof(g_color_presets[0])) == UI_COLOR_PRESET_COUNT,
               "color preset table must match setting options");

static uint8_t ui_color_preset_count(void)
{
  return (uint8_t)(sizeof(g_color_presets) / sizeof(g_color_presets[0]));
}

static ui_setting_t *ui_find_setting(uint16_t id)
{
  uint16_t i;

  for (i = 0U; i < (uint16_t)(sizeof(g_settings) / sizeof(g_settings[0])); ++i)
  {
    if (g_settings[i].id == id)
    {
      return &g_settings[i];
    }
  }

  return 0;
}

uint32_t ui_color_preset_flash_error(void)
{
  return UF4_ParameterManager_LastSaveOk() != 0U ? 0U : 1U;
}

uint8_t UI_Esp32ModeIsActive(void)
{
  ui_setting_t *setting = ui_find_setting(UI_SETTING_ID_ESP32_START_MODE);

  return (setting != 0 && setting->current != UI_ESP32_START_NONE) ? 1U : 0U;
}

uint8_t UI_ParamIsEditable(const ui_param_t *param)
{
  return (param != 0 && param->access == ACCESS_RW && UI_Esp32ModeIsActive() == 0U) ? 1U : 0U;
}

uint8_t UI_SettingIsEditable(const ui_setting_t *setting)
{
  if (setting == 0 || setting->access != ACCESS_RW)
  {
    return 0U;
  }

  return 1U;
}

uint32_t ui_color_preset_flash_sector_error(void)
{
  return UF4_ParameterManager_IsReady() != 0U ? 0U : 0xFFFFFFFFU;
}

static void ui_apply_color_scheme(const ui_color_scheme_t *scheme)
{
  if (scheme == 0)
  {
    return;
  }

  g_ui_color_bg = scheme->bg;
  g_ui_color_ink = scheme->ink;
  g_ui_color_accent = scheme->accent;
  g_ui_color_panel = scheme->panel;
  g_ui_color_muted = scheme->muted;
  g_ui_color_disabled = scheme->disabled;
  g_ui_color_border = scheme->border;
  g_ui_color_text = scheme->text;
}

void ui_apply_color_preset(uint8_t preset)
{
  if (preset >= ui_color_preset_count())
  {
    preset = 0U;
  }

  ui_apply_color_scheme(&g_color_presets[preset]);
}

uint8_t ui_set_color_preset(uint8_t preset, uint8_t persist)
{
  ui_setting_t *setting = ui_find_setting(29U);

  if (preset >= ui_color_preset_count())
  {
    preset = 0U;
  }

  ui_apply_color_preset(preset);
  if (setting != 0)
  {
    setting->current = preset;
  }
  if (persist != 0U)
  {
    UF4_ParameterManager_SaveNow();
  }
  return 1U;
}

uint8_t ui_load_color_preset(void)
{
  ui_setting_t *setting = ui_find_setting(29U);

  return ui_set_color_preset((setting != 0) ? setting->current : 0U, 0U);
}

uint8_t GUI_ParamGet(uint16_t id, float *value)
{
  uint16_t i;

  if (value == 0)
  {
    return 0U;
  }

  for (i = 0U; i < UI_PARAM_COUNT; ++i)
  {
    if (g_params[i].id == id)
    {
      *value = g_params[i].value;
      return 1U;
    }
  }

  /* Telemetry is intentionally absent from the editable parameter page.
     Read it from the G474 register cache for the home/status presentation. */
  return G474_Remote_ReadFloat((uint8_t)id, value);
}

uint8_t GUI_ParamSet(uint16_t id, float value)
{
  uint16_t i;

  for (i = 0U; i < UI_PARAM_COUNT; ++i)
  {
    if (g_params[i].id == id)
    {
      if (UI_ParamIsEditable(&g_params[i]) == 0U)
      {
        return 0U;
      }
      if (value < g_params[i].min)
      {
        value = g_params[i].min;
      }
      if (value > g_params[i].max)
      {
        value = g_params[i].max;
      }
      if(G474_Remote_WriteFloat((uint8_t)id, value) == 0U)
      {
        return 0U;
      }
      g_params[i].value = value;
      return 1U;
    }
  }

  return 0U;
}

uint8_t GUI_SettingGet(uint16_t id, uint8_t *value)
{
  ui_setting_t *setting = ui_find_setting(id);

  if (value == 0 || setting == 0)
  {
    return 0U;
  }

  *value = setting->current;
  return 1U;
}

uint8_t GUI_SettingSet(uint16_t id, uint8_t value)
{
  ui_setting_t *setting = ui_find_setting(id);

  if (UI_SettingIsEditable(setting) == 0U)
  {
    return 0U;
  }

  if (setting->option_count > 0U && value >= setting->option_count)
  {
    value = (uint8_t)(setting->option_count - 1U);
  }

  if (id == UI_SETTING_ID_POPUP_TEST)
  {
    return 1U;
  }

  setting->current = value;
  if (id == UI_SETTING_ID_ESP32_START_MODE)
  {
    (void)ESP32C6_BusSetServiceMode(value);
  }
  if (id == 29U)
  {
    return ui_set_color_preset(value, 1U);
  }
  if (id == 9U)
  {
    LCD_SetBacklightLevel(value);
  }
  if (id == 23U && value == 1U)
  {
    UF4_ParameterManager_SaveNow();
    setting->current = 0U;
    return 1U;
  }
  if (id == 24U && value == 1U)
  {
    UF4_ParameterManager_LoadDefaults();
    return 1U;
  }

  UF4_ParameterManager_OnWriteApply(0);
  return 1U;
}
