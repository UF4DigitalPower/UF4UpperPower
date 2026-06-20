#include "app_user.h"

#include "gpio.h"
#include "main.h"
#include "tim.h"
#include "uf4_power_client.h"
#include "uf4com.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  APP_INPUT_VSET = 0,
  APP_INPUT_ISET,
  APP_INPUT_OTP,
  APP_INPUT_OVP,
  APP_INPUT_OCP,
  APP_INPUT_UVP,
  APP_INPUT_FAN,
  APP_INPUT_COUNT
} app_input_id_t;

typedef enum {
  APP_KEY_UP = 0,
  APP_KEY_DOWN,
  APP_KEY_LEFT,
  APP_KEY_RIGHT,
  APP_KEY_M,
  APP_KEY_V_PUSH,
  APP_KEY_I_PUSH,
  APP_KEY_COUNT
} app_key_id_t;

typedef struct {
  app_input_id_t id;
  uint8_t tlv_type;
  int32_t min_value;
  int32_t max_value;
  int32_t scale;
  int32_t step_value;
  uint8_t decimals;
  uint32_t color;
} app_input_t;

typedef struct {
  GPIO_TypeDef *port;
  uint16_t pin;
  bool active_high;
  bool is_down;
  bool long_fired;
  uint32_t down_tick;
} app_key_state_t;

#define APP_FIELD_COLOR_VSET 0x005BFFU
#define APP_FIELD_COLOR_ISET 0x00A651U
#define APP_FIELD_COLOR_OTP 0xE53935U
#define APP_FIELD_COLOR_OVP 0xD500F9U
#define APP_FIELD_COLOR_OCP 0xFF8F00U
#define APP_FIELD_COLOR_UVP 0x00A8D8U
#define APP_FIELD_COLOR_FAN 0x7C4DFFU
#define APP_FIELD_COLOR_CURSOR 0xFFD166U
#define APP_FIELD_COLOR_SELECTED_TEXT 0x000000U
#define APP_LONG_PRESS_MS 800U
#define APP_POWER_STREAM_START_DELAY_MS 300U
#define APP_POWER_CONNECT_TIMEOUT_MS 1000U
#define APP_ENCODER_FAST_MS 10U
#define APP_ENCODER_MEDIUM_MS 25U
#define APP_ENCODER_SLOW_MS 60U

static app_input_id_t g_active_input = APP_INPUT_VSET;
static bool g_output_enabled;
static bool g_input_dirty[APP_INPUT_COUNT];
static int32_t g_last_v_count;
static int32_t g_last_i_count;
static uint32_t g_last_v_encoder_tick;
static uint32_t g_last_i_encoder_tick;
static uint32_t g_last_power_rx_count;
static uint32_t g_last_power_rx_tick;
static bool g_power_initial_report_received;
static bool g_power_stream_start_requested;

static app_key_state_t g_keys[APP_KEY_COUNT] = {
  {KEY_UP_GPIO_Port, KEY_UP_Pin, true, false, false, 0},
  {KEY_DN_GPIO_Port, KEY_DN_Pin, true, false, false, 0},
  {KEY_L_GPIO_Port, KEY_L_Pin, true, false, false, 0},
  {KEY_R_GPIO_Port, KEY_R_Pin, true, false, false, 0},
  {KEY_M_GPIO_Port, KEY_M_Pin, true, false, false, 0},
  {KEY_V_PUSH_GPIO_Port, KEY_V_PUSH_Pin, true, false, false, 0},
  {KEY_I_PUSH_GPIO_Port, KEY_I_PUSH_Pin, true, false, false, 0},
};

