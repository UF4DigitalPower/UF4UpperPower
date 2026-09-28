/**
  ******************************************************************************
  * @file    gui_home.c
  * @author  UF4
  * @date    26-8-14
  * @brief   Home page drawing and touch handling.
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
#include "gui_internal.h"
#include "g474_remote.h"
#include "parameter_manager.h"

#define HOME_SET_Y_V              386U
#define HOME_SET_Y_I              464U
#define HOME_SET_ROW_H             78U
#define HOME_SET_BUTTON_W          56U
#define HOME_SET_VALUE_W          208U
#define HOME_SET_INC_X            264U
#define HOME_SET_AREA_W           320U
#define HOME_PRESET_X             320U
#define HOME_PRESET_W             160U
#define HOME_PRESET_H             156U
#define HOME_PRESET_SELECT_Y      HOME_SET_Y_V
#define HOME_PRESET_SELECT_H       40U
#define HOME_PRESET_VALUE_Y       426U
#define HOME_PRESET_VALUE_H        64U
#define HOME_PRESET_VALUE_W        80U
#define HOME_PRESET_SAVE_X        HOME_PRESET_X
#define HOME_PRESET_SAVE_Y        490U
#define HOME_PRESET_BUTTON_W       80U
#define HOME_PRESET_APPLY_X       400U
#define HOME_PRESET_BUTTON_H       52U
#define HOME_VOLTAGE_MAX          75.0f
#define HOME_CURRENT_MAX          10.0f

static ui_home_preset_t g_home_presets[UI_HOME_PRESET_COUNT] =
{
  {  5.0f,  2.0f },
  { 12.0f,  2.0f },
  { 24.0f,  3.2f },
  { 48.0f,  5.0f }
};

static uint8_t g_home_preset_selected;
static float g_home_energy_wh;
static uint32_t g_home_energy_tick;

static const char *home_fault_text(uint16_t fault)
{
  if (G474_Remote_IsOnline() == 0U) { return "COMM"; }
  if ((fault & 0x0001U) != 0U) { return "OCP"; }
  if ((fault & 0x0002U) != 0U) { return "VIN OVP"; }
  if ((fault & 0x0004U) != 0U) { return "VOUT OVP"; }
  if ((fault & 0x0008U) != 0U) { return "OTP"; }
  if ((fault & 0x0010U) != 0U) { return "HW FLT"; }
  if ((fault & 0x0020U) != 0U) { return "VIN UVP"; }
  if ((fault & 0x0040U) != 0U) { return "ISR OVR"; }
  return "NORMAL";
}

static void clamp_home_setpoints(void)
{
  if (g_ui.voltage_set < 0.0f) { g_ui.voltage_set = 0.0f; }
  if (g_ui.voltage_set > HOME_VOLTAGE_MAX) { g_ui.voltage_set = HOME_VOLTAGE_MAX; }
  if (g_ui.current_limit < 0.0f) { g_ui.current_limit = 0.0f; }
  if (g_ui.current_limit > HOME_CURRENT_MAX) { g_ui.current_limit = HOME_CURRENT_MAX; }
}

void ui_home_export_state(uint8_t *step_index,
                          ui_home_preset_t *presets,
                          uint8_t *selected)
{
  if (step_index != 0)
  {
    *step_index = g_ui.step_index;
  }

  if (presets != 0)
  {
    (void)memcpy(presets, g_home_presets, sizeof(g_home_presets));
  }

  if (selected != 0)
  {
    *selected = g_home_preset_selected;
  }
}

void ui_home_import_state(uint8_t step_index,
                          const ui_home_preset_t *presets,
                          uint8_t selected)
{
  if (step_index >= (uint8_t)(sizeof(g_steps) / sizeof(g_steps[0])))
  {
    step_index = 0U;
  }
  if (selected >= UI_HOME_PRESET_COUNT)
  {
    selected = 0U;
  }

  g_ui.step_index = step_index;
  g_home_preset_selected = selected;

  if (presets != 0)
  {
    (void)memcpy(g_home_presets, presets, sizeof(g_home_presets));
    for (uint8_t i = 0U; i < UI_HOME_PRESET_COUNT; i++)
    {
      if (g_home_presets[i].voltage < 0.0f) { g_home_presets[i].voltage = 0.0f; }
      if (g_home_presets[i].voltage > HOME_VOLTAGE_MAX) { g_home_presets[i].voltage = HOME_VOLTAGE_MAX; }
      if (g_home_presets[i].current < 0.0f) { g_home_presets[i].current = 0.0f; }
      if (g_home_presets[i].current > HOME_CURRENT_MAX) { g_home_presets[i].current = HOME_CURRENT_MAX; }
    }
  }
}

static void draw_home_set_row(uint8_t row)
{
  char value[24];
  char with_unit[32];
  uint16_t y = (row == 0U) ? HOME_SET_Y_V : HOME_SET_Y_I;
  float setpoint = (row == 0U) ? g_ui.voltage_set : g_ui.current_limit;
  uint8_t locked = UI_Esp32ModeIsActive();

  draw_button(0U, y, HOME_SET_BUTTON_W, HOME_SET_ROW_H, "-", 0U, locked);
  draw_cell(HOME_SET_BUTTON_W, y, HOME_SET_VALUE_W, HOME_SET_ROW_H, UI_COLOR_PANEL);
  (void)snprintf(value, sizeof(value), "%05.2f", (double)setpoint);
  draw_text_center(HOME_SET_BUTTON_W, (uint16_t)(y + 8U), HOME_SET_VALUE_W, 24U,
                   (row == 0U) ? "VOLTAGE SET" : "CURRENT LIMIT", 1U, UI_COLOR_MUTED);
  (void)snprintf(with_unit, sizeof(with_unit), "%s %c", value, (row == 0U) ? 'V' : 'A');
  draw_text_center(HOME_SET_BUTTON_W, (uint16_t)(y + 34U), HOME_SET_VALUE_W, 34U, with_unit, 3U, UI_COLOR_TEXT);
  draw_button(HOME_SET_INC_X, y, HOME_SET_BUTTON_W, HOME_SET_ROW_H, "+", 0U, locked);
}

static void draw_home_preset_panel(void)
{
  const ui_home_preset_t *preset = &g_home_presets[g_home_preset_selected];
  char label[24];
  char voltage[16];
  char current[16];
  uint8_t select_active = (g_ui.hover_target == UI_HOVER_HOME_PRESET_SELECT);
  uint8_t save_active = (g_ui.hover_target == UI_HOVER_HOME_PRESET_SAVE);
  uint8_t apply_active = (g_ui.hover_target == UI_HOVER_HOME_PRESET_APPLY);

  (void)snprintf(label, sizeof(label), "PRESET P%u", (unsigned)(g_home_preset_selected + 1U));
  draw_cell(HOME_PRESET_X, HOME_PRESET_SELECT_Y, HOME_PRESET_W, HOME_PRESET_SELECT_H,
            select_active ? UI_COLOR_INK : UI_COLOR_ACCENT);
  draw_text_center(HOME_PRESET_X, HOME_PRESET_SELECT_Y, HOME_PRESET_W, HOME_PRESET_SELECT_H,
                   label, 2U, select_active ? UI_COLOR_BG : UI_COLOR_TEXT);

  draw_cell(HOME_PRESET_X, HOME_PRESET_VALUE_Y, HOME_PRESET_VALUE_W, HOME_PRESET_VALUE_H, UI_COLOR_PANEL);
  draw_cell(HOME_PRESET_APPLY_X, HOME_PRESET_VALUE_Y, HOME_PRESET_VALUE_W, HOME_PRESET_VALUE_H, UI_COLOR_PANEL);
  draw_text_center(HOME_PRESET_X, HOME_PRESET_VALUE_Y, HOME_PRESET_VALUE_W, 18U, "VSET", 1U, UI_COLOR_MUTED);
  draw_text_center(HOME_PRESET_APPLY_X, HOME_PRESET_VALUE_Y, HOME_PRESET_VALUE_W, 18U, "ISET", 1U, UI_COLOR_MUTED);
  (void)snprintf(voltage, sizeof(voltage), "%.1f", (double)preset->voltage);
  (void)snprintf(current, sizeof(current), "%.1f", (double)preset->current);
  draw_text_center(HOME_PRESET_X, 448U, HOME_PRESET_VALUE_W, 34U, voltage, 3U, UI_COLOR_TEXT);
  draw_text_center(HOME_PRESET_APPLY_X, 448U, HOME_PRESET_VALUE_W, 34U, current, 3U, UI_COLOR_TEXT);

  draw_button(HOME_PRESET_SAVE_X, HOME_PRESET_SAVE_Y, HOME_PRESET_BUTTON_W, HOME_PRESET_BUTTON_H,
              "SAVE", save_active, UI_Esp32ModeIsActive());
  draw_button(HOME_PRESET_APPLY_X, HOME_PRESET_SAVE_Y, HOME_PRESET_BUTTON_W, HOME_PRESET_BUTTON_H,
              "APPLY", apply_active, UI_Esp32ModeIsActive());
}

void handle_home_preset_hover(uint8_t target, uint8_t active)
{
  if (target == UI_HOVER_HOME_PRESET_SELECT)
  {
    draw_home_preset_panel();
    sync_dirty(HOME_PRESET_X, HOME_PRESET_SELECT_Y, HOME_PRESET_W, HOME_PRESET_SELECT_H);
  }
  else if (target == UI_HOVER_HOME_PRESET_SAVE)
  {
    draw_button(HOME_PRESET_SAVE_X, HOME_PRESET_SAVE_Y, HOME_PRESET_BUTTON_W, HOME_PRESET_BUTTON_H,
                "SAVE", active, 0U);
    sync_dirty(HOME_PRESET_SAVE_X, HOME_PRESET_SAVE_Y, HOME_PRESET_BUTTON_W, HOME_PRESET_BUTTON_H);
  }
  else if (target == UI_HOVER_HOME_PRESET_APPLY)
  {
    draw_button(HOME_PRESET_APPLY_X, HOME_PRESET_SAVE_Y, HOME_PRESET_BUTTON_W, HOME_PRESET_BUTTON_H,
                "APPLY", active, 0U);
    sync_dirty(HOME_PRESET_APPLY_X, HOME_PRESET_SAVE_Y, HOME_PRESET_BUTTON_W, HOME_PRESET_BUTTON_H);
  }
}

static void draw_home_stepbar(void)
{
  uint8_t locked = UI_Esp32ModeIsActive();

  draw_cell(0U, 542U, 160U, 44U, locked ? UI_COLOR_DISABLED : ((g_ui.step_index == 0U) ? UI_COLOR_ACCENT : UI_COLOR_PANEL));
  draw_text_center(0U, 542U, 160U, 44U, "0.02", 1U, locked ? UI_COLOR_MUTED : ((g_ui.step_index == 0U) ? UI_COLOR_TEXT : UI_COLOR_MUTED));
  draw_cell(160U, 542U, 160U, 44U, locked ? UI_COLOR_DISABLED : ((g_ui.step_index == 1U) ? UI_COLOR_ACCENT : UI_COLOR_PANEL));
  draw_text_center(160U, 542U, 160U, 44U, "0.2", 1U, locked ? UI_COLOR_MUTED : ((g_ui.step_index == 1U) ? UI_COLOR_TEXT : UI_COLOR_MUTED));
  draw_cell(320U, 542U, 160U, 44U, locked ? UI_COLOR_DISABLED : ((g_ui.step_index == 2U) ? UI_COLOR_ACCENT : UI_COLOR_PANEL));
  draw_text_center(320U, 542U, 160U, 44U, "2", 1U, locked ? UI_COLOR_MUTED : ((g_ui.step_index == 2U) ? UI_COLOR_TEXT : UI_COLOR_MUTED));
}

/* Draw only the regions whose values are supplied by the G474.  The static
 * setpoint, preset and step controls are rendered by draw_home_page(). */
