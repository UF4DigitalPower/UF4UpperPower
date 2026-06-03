#include "app_user.h"

#include "gpio.h"
#include "gui_guider.h"
#include "lvgl.h"
#include "main.h"
#include "tim.h"
#include "usart.h"

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
  lv_obj_t **obj;
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

#define F4CP_SOF0 0xAAU
#define F4CP_SOF1 0x55U
#define F4CP_CMD_WRITE 0x02U
#define F4CP_TYPE_SET_VOLTAGE_LIMIT 17U
#define F4CP_TYPE_SET_CURRENT_LIMIT 18U
#define F4CP_TYPE_POWER_STATE 21U
#define F4CP_TYPE_OTP_SET_VALUE 30U
#define F4CP_TYPE_OVP_SET_VALUE 32U
#define F4CP_TYPE_OCP_SET_VALUE 34U
#define F4CP_TYPE_FAN_SET_VALUE 39U
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
#define APP_VI_SEND_DELAY_MS 500U
#define APP_ENCODER_FAST_MS 10U
#define APP_ENCODER_MEDIUM_MS 25U
#define APP_ENCODER_SLOW_MS 60U

static uint8_t g_f4cp_seq;
static app_input_id_t g_active_input = APP_INPUT_VSET;
static bool g_output_enabled;
static bool g_vi_send_pending;
static uint32_t g_vi_last_edit_tick;
static int32_t g_last_v_count;
static int32_t g_last_i_count;
static uint32_t g_last_v_encoder_tick;
static uint32_t g_last_i_encoder_tick;

static app_input_t g_inputs[APP_INPUT_COUNT] = {
  {APP_INPUT_VSET, &guider_ui.PAGE_MAIN_VSET_INPUT, F4CP_TYPE_SET_VOLTAGE_LIMIT, 0, 50000, 1000, 100, 2, APP_FIELD_COLOR_VSET},
  {APP_INPUT_ISET, &guider_ui.PAGE_MAIN_ISET_INPUT, F4CP_TYPE_SET_CURRENT_LIMIT, 0, 10000, 1000, 50, 2, APP_FIELD_COLOR_ISET},
  {APP_INPUT_OTP, &guider_ui.PAGE_MAIN_OTP_SET_INPUT, F4CP_TYPE_OTP_SET_VALUE, 0, 150000, 1000, 100, 2, APP_FIELD_COLOR_OTP},
  {APP_INPUT_OVP, &guider_ui.PAGE_MAIN_OVP_SET_INPUT, F4CP_TYPE_OVP_SET_VALUE, 0, 60000, 1000, 100, 2, APP_FIELD_COLOR_OVP},
  {APP_INPUT_OCP, &guider_ui.PAGE_MAIN_OCP_SET_INPUT, F4CP_TYPE_OCP_SET_VALUE, 0, 60000, 1000, 100, 2, APP_FIELD_COLOR_OCP},
  {APP_INPUT_UVP, &guider_ui.PAGE_MAIN_UVP_SET_INPUT, 0, 0, 60000, 1000, 100, 2, APP_FIELD_COLOR_UVP},
  {APP_INPUT_FAN, &guider_ui.PAGE_MAIN_FAN_SET_INPUT, F4CP_TYPE_FAN_SET_VALUE, 0, 1000, 100, 10, 2, APP_FIELD_COLOR_FAN},
};

static app_key_state_t g_keys[APP_KEY_COUNT] = {
  {KEY_UP_GPIO_Port, KEY_UP_Pin, true, false, false, 0},
  {KEY_DN_GPIO_Port, KEY_DN_Pin, true, false, false, 0},
  {KEY_L_GPIO_Port, KEY_L_Pin, true, false, false, 0},
  {KEY_R_GPIO_Port, KEY_R_Pin, true, false, false, 0},
  {KEY_M_GPIO_Port, KEY_M_Pin, true, false, false, 0},
  {KEY_V_PUSH_GPIO_Port, KEY_V_PUSH_Pin, true, false, false, 0},
  {KEY_I_PUSH_GPIO_Port, KEY_I_PUSH_Pin, true, false, false, 0},
};