static uint16_t APP_FieldValueU16(const app_input_t *input);
static int32_t APP_Pow10(uint8_t n);
static int32_t APP_FieldValue(const app_input_t *input);
static void APP_SetInputText(app_input_t *input, const char *text);
static void APP_SetFieldValue(app_input_t *input, int32_t value);
static void APP_UpdateFocus(void);
static void APP_SelectInput(app_input_id_t id);
static void APP_NextSettingsInput(void);
static void APP_AdjustInput(app_input_t *input, int32_t steps);
static void APP_SendInput(app_input_id_t id);
static void APP_SaveSettingsNoVi(void);
static void APP_SetOutput(bool enable);
static void APP_HandleShortPress(app_key_id_t key);
static void APP_HandleLongPress(app_key_id_t key);
static bool APP_ReadKey(const app_key_state_t *key);
static void APP_ScanKeys(void);
static int32_t APP_EncoderDelta(TIM_HandleTypeDef *timer, int32_t *last_count);
static int32_t APP_EncoderStepMultiplier(uint32_t now, uint32_t *last_tick);
static void APP_ScanEncoders(void);
static void APP_SetAcceptedChars(void);
static void APP_DisableInputHoverAnimation(void);
static void APP_RequestPowerStatus(void);
static void APP_StartPowerStream(void);
static void APP_UpdatePowerUi(void);

static uint16_t APP_FieldValueU16(const app_input_t *input)
{
  int32_t value = APP_FieldValue(input);

  if (value < 0) {
    value = 0;
  } else if (value > 65535) {
    value = 65535;
  }

  return (uint16_t)value;
}

static int32_t APP_Pow10(uint8_t n)
{
  int32_t value = 1;
  while (n-- > 0U) {
    value *= 10;
  }
  return value;
}

static int32_t APP_FieldValue(const app_input_t *input)
{
}

static void APP_SetInputText(app_input_t *input, const char *text)
{
}

static void APP_SetFieldValue(app_input_t *input, int32_t value)
{
  char text[16];

  if (value < input->min_value) {
    value = input->min_value;
  } else if (value > input->max_value) {
    value = input->max_value;
  }

  if (input->decimals == 0U) {
    snprintf(text, sizeof(text), "%ld", (long)value);
  } else {
    const int32_t whole = value / input->scale;
    const int32_t frac_divisor = input->scale / APP_Pow10(input->decimals);
    int32_t frac = labs(value % input->scale);
    if (frac_divisor > 1) {
      frac /= frac_divisor;
    }
    snprintf(text, sizeof(text), "%02ld.%02ld", (long)whole, (long)frac);
  }

  APP_SetInputText(input, text);
}

static void APP_UpdateFocus(void)
{
  for (uint8_t i = 0; i < APP_INPUT_COUNT; ++i) {
  }
}

static void APP_SelectInput(app_input_id_t id)
{
  if (id >= APP_INPUT_COUNT) {
    return;
  }

  g_active_input = id;
  APP_UpdateFocus();
}

static void APP_NextSettingsInput(void)
{
  if (g_active_input < APP_INPUT_OTP || g_active_input >= APP_INPUT_FAN) {
    APP_SelectInput(APP_INPUT_OTP);
    return;
  }

  APP_SelectInput((app_input_id_t)((uint8_t)g_active_input + 1U));
}

static void APP_AdjustInput(app_input_t *input, int32_t steps)
{
  const int32_t value = APP_FieldValue(input) + steps * input->step_value;

  APP_SetFieldValue(input, value);
  g_input_dirty[input->id] = true;
  APP_UpdateFocus();
}

static void APP_SendInput(app_input_id_t id)
{
}

static void APP_SaveSettingsNoVi(void)
{
  const uint8_t ids[] = {
    UF4_ID_OTP_SET_VALUE,
    UF4_ID_OVP_SET_VALUE,
    UF4_ID_OCP_SET_VALUE,
    UF4_ID_FAN_SET_VALUE,
  };
  const uint16_t values[] = {
  };

  if (UF4PowerClient_WriteU16Pairs(ids, values, 4U)) {
    g_input_dirty[APP_INPUT_OTP] = false;
    g_input_dirty[APP_INPUT_OVP] = false;
    g_input_dirty[APP_INPUT_OCP] = false;
    g_input_dirty[APP_INPUT_FAN] = false;
    APP_RequestPowerStatus();
  }
}