void draw_home_telemetry(void)
{
  char vbuf[24];
  char ibuf[24];
  char meta_l[32];
  char meta_r[32];
  char energy[16];
  char efficiency[16];
  char values[6][16];
  float input_voltage = 0.0f;
  float input_current = 0.0f;
  float output_voltage = 0.0f;
  float output_current = 0.0f;
  float temp_mcu = 0.0f;
  float temp_ntc1 = 0.0f;
  float temp_ntc2 = 0.0f;
  float fan_set = 0.0f;
  float fault_state = 0.0f;
  float power_state = 0.0f;
  float cc_cv_mode = 0.0f;
  float converter_mode = 0.0f;
  float output_power;
  float input_power;
  float efficiency_percent;
  uint32_t now_ms;
  const char *power_state_text;
  const char *converter_mode_text;
  const char *cc_cv_text;
  uint8_t i;
  static const char *labels[] = { "TEMP MCU", "TEMP NTC1", "TEMP NTC2", "FAN", "FAULT", "STATE" };

  (void)GUI_ParamGet(UF4_ID_INPUT_VOLTAGE, &input_voltage);
  (void)GUI_ParamGet(UF4_ID_INPUT_CURRENT, &input_current);
  /* Read the live register cache, rather than the UI state mirror.  This
     keeps the primary values on the same telemetry path as the other fields. */
  (void)GUI_ParamGet(UF4_ID_OUTPUT_VOLTAGE, &output_voltage);
  (void)GUI_ParamGet(UF4_ID_OUTPUT_CURRENT, &output_current);
  (void)GUI_ParamGet(UF4_ID_CORE_TEMPERATURE, &temp_mcu);
  (void)GUI_ParamGet(UF4_ID_TEMP1_TEMPERATURE, &temp_ntc1);
  (void)GUI_ParamGet(UF4_ID_TEMP2_TEMPERATURE, &temp_ntc2);
  (void)GUI_ParamGet(UF4_ID_FAN_SET_VALUE, &fan_set);
  (void)GUI_ParamGet(UF4_ID_FAULT_STATE, &fault_state);
  (void)GUI_ParamGet(UF4_ID_POWER_STATE, &power_state);
  (void)GUI_ParamGet(UF4_ID_CC_CV_MODE, &cc_cv_mode);
  (void)GUI_ParamGet(UF4_ID_POWER_CONVERTER_MODE, &converter_mode);

  switch((uint16_t)power_state)
  {
    case 0U: power_state_text = "BOOT"; break;
    case 1U: power_state_text = "HW INIT"; break;
    case 2U: power_state_text = "PARAM"; break;
    case 3U: power_state_text = "CHECK"; break;
    case 4U: power_state_text = "IDLE"; break;
    case 5U: power_state_text = "START"; break;
    case 6U: power_state_text = "RUN"; break;
    case 7U: power_state_text = "STOP"; break;
    case 8U: power_state_text = "SWITCH"; break;
    case 9U: power_state_text = "FAULT"; break;
    default: power_state_text = "FAULT"; break;
  }
  switch((uint16_t)converter_mode)
  {
    case 0U: converter_mode_text = "BUCK"; break;
    case 1U: converter_mode_text = "MIX"; break;
    default: converter_mode_text = "BOOST"; break;
  }
  cc_cv_text = (cc_cv_mode != 0.0f) ? "CC" : "CV";

  (void)snprintf(values[0], sizeof(values[0]), "%.2fC", (double)temp_mcu);
  (void)snprintf(values[1], sizeof(values[1]), "%.2fC", (double)temp_ntc1);
  (void)snprintf(values[2], sizeof(values[2]), "%.2fC", (double)temp_ntc2);
  (void)snprintf(values[3], sizeof(values[3]), "%.0f%%", (double)fan_set);
  (void)snprintf(values[4], sizeof(values[4]), "%s", home_fault_text((uint16_t)fault_state));
  (void)snprintf(values[5], sizeof(values[5]), "%s", power_state_text);

  (void)snprintf(vbuf, sizeof(vbuf), "%05.2f", (double)output_voltage);
  (void)snprintf(ibuf, sizeof(ibuf), "%05.2f", (double)output_current);

  input_power = input_voltage * input_current;
  output_power = output_voltage * output_current;
  now_ms = HAL_GetTick();
  if(g_home_energy_tick != 0U)
  {
    const uint32_t elapsed_ms = (uint32_t)(now_ms - g_home_energy_tick);

    g_home_energy_wh += output_power * (float)elapsed_ms / 3600000.0f;
  }
  g_home_energy_tick = now_ms;

  efficiency_percent = (input_power > 0.05f) ? (output_power * 100.0f / input_power) : 0.0f;
  if(efficiency_percent < 0.0f) { efficiency_percent = 0.0f; }
  if(efficiency_percent > 100.0f) { efficiency_percent = 100.0f; }
  (void)snprintf(energy, sizeof(energy), "%.2fWH", (double)g_home_energy_wh);
  (void)snprintf(efficiency, sizeof(efficiency), "%.1f%%", (double)efficiency_percent);
  (void)snprintf(meta_l, sizeof(meta_l), "INPUT VOLTAGE : %05.2fV", (double)input_voltage);
  (void)snprintf(meta_r, sizeof(meta_r), "INPUT POWER : %05.2fW", (double)input_power);

  draw_big_value(0U, 118U, 480U, 134U, "OUTPUT VOLTAGE", cc_cv_text, vbuf, "V", meta_l, meta_r, 1U, "ENERGY", energy);

  (void)snprintf(meta_l, sizeof(meta_l), "INPUT CURRENT : %05.2fA", (double)input_current);
  (void)snprintf(meta_r, sizeof(meta_r), "OUTPUT POWER : %05.2fW", (double)output_power);

  draw_big_value(0U, 252U, 480U, 134U, "OUTPUT CURRENT", "LIVE", ibuf, "A", meta_l, meta_r, 0U, "EFF", efficiency);

  for (i = 0U; i < 6U; ++i)
  {
    uint16_t x = (uint16_t)((i % 3U) * 160U);
    uint16_t y = (uint16_t)(586U + (i / 3U) * 68U);
    draw_cell(x, y, 160U, 68U, UI_COLOR_PANEL);
    draw_text((uint16_t)(x + 14U), (uint16_t)(y + 10U), labels[i], 1U, UI_COLOR_MUTED);
    draw_text((uint16_t)(x + 14U), (uint16_t)(y + 36U), values[i], 2U, UI_COLOR_TEXT);
  }

  draw_cell(0U, 722U, 480U, 26U, UI_COLOR_BG);
  draw_text(14U, 728U, "CAN 500K", 1U, UI_COLOR_MUTED);
  (void)snprintf(meta_l, sizeof(meta_l), "MODE : %s / %s", cc_cv_text, converter_mode_text);
  draw_text_center(0U, 728U, 480U, 18U, meta_l, 1U, UI_COLOR_MUTED);
  draw_text_right(360U, 728U, 106U, "00:00:00", 1U, UI_COLOR_MUTED);
}