static uint16_t APP_Crc16(const uint8_t *data, uint16_t len);
static void APP_PutU16(uint8_t *dst, uint16_t value);
static void APP_PutI32(uint8_t *dst, int32_t value);
static bool APP_SendFrame(uint8_t cmd, const uint8_t *payload, uint16_t payload_len);
static uint16_t APP_AddTlvI32(uint8_t *payload, uint16_t offset, uint8_t type, int32_t value);
static uint16_t APP_AddTlvU8(uint8_t *payload, uint16_t offset, uint8_t type, uint8_t value);
static int32_t APP_Pow10(uint8_t n);
static int32_t APP_FieldValue(const app_input_t *input);
static void APP_SetInputText(app_input_t *input, const char *text);
static void APP_SetFieldValue(app_input_t *input, int32_t value);
static void APP_UpdateFocus(void);
static void APP_SelectInput(app_input_id_t id);
static void APP_NextSettingsInput(void);
static void APP_AdjustInput(app_input_t *input, int32_t steps);
static void APP_SendVsetIset(void);
static void APP_SaveSettingsNoVi(void);
static void APP_ToggleOutput(void);
static void APP_HandleShortPress(app_key_id_t key);
static void APP_HandleLongPress(app_key_id_t key);
static bool APP_ReadKey(const app_key_state_t *key);
static void APP_ScanKeys(void);
static int32_t APP_EncoderDelta(TIM_HandleTypeDef *timer, int32_t *last_count);
static int32_t APP_EncoderStepMultiplier(uint32_t now, uint32_t *last_tick);
static void APP_ScanEncoders(void);
static void APP_SetAcceptedChars(void);
static void APP_DisableInputHoverAnimation(void);

static uint16_t APP_Crc16(const uint8_t *data, uint16_t len)
{
  uint16_t crc = 0xFFFFU;

  for (uint16_t i = 0; i < len; ++i) {
    crc ^= data[i];
    for (uint8_t bit = 0; bit < 8U; ++bit) {
      crc = (crc & 1U) ? (uint16_t)((crc >> 1U) ^ 0xA001U) : (uint16_t)(crc >> 1U);
    }
  }

  return crc;
}

static void APP_PutU16(uint8_t *dst, uint16_t value)
{
  dst[0] = (uint8_t)value;
  dst[1] = (uint8_t)(value >> 8U);
}

static void APP_PutI32(uint8_t *dst, int32_t value)
{
  dst[0] = (uint8_t)value;
  dst[1] = (uint8_t)((uint32_t)value >> 8U);
  dst[2] = (uint8_t)((uint32_t)value >> 16U);
  dst[3] = (uint8_t)((uint32_t)value >> 24U);
}

static bool APP_SendFrame(uint8_t cmd, const uint8_t *payload, uint16_t payload_len)
{
  uint8_t frame[96];
  const uint16_t body_len = (uint16_t)(2U + payload_len);
  const uint16_t frame_len = (uint16_t)(body_len + 6U);
  uint16_t crc;

  if (frame_len > sizeof(frame)) {
    return false;
  }

  frame[0] = F4CP_SOF0;
  frame[1] = F4CP_SOF1;
  APP_PutU16(&frame[2], body_len);
  frame[4] = cmd;
  frame[5] = g_f4cp_seq++;
  if (payload_len > 0U && payload != NULL) {
    memcpy(&frame[6], payload, payload_len);
  }
  crc = APP_Crc16(frame, (uint16_t)(body_len + 4U));
  APP_PutU16(&frame[body_len + 4U], crc);

  return HAL_UART_Transmit(&huart1, frame, frame_len, 20U) == HAL_OK;
}

static uint16_t APP_AddTlvI32(uint8_t *payload, uint16_t offset, uint8_t type, int32_t value)
{
  payload[offset++] = type;
  APP_PutU16(&payload[offset], 4U);
  offset = (uint16_t)(offset + 2U);
  APP_PutI32(&payload[offset], value);
  return (uint16_t)(offset + 4U);
}

static uint16_t APP_AddTlvU8(uint8_t *payload, uint16_t offset, uint8_t type, uint8_t value)
{
  payload[offset++] = type;
  APP_PutU16(&payload[offset], 1U);
  offset = (uint16_t)(offset + 2U);
  payload[offset++] = value;
  return offset;
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
  const char *text = lv_textarea_get_text(*input->obj);
  int32_t whole = 0;
  int32_t frac = 0;
  int32_t frac_scale = input->scale;
  bool after_dot = false;

  while (*text != '\0') {
    if (*text == '.') {
      after_dot = true;
    } else if (*text >= '0' && *text <= '9') {
      if (after_dot) {
        if (frac_scale > 1) {
          frac_scale /= 10;
          frac += (int32_t)(*text - '0') * frac_scale;
        }
      } else {
        whole = whole * 10 + (int32_t)(*text - '0');
      }
    }
    ++text;
  }

  return whole * input->scale + frac;
}