static void APP_SetOutput(bool enable)
{
  g_output_enabled = enable;
  if (UF4PowerClient_WriteU16(UF4_ID_POWER_STATE, g_output_enabled ? 1U : 0U)) {
    APP_RequestPowerStatus();
  }
}

static void APP_HandleShortPress(app_key_id_t key)
{
  switch (key) {
    case APP_KEY_UP:
      break;
    case APP_KEY_DOWN:
      break;
    case APP_KEY_LEFT:
      break;
    case APP_KEY_RIGHT:
      break;
    case APP_KEY_M:
      APP_NextSettingsInput();
      break;
    case APP_KEY_V_PUSH:
      if (g_active_input == APP_INPUT_VSET && g_input_dirty[APP_INPUT_VSET]) {
        APP_SendInput(APP_INPUT_VSET);
      }
      APP_SelectInput(APP_INPUT_VSET);
      break;
    case APP_KEY_I_PUSH:
      if (g_active_input == APP_INPUT_ISET && g_input_dirty[APP_INPUT_ISET]) {
        APP_SendInput(APP_INPUT_ISET);
      }
      APP_SelectInput(APP_INPUT_ISET);
      break;
    default:
      break;
  }
}

static void APP_HandleLongPress(app_key_id_t key)
{
  if (key == APP_KEY_M) {
    APP_SaveSettingsNoVi();
  } else if (key == APP_KEY_V_PUSH) {
    APP_SetOutput(true);
  } else if (key == APP_KEY_I_PUSH) {
    APP_SetOutput(false);
  }
}

static bool APP_ReadKey(const app_key_state_t *key)
{
  const bool high = HAL_GPIO_ReadPin(key->port, key->pin) == GPIO_PIN_SET;
  return key->active_high ? high : !high;
}

static void APP_ScanKeys(void)
{
  const uint32_t now = HAL_GetTick();

  for (uint8_t i = 0; i < APP_KEY_COUNT; ++i) {
    const bool down = APP_ReadKey(&g_keys[i]);
    if (down && !g_keys[i].is_down) {
      g_keys[i].is_down = true;
      g_keys[i].long_fired = false;
      g_keys[i].down_tick = now;
    } else if (down && !g_keys[i].long_fired && (now - g_keys[i].down_tick) >= APP_LONG_PRESS_MS) {
      g_keys[i].long_fired = true;
      APP_HandleLongPress((app_key_id_t)i);
    } else if (!down && g_keys[i].is_down) {
      if (!g_keys[i].long_fired) {
        APP_HandleShortPress((app_key_id_t)i);
      }
      g_keys[i].is_down = false;
    }
  }
}

static int32_t APP_EncoderDelta(TIM_HandleTypeDef *timer, int32_t *last_count)
{
  const int32_t count = (int32_t)__HAL_TIM_GET_COUNTER(timer);
  int32_t delta = count - *last_count;

  if (timer->Instance == TIM4) {
    if (delta > 32767) {
      delta -= 65536;
    } else if (delta < -32768) {
      delta += 65536;
    }
  }

  *last_count = count;
  return delta;
}

static int32_t APP_EncoderStepMultiplier(uint32_t now, uint32_t *last_tick)
{
  const uint32_t elapsed = now - *last_tick;

  *last_tick = now;

  if (elapsed <= APP_ENCODER_FAST_MS) {
    return 10;
  }
  if (elapsed <= APP_ENCODER_MEDIUM_MS) {
    return 5;
  }
  if (elapsed <= APP_ENCODER_SLOW_MS) {
    return 1;
  }
  return 1;
}

static void APP_ScanEncoders(void)
{
  const int32_t v_delta = APP_EncoderDelta(&htim4, &g_last_v_count);
  const int32_t i_delta = APP_EncoderDelta(&htim2, &g_last_i_count);
  const uint32_t now = HAL_GetTick();

  if (v_delta != 0) {
    APP_SelectInput(APP_INPUT_VSET);
  }
  if (i_delta != 0) {
    APP_SelectInput(APP_INPUT_ISET);
  }
}