void draw_home_page(void)
{
  draw_home_live_content();
  draw_home_preset_panel();
  draw_home_stepbar();
}

void draw_home_live_content(void)
{
  draw_home_telemetry();
  draw_home_set_row(0U);
  draw_home_set_row(1U);
}

static void adjust_home_value(uint8_t target)
{
  if (UI_Esp32ModeIsActive() != 0U)
  {
    return;
  }

  float step = g_steps[g_ui.step_index];
  if (target == 1U && g_ui.voltage_set > 0.0f) { g_ui.voltage_set -= step; }
  if (target == 2U && g_ui.voltage_set < HOME_VOLTAGE_MAX) { g_ui.voltage_set += step; }
  if (target == 3U && g_ui.current_limit > 0.0f) { g_ui.current_limit -= step; }
  if (target == 4U && g_ui.current_limit < HOME_CURRENT_MAX) { g_ui.current_limit += step; }
  clamp_home_setpoints();
  (void)G474_Remote_Write((target <= 2U) ? UF4_ID_SET_VOLTAGE_LIMIT : UF4_ID_SET_CURRENT_LIMIT,
                          (uint16_t)(((target <= 2U) ? g_ui.voltage_set : g_ui.current_limit) * 100.0f));
}

static void draw_home_hover_button(uint8_t target, uint8_t active)
{
  if (target == UI_HOVER_HOME_VDEC)
  {
    draw_button(0U, HOME_SET_Y_V, HOME_SET_BUTTON_W, HOME_SET_ROW_H, "-", active, 0U);
  }
  else if (target == UI_HOVER_HOME_VINC)
  {
    draw_button(HOME_SET_INC_X, HOME_SET_Y_V, HOME_SET_BUTTON_W, HOME_SET_ROW_H, "+", active, 0U);
  }
  else if (target == UI_HOVER_HOME_IDEC)
  {
    draw_button(0U, HOME_SET_Y_I, HOME_SET_BUTTON_W, HOME_SET_ROW_H, "-", active, 0U);
  }
  else if (target == UI_HOVER_HOME_IINC)
  {
    draw_button(HOME_SET_INC_X, HOME_SET_Y_I, HOME_SET_BUTTON_W, HOME_SET_ROW_H, "+", active, 0U);
  }
}