static void APP_SetInputText(app_input_t *input, const char *text)
{
  lv_obj_t *textarea = *input->obj;
  lv_obj_t *label = lv_textarea_get_label(textarea);

  lv_textarea_clear_selection(textarea);
  lv_anim_del(textarea, NULL);
  lv_anim_del(label, NULL);
  lv_label_set_text(label, text);
  lv_obj_scroll_to(textarea, 0, 0, LV_ANIM_OFF);
  lv_obj_set_pos(label, 0, 0);
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
    const bool active = i == (uint8_t)g_active_input;
    lv_obj_t *textarea = *g_inputs[i].obj;
    lv_obj_t *label = lv_textarea_get_label(textarea);
    uint32_t color = g_inputs[i].color;

    lv_obj_clear_state(*g_inputs[i].obj, LV_STATE_HOVERED | LV_STATE_FOCUSED | LV_STATE_EDITED | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(textarea, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(textarea, lv_color_hex(color), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label, lv_color_hex(color), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(textarea, active ? 1 : 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(textarea, lv_color_hex(APP_FIELD_COLOR_CURSOR), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_textarea_clear_selection(textarea);
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
  APP_UpdateFocus();
  if (input->id == APP_INPUT_VSET || input->id == APP_INPUT_ISET) {
    g_vi_send_pending = true;
    g_vi_last_edit_tick = HAL_GetTick();
  }
}

static void APP_SendVsetIset(void)
{
  uint8_t payload[14];
  uint16_t offset = 0;

  offset = APP_AddTlvI32(payload, offset, F4CP_TYPE_SET_VOLTAGE_LIMIT, APP_FieldValue(&g_inputs[APP_INPUT_VSET]));
  offset = APP_AddTlvI32(payload, offset, F4CP_TYPE_SET_CURRENT_LIMIT, APP_FieldValue(&g_inputs[APP_INPUT_ISET]));
  (void)APP_SendFrame(F4CP_CMD_WRITE, payload, offset);
}

static void APP_SaveSettingsNoVi(void)
{
  uint8_t payload[32];
  uint16_t offset = 0;

  offset = APP_AddTlvI32(payload, offset, F4CP_TYPE_OTP_SET_VALUE, APP_FieldValue(&g_inputs[APP_INPUT_OTP]));
  offset = APP_AddTlvI32(payload, offset, F4CP_TYPE_OVP_SET_VALUE, APP_FieldValue(&g_inputs[APP_INPUT_OVP]));
  offset = APP_AddTlvI32(payload, offset, F4CP_TYPE_OCP_SET_VALUE, APP_FieldValue(&g_inputs[APP_INPUT_OCP]));
  offset = APP_AddTlvI32(payload, offset, F4CP_TYPE_FAN_SET_VALUE, APP_FieldValue(&g_inputs[APP_INPUT_FAN]));
  (void)APP_SendFrame(F4CP_CMD_WRITE, payload, offset);
}

static void APP_ToggleOutput(void)
{
  uint8_t payload[4];
  uint16_t offset = 0;

  g_output_enabled = !g_output_enabled;
  offset = APP_AddTlvU8(payload, offset, F4CP_TYPE_POWER_STATE, g_output_enabled ? 1U : 0U);
  (void)APP_SendFrame(F4CP_CMD_WRITE, payload, offset);
  lv_label_set_text(guider_ui.PAGE_MAIN_FSM_LABEL, g_output_enabled ? "RUN" : "STOP");
}

static void APP_HandleShortPress(app_key_id_t key)
{
  switch (key) {
    case APP_KEY_UP:
      APP_AdjustInput(&g_inputs[g_active_input], 1);
      break;
    case APP_KEY_DOWN:
      APP_AdjustInput(&g_inputs[g_active_input], -1);
      break;
    case APP_KEY_LEFT:
      break;
    case APP_KEY_RIGHT:
      break;
    case APP_KEY_M:
      APP_NextSettingsInput();
      break;
    case APP_KEY_V_PUSH:
      APP_SelectInput(APP_INPUT_VSET);
      break;
    case APP_KEY_I_PUSH:
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
  } else if (key == APP_KEY_V_PUSH || key == APP_KEY_I_PUSH) {
    APP_ToggleOutput();
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
    return 2;
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
    APP_AdjustInput(&g_inputs[APP_INPUT_VSET], v_delta * APP_EncoderStepMultiplier(now, &g_last_v_encoder_tick));
  }
  if (i_delta != 0) {
    APP_SelectInput(APP_INPUT_ISET);
    APP_AdjustInput(&g_inputs[APP_INPUT_ISET], i_delta * APP_EncoderStepMultiplier(now, &g_last_i_encoder_tick));
  }
}

static void APP_SetAcceptedChars(void)
{
  for (uint8_t i = 0; i < APP_INPUT_COUNT; ++i) {
    lv_textarea_set_accepted_chars(*g_inputs[i].obj, "0123456789.");
  }
}

static void APP_DisableInputHoverAnimation(void)
{
  const lv_style_selector_t states[] = {
    LV_PART_MAIN | LV_STATE_DEFAULT,
    LV_PART_MAIN | LV_STATE_HOVERED,
    LV_PART_MAIN | LV_STATE_FOCUSED,
    LV_PART_MAIN | LV_STATE_EDITED,
    LV_PART_MAIN | LV_STATE_PRESSED,
    LV_PART_MAIN | (LV_STATE_HOVERED | LV_STATE_FOCUSED),
    LV_PART_MAIN | (LV_STATE_HOVERED | LV_STATE_PRESSED),
    LV_PART_MAIN | (LV_STATE_FOCUSED | LV_STATE_EDITED),
    LV_PART_MAIN | (LV_STATE_FOCUSED | LV_STATE_PRESSED),
  };

  for (uint8_t i = 0; i < APP_INPUT_COUNT; ++i) {
    lv_obj_t *textarea = *g_inputs[i].obj;
    lv_obj_t *label = lv_textarea_get_label(textarea);

    for (uint8_t s = 0; s < (uint8_t)(sizeof(states) / sizeof(states[0])); ++s) {
      lv_obj_remove_local_style_prop(textarea, LV_STYLE_TRANSITION, states[s]);
      lv_obj_set_style_transform_width(textarea, 0, states[s]);
      lv_obj_set_style_transform_height(textarea, 0, states[s]);
      lv_obj_set_style_translate_x(textarea, 0, states[s]);
      lv_obj_set_style_translate_y(textarea, 0, states[s]);
      lv_obj_set_style_transform_zoom(textarea, LV_IMG_ZOOM_NONE, states[s]);
      lv_obj_set_style_transform_angle(textarea, 0, states[s]);
    }

    lv_textarea_set_cursor_click_pos(textarea, false);
    lv_obj_set_scrollbar_mode(textarea, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(textarea, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(label, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_style_anim_time(textarea, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(textarea, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(label, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(textarea, LV_OPA_TRANSP, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(textarea, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(textarea, LV_OPA_COVER, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(textarea, lv_color_hex(APP_FIELD_COLOR_CURSOR), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(textarea, lv_color_hex(APP_FIELD_COLOR_SELECTED_TEXT), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(label, LV_OPA_COVER, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(label, lv_color_hex(APP_FIELD_COLOR_CURSOR), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label, lv_color_hex(APP_FIELD_COLOR_SELECTED_TEXT), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_pos(label, 0, 0);
  }
}

void APP_Init(void)
{
  APP_SetAcceptedChars();
  APP_DisableInputHoverAnimation();
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
  g_last_i_count = (int32_t)__HAL_TIM_GET_COUNTER(&htim2);
  g_last_v_count = (int32_t)__HAL_TIM_GET_COUNTER(&htim4);
  g_last_i_encoder_tick = HAL_GetTick();
  g_last_v_encoder_tick = g_last_i_encoder_tick;
  APP_UpdateFocus();
}

void APP_Tick(void)
{
  APP_ScanKeys();
  APP_ScanEncoders();

  if (g_vi_send_pending && (HAL_GetTick() - g_vi_last_edit_tick) >= APP_VI_SEND_DELAY_MS) {
    g_vi_send_pending = false;
    APP_SendVsetIset();
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