static void APP_SetAcceptedChars(void)
{
  for (uint8_t i = 0; i < APP_INPUT_COUNT; ++i) {
  }
}

static void APP_DisableInputHoverAnimation(void)
{
}

static void APP_FormatFixed(char *text, size_t text_size, uint16_t value, uint16_t scale, uint8_t decimals)
{
  const uint16_t whole = (scale == 0U) ? value : (uint16_t)(value / scale);
  uint16_t frac = (scale == 0U) ? 0U : (uint16_t)(value % scale);

  if (decimals == 0U) {
    snprintf(text, text_size, "%u", (unsigned)value);
  } else if (decimals == 1U) {
    frac = (uint16_t)(frac / (scale / 10U));
    snprintf(text, text_size, "%u.%01u", (unsigned)whole, (unsigned)frac);
  } else {
    frac = (uint16_t)(frac / (scale / 100U));
    snprintf(text, text_size, "%u.%02u", (unsigned)whole, (unsigned)frac);
  }
}



static const char *APP_CcCvText(uint16_t value)
{
  return (value == 0U) ? "CC" : "CV";
}

static const char *APP_TopoText(uint16_t value)
{
  switch (value) {
    case 1U:
      return "BUCK";
    case 2U:
      return "BOOST";
    case 3U:
      return "MIX";
    default:
      return "NA";
  }
}

static const char *APP_FsmText(uint16_t value)
{
  if ((value & 0x0008U) != 0U) {
    return "RUN";
  }
  if ((value & 0x0004U) != 0U) {
    return "RISE";
  }
  if ((value & 0x0002U) != 0U) {
    return "WAIT";
  }
  if ((value & 0x0001U) != 0U) {
    return "INIT";
  }
  return "ERR";
}

static const char *APP_FaultText(uint16_t value)
{
  return (value == 0U) ? "NONE" : "FAULT";
}

static void APP_RequestPowerStatus(void)
{
  const uint8_t ids[] = {
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
    UF4_ID_OTP_SET_VALUE,
    UF4_ID_OVP_SET_VALUE,
    UF4_ID_OCP_SET_VALUE,
    UF4_ID_DUTY_CMD,
    UF4_ID_PWM_A_COMPARE,
    UF4_ID_PWM_D_COMPARE,
    UF4_ID_FAN_SPEED,
    UF4_ID_FAN_SET_VALUE,
  };

  (void)UF4PowerClient_ReadU16Pairs(ids, (uint8_t)(sizeof(ids) / sizeof(ids[0])));
}

static void APP_StartPowerStream(void)
{
  (void)UF4PowerClient_StartStreamAll();
  g_power_stream_start_requested = true;
}

void APP_Init(void)
{
  UF4PowerClient_Init();
  APP_SetAcceptedChars();
  APP_DisableInputHoverAnimation();

  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
  g_last_i_count = (int32_t)__HAL_TIM_GET_COUNTER(&htim2);
  g_last_v_count = (int32_t)__HAL_TIM_GET_COUNTER(&htim4);
  g_last_i_encoder_tick = HAL_GetTick();
  g_last_v_encoder_tick = g_last_i_encoder_tick;
  g_last_power_rx_tick = g_last_i_encoder_tick;
  APP_RequestPowerStatus();
  APP_UpdateFocus();
}

void APP_Tick(void)
{
  UF4PowerClient_Tick();
  APP_ScanKeys();
  APP_ScanEncoders();

  if (UF4PowerClient_ConsumeDataChanged() ||
      ((HAL_GetTick() - g_last_power_rx_tick) >= APP_POWER_CONNECT_TIMEOUT_MS)) {
    APP_UpdatePowerUi();
  }
  if (!g_power_stream_start_requested &&
      g_power_initial_report_received &&
      ((HAL_GetTick() - g_last_power_rx_tick) >= APP_POWER_STREAM_START_DELAY_MS)) {
    APP_StartPowerStream();
  }
}

void APP_OnExti(uint16_t gpio_pin)
{
  (void)gpio_pin;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  APP_OnExti(GPIO_Pin);
}