void handle_home_button_hover(uint8_t target, uint8_t active)
{
  if (target != UI_HOVER_HOME_VDEC && target != UI_HOVER_HOME_VINC &&
      target != UI_HOVER_HOME_IDEC && target != UI_HOVER_HOME_IINC)
  {
    return;
  }

  draw_home_hover_button(target, active);
  if (target == UI_HOVER_HOME_VDEC || target == UI_HOVER_HOME_VINC)
  {
    sync_dirty((target == UI_HOVER_HOME_VDEC) ? 0U : HOME_SET_INC_X,
               HOME_SET_Y_V, HOME_SET_BUTTON_W, HOME_SET_ROW_H);
  }
  else
  {
    sync_dirty((target == UI_HOVER_HOME_IDEC) ? 0U : HOME_SET_INC_X,
               HOME_SET_Y_I, HOME_SET_BUTTON_W, HOME_SET_ROW_H);
  }
}

void handle_home_repeat(uint8_t target)
{
  adjust_home_value(target);
  draw_home_set_row((target >= 3U) ? 1U : 0U);
  if (target == 1U)
  {
    draw_home_hover_button(UI_HOVER_HOME_VDEC, 1U);
  }
  else if (target == 2U)
  {
    draw_home_hover_button(UI_HOVER_HOME_VINC, 1U);
  }
  else if (target == 3U)
  {
    draw_home_hover_button(UI_HOVER_HOME_IDEC, 1U);
  }
  else if (target == 4U)
  {
    draw_home_hover_button(UI_HOVER_HOME_IINC, 1U);
  }
  sync_dirty(0U, (target >= 3U) ? HOME_SET_Y_I : HOME_SET_Y_V,
             HOME_SET_AREA_W, HOME_SET_ROW_H);
}

void handle_home_touch(uint16_t x, uint16_t y, uint8_t is_press)
{
  char message[32];

  if (!is_press)
  {
    return;
  }

  if (UI_Esp32ModeIsActive() != 0U)
  {
    GUI_PopupShow("CONTROL LOCKED", "ESP32 SERVICE ACTIVE", 1500U);
    return;
  }

  if (pt_in(x, y, 0U, HOME_SET_Y_V, HOME_SET_BUTTON_W, HOME_SET_ROW_H))
  {
    g_ui.hover_target = UI_HOVER_HOME_VDEC;
    g_ui.repeat_target = 1U;
    handle_home_repeat(g_ui.repeat_target);
  }
  else if (pt_in(x, y, HOME_SET_INC_X, HOME_SET_Y_V, HOME_SET_BUTTON_W, HOME_SET_ROW_H))
  {
    g_ui.hover_target = UI_HOVER_HOME_VINC;
    g_ui.repeat_target = 2U;
    handle_home_repeat(g_ui.repeat_target);
  }
  else if (pt_in(x, y, 0U, HOME_SET_Y_I, HOME_SET_BUTTON_W, HOME_SET_ROW_H))
  {
    g_ui.hover_target = UI_HOVER_HOME_IDEC;
    g_ui.repeat_target = 3U;
    handle_home_repeat(g_ui.repeat_target);
  }
  else if (pt_in(x, y, HOME_SET_INC_X, HOME_SET_Y_I, HOME_SET_BUTTON_W, HOME_SET_ROW_H))
  {
    g_ui.hover_target = UI_HOVER_HOME_IINC;
    g_ui.repeat_target = 4U;
    handle_home_repeat(g_ui.repeat_target);
  }
  else if (pt_in(x, y, HOME_PRESET_X, HOME_SET_Y_V, HOME_PRESET_W,
                 (uint16_t)(HOME_PRESET_SAVE_Y - HOME_SET_Y_V)))
  {
    g_home_preset_selected = (uint8_t)((g_home_preset_selected + 1U) % UI_HOME_PRESET_COUNT);
    g_ui.hover_target = UI_HOVER_HOME_PRESET_SELECT;
    draw_home_preset_panel();
    sync_dirty(HOME_PRESET_X, HOME_SET_Y_V, HOME_PRESET_W, HOME_PRESET_H);
  }
  else if (pt_in(x, y, HOME_PRESET_SAVE_X, HOME_PRESET_SAVE_Y,
                 HOME_PRESET_BUTTON_W, HOME_PRESET_BUTTON_H))
  {
    g_ui.hover_target = UI_HOVER_HOME_PRESET_SAVE;
    g_home_presets[g_home_preset_selected].voltage = g_ui.voltage_set;
    g_home_presets[g_home_preset_selected].current = g_ui.current_limit;
    UF4_ParameterManager_SaveNow();
    GUI_LogPrintf("PRESET P%u SAVED", (unsigned)(g_home_preset_selected + 1U));
    draw_home_preset_panel();
    sync_dirty(HOME_PRESET_X, HOME_SET_Y_V, HOME_PRESET_W, HOME_PRESET_H);
    (void)snprintf(message, sizeof(message), "P%u SET SAVED",
                   (unsigned)(g_home_preset_selected + 1U));
    GUI_PopupShow("PRESET", message, 1500U);
  }
  else if (pt_in(x, y, HOME_PRESET_APPLY_X, HOME_PRESET_SAVE_Y,
                 HOME_PRESET_BUTTON_W, HOME_PRESET_BUTTON_H))
  {
    g_ui.hover_target = UI_HOVER_HOME_PRESET_APPLY;
    g_ui.voltage_set = g_home_presets[g_home_preset_selected].voltage;
    g_ui.current_limit = g_home_presets[g_home_preset_selected].current;
    clamp_home_setpoints();
    GUI_LogPrintf("PRESET P%u APPLIED", (unsigned)(g_home_preset_selected + 1U));
    (void)G474_Remote_Write(UF4_ID_SET_VOLTAGE_LIMIT, (uint16_t)(g_ui.voltage_set * 100.0f));
    (void)G474_Remote_Write(UF4_ID_SET_CURRENT_LIMIT, (uint16_t)(g_ui.current_limit * 100.0f));
    draw_home_set_row(0U);
    draw_home_set_row(1U);
    draw_home_preset_panel();
    sync_dirty(0U, HOME_SET_Y_V, UI_W, HOME_PRESET_H);
    (void)snprintf(message, sizeof(message), "P%u SET APPLIED",
                   (unsigned)(g_home_preset_selected + 1U));
    GUI_PopupShow("PRESET", message, 1500U);
  }
  else if (pt_in(x, y, 0U, 542U, 160U, 44U))
  {
    g_ui.step_index = 0U;
    UF4_ParameterManager_OnWriteApply(0);
    draw_home_stepbar();
    sync_dirty(0U, 542U, 480U, 44U);
    return;
  }
  else if (pt_in(x, y, 160U, 542U, 160U, 44U))
  {
    g_ui.step_index = 1U;
    UF4_ParameterManager_OnWriteApply(0);
    draw_home_stepbar();
    sync_dirty(0U, 542U, 480U, 44U);
    return;
  }
  else if (pt_in(x, y, 320U, 542U, 160U, 44U))
  {
    g_ui.step_index = 2U;
    UF4_ParameterManager_OnWriteApply(0);
    draw_home_stepbar();
    sync_dirty(0U, 542U, 480U, 44U);
    return;
  }
  else
  {
    g_ui.repeat_target = 0U;
    g_ui.hover_target = UI_HOVER_NONE;
    draw_home_preset_panel();
    draw_home_stepbar();
    sync_dirty(HOME_PRESET_X, HOME_SET_Y_V, HOME_PRESET_W, HOME_PRESET_H);
    sync_dirty(0U, 542U, UI_W, 44U);
    return;
  }
}
