/**
  ******************************************************************************
  * @file    gui_params.c
  * @author  UF4
  * @date    26-8-14
  * @brief   Params page drawing and touch handling.
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

#define PARAM_PAGE_BUTTON_Y       118U
#define PARAM_PAGE_BUTTON_W       72U
#define PARAM_PAGE_BUTTON_H       44U
#define PARAM_PAGE_PREV_X         0U
#define PARAM_PAGE_NEXT_X         408U
#define PARAM_ROW_X               0U
#define PARAM_ROW_Y               162U
#define PARAM_ROW_W               480U
#define PARAM_ROW_H               56U
#define PARAM_EDITOR_Y            498U
#define PARAM_EDITOR_H            128U
#define PARAM_DEC_X               0U
#define PARAM_DEC_W               82U
#define PARAM_INC_X               398U
#define PARAM_INC_W               82U
#define PARAM_STEP_Y              626U
#define PARAM_STEP_W              160U
#define PARAM_STEP_H              48U
#define PARAM_INFO_Y              674U
#define PARAM_INFO_H              38U
#define PARAM_FOOTER_Y            712U
#define PARAM_FOOTER_H            36U

typedef enum
{
  PARAM_TOUCH_NONE = 0,
  PARAM_TOUCH_PAGE_PREV,
  PARAM_TOUCH_PAGE_NEXT,
  PARAM_TOUCH_ROW,
  PARAM_TOUCH_DEC,
  PARAM_TOUCH_INC,
  PARAM_TOUCH_STEP,
  PARAM_TOUCH_SAVE
} param_touch_zone_t;

static uint64_t s_dirty_param_mask;
static volatile uint64_t s_remote_param_update_mask;

static uint16_t param_index(uint8_t row)
{
  return (uint16_t)(g_ui.param_page * UI_PARAMS_PER_PAGE + row);
}

static ui_param_t *param_at_row(uint8_t row)
{
  uint16_t index = param_index(row);

  if (index >= UI_PARAM_COUNT)
  {
    return 0;
  }

  return &g_params[index];
}

static param_touch_zone_t param_touch_zone(uint16_t x, uint16_t y, uint8_t *slot)
{
  if (slot != 0)
  {
    *slot = 0U;
  }

  if (pt_in(x, y, PARAM_PAGE_PREV_X, PARAM_PAGE_BUTTON_Y, PARAM_PAGE_BUTTON_W, PARAM_PAGE_BUTTON_H))
  {
    return PARAM_TOUCH_PAGE_PREV;
  }
  if (pt_in(x, y, PARAM_PAGE_NEXT_X, PARAM_PAGE_BUTTON_Y, PARAM_PAGE_BUTTON_W, PARAM_PAGE_BUTTON_H))
  {
    return PARAM_TOUCH_PAGE_NEXT;
  }
  if (pt_in(x, y, PARAM_ROW_X, PARAM_ROW_Y, PARAM_ROW_W, UI_PARAMS_PER_PAGE * PARAM_ROW_H))
  {
    if (slot != 0)
    {
      *slot = (uint8_t)((y - PARAM_ROW_Y) / PARAM_ROW_H);
    }
    return PARAM_TOUCH_ROW;
  }
  if (pt_in(x, y, PARAM_DEC_X, PARAM_EDITOR_Y, PARAM_DEC_W, PARAM_EDITOR_H))
  {
    return PARAM_TOUCH_DEC;
  }
  if (pt_in(x, y, PARAM_INC_X, PARAM_EDITOR_Y, PARAM_INC_W, PARAM_EDITOR_H))
  {
    return PARAM_TOUCH_INC;
  }
  if (pt_in(x, y, 0U, PARAM_STEP_Y, UI_W, PARAM_STEP_H))
  {
    if (x >= (uint16_t)(2U * PARAM_STEP_W))
    {
      return PARAM_TOUCH_SAVE;
    }
    if (slot != 0)
    {
      *slot = (uint8_t)(x / PARAM_STEP_W);
    }
    return PARAM_TOUCH_STEP;
  }

  return PARAM_TOUCH_NONE;
}

static void draw_param_row(uint8_t row)
{
  uint16_t index = param_index(row);
  uint16_t y = (uint16_t)(PARAM_ROW_Y + row * PARAM_ROW_H);
  char buf[48];
  uint8_t selected_row = (uint8_t)(row == g_ui.param_selected);

  if (index >= UI_PARAM_COUNT)
  {
    draw_cell(0U, y, UI_W, PARAM_ROW_H, UI_COLOR_BG);
    return;
  }

  ui_param_t *p = &g_params[index];
  uint8_t editable = UI_ParamIsEditable(p);
  uint16_t row_bg = selected_row ? UI_COLOR_INK : UI_COLOR_PANEL;

  draw_cell(0U, y, 54U, PARAM_ROW_H, selected_row ? UI_COLOR_ACCENT : UI_COLOR_PANEL);
  (void)snprintf(buf, sizeof(buf), "P%03u", (unsigned)p->id);
  draw_text_center(0U, y, 54U, PARAM_ROW_H, buf, 1U, selected_row ? UI_COLOR_INK : UI_COLOR_MUTED);
  draw_cell(54U, y, 280U, PARAM_ROW_H, row_bg);
  draw_text(64U, (uint16_t)(y + 18U), p->name, 1U, selected_row ? UI_COLOR_BG : UI_COLOR_TEXT);
  draw_cell(334U, y, 54U, PARAM_ROW_H, row_bg);
  draw_text_center(334U, y, 54U, PARAM_ROW_H, editable ? "RW" : "EO", 1U, selected_row ? UI_COLOR_ACCENT : UI_COLOR_MUTED);
  draw_cell(388U, y, 92U, PARAM_ROW_H, editable ? UI_COLOR_ACCENT : UI_COLOR_BG);
  format_param_value(buf, sizeof(buf), p);
  draw_text_center(388U, y, 92U, PARAM_ROW_H, buf, 1U, editable ? UI_COLOR_INK : UI_COLOR_MUTED);
}

static float param_fine_step(const ui_param_t *p)
{
  switch(p->id)
  {
    case UF4_ID_OCP_SET_VALUE: return 0.1f;
    case UF4_ID_CFG_VIN_OVP:
    case UF4_ID_CFG_VOUT_OVP:
    case UF4_ID_OTP_SET_VALUE:
    case UF4_ID_CFG_VIN_UVP:
    case UF4_ID_FAN_SET_VALUE: return 1.0f;
    case UF4_ID_CFG_VIN_R_UPPER:
    case UF4_ID_CFG_VIN_R_LOWER:
    case UF4_ID_CFG_VOUT_R_UPPER:
    case UF4_ID_CFG_VOUT_R_LOWER: return 10.0f;
    default: break;
  }
  if (p->digits >= 4U) { return 0.0001f; }
  if (p->digits == 3U) { return 0.001f; }
  if (p->digits == 2U) { return 0.01f; }
  if (p->digits == 1U) { return 0.1f; }
  return 1.0f;
}

static float param_coarse_step(const ui_param_t *p)
{
  switch(p->id)
  {
    case UF4_ID_OCP_SET_VALUE: return 1.0f;
    case UF4_ID_CFG_CURRENT_OFFSET:
    case UF4_ID_CFG_VIN_OFFSET: return 0.1f;
    case UF4_ID_CFG_VOUT_OFFSET:
    case UF4_ID_CFG_IIN_OFFSET:
    case UF4_ID_CFG_IOUT_OFFSET: return 0.1f;
    case UF4_ID_CFG_VIN_OVP:
    case UF4_ID_CFG_VOUT_OVP:
    case UF4_ID_OTP_SET_VALUE:
    case UF4_ID_CFG_VIN_UVP:
    case UF4_ID_FAN_SET_VALUE: return 10.0f;
    case UF4_ID_CFG_VIN_R_UPPER:
    case UF4_ID_CFG_VIN_R_LOWER:
    case UF4_ID_CFG_VOUT_R_UPPER:
    case UF4_ID_CFG_VOUT_R_LOWER: return 100.0f;
    default: break;
  }
  const float span = p->max - p->min;

  if (span >= 1000.0f) { return 100.0f; }
  if (span >= 100.0f) { return 10.0f; }
  if (span >= 10.0f) { return 1.0f; }
  if (p->digits == 0U) { return 1.0f; }
  if (span >= 1.0f) { return 0.1f; }
  if (span >= 0.1f) { return 0.01f; }
  return 0.001f;
}

static void draw_params_stepbar(void)
{
  ui_param_t *p = &g_params[param_index(g_ui.param_selected)];
  char fine[16];
  char coarse[16];

  (void)snprintf(fine, sizeof(fine), "%.4g", (double)param_fine_step(p));
  (void)snprintf(coarse, sizeof(coarse), "%.4g", (double)param_coarse_step(p));
  draw_cell(0U, PARAM_STEP_Y, PARAM_STEP_W, PARAM_STEP_H, (g_ui.step_index == 0U) ? UI_COLOR_ACCENT : UI_COLOR_PANEL);
  draw_text_center(0U, PARAM_STEP_Y, PARAM_STEP_W, PARAM_STEP_H, fine, 1U, (g_ui.step_index == 0U) ? UI_COLOR_TEXT : UI_COLOR_MUTED);
  draw_cell(PARAM_STEP_W, PARAM_STEP_Y, PARAM_STEP_W, PARAM_STEP_H, (g_ui.step_index == 1U) ? UI_COLOR_ACCENT : UI_COLOR_PANEL);
  draw_text_center(PARAM_STEP_W, PARAM_STEP_Y, PARAM_STEP_W, PARAM_STEP_H, coarse, 1U, (g_ui.step_index == 1U) ? UI_COLOR_TEXT : UI_COLOR_MUTED);
  draw_button((uint16_t)(2U * PARAM_STEP_W), PARAM_STEP_Y, PARAM_STEP_W, PARAM_STEP_H,
              "SAVE", 0U, G474_Remote_IsSaveInProgress() != 0U ? 1U : 0U);
}

static void refresh_params_save_button(void)
{
  draw_button((uint16_t)(2U * PARAM_STEP_W), PARAM_STEP_Y, PARAM_STEP_W, PARAM_STEP_H,
              "SAVE", 0U, G474_Remote_IsSaveInProgress() != 0U ? 1U : 0U);
  sync_dirty((uint16_t)(2U * PARAM_STEP_W), PARAM_STEP_Y, PARAM_STEP_W, PARAM_STEP_H);
}

static void draw_params_editor_detail(void)
{
  ui_param_t *selected = &g_params[param_index(g_ui.param_selected)];
  char buf[48];

  draw_button(PARAM_DEC_X, PARAM_EDITOR_Y, PARAM_DEC_W, PARAM_EDITOR_H, "-", 0U, (uint8_t)(UI_ParamIsEditable(selected) == 0U));
  draw_cell(PARAM_DEC_W, PARAM_EDITOR_Y, 316U, PARAM_EDITOR_H, UI_COLOR_PANEL);
  draw_text_center(82U, 524U, 316U, 24U, selected->name, 1U, UI_COLOR_MUTED);
  format_param_value(buf, sizeof(buf), selected);
  draw_text_center(82U, 564U, 316U, 38U, buf, 3U, UI_COLOR_TEXT);
  draw_button(PARAM_INC_X, PARAM_EDITOR_Y, PARAM_INC_W, PARAM_EDITOR_H, "+", 0U, (uint8_t)(UI_ParamIsEditable(selected) == 0U));

  draw_cell(0U, PARAM_INFO_Y, 160U, PARAM_INFO_H, UI_COLOR_PANEL);
  draw_cell(160U, PARAM_INFO_Y, 160U, PARAM_INFO_H, UI_COLOR_PANEL);
  draw_cell(320U, PARAM_INFO_Y, 160U, PARAM_INFO_H, UI_COLOR_PANEL);
  draw_text(12U, 679U, "ACCESS", 1U, UI_COLOR_MUTED);
  draw_text_right(76U, 679U, 72U, UI_ParamIsEditable(selected) ? "RW" : "EO", 1U, UI_COLOR_TEXT);
  draw_text(172U, 679U, "RANGE", 1U, UI_COLOR_MUTED);
  (void)snprintf(buf, sizeof(buf), "%.0f-%.0f", (double)selected->min, (double)selected->max);
  draw_text_right(222U, 679U, 86U, buf, 1U, UI_COLOR_TEXT);
  draw_text(332U, 679U, "UNIT", 1U, UI_COLOR_MUTED);
  draw_text_right(376U, 679U, 92U, selected->unit, 1U, UI_COLOR_TEXT);
}

static void draw_params_hover_button(uint8_t target, uint8_t active)
{
  ui_param_t *selected = &g_params[param_index(g_ui.param_selected)];

  if (UI_ParamIsEditable(selected) == 0U)
  {
    return;
  }

  if (target == UI_HOVER_PARAM_DEC)
  {
    draw_button(PARAM_DEC_X, PARAM_EDITOR_Y, PARAM_DEC_W, PARAM_EDITOR_H, "-", active, 0U);
  }
  else if (target == UI_HOVER_PARAM_INC)
  {
    draw_button(PARAM_INC_X, PARAM_EDITOR_Y, PARAM_INC_W, PARAM_EDITOR_H, "+", active, 0U);
  }
}

void handle_params_button_hover(uint8_t target, uint8_t active)
{
  if (target != UI_HOVER_PARAM_DEC && target != UI_HOVER_PARAM_INC)
  {
    return;
  }

  draw_params_hover_button(target, active);
  sync_dirty((target == UI_HOVER_PARAM_DEC) ? PARAM_DEC_X : PARAM_INC_X,
             PARAM_EDITOR_Y,
             (target == UI_HOVER_PARAM_DEC) ? PARAM_DEC_W : PARAM_INC_W,
             PARAM_EDITOR_H);
}

static void draw_params_editor_value(void)
{
  ui_param_t *selected = &g_params[param_index(g_ui.param_selected)];
  char buf[48];

  draw_cell(PARAM_DEC_W, PARAM_EDITOR_Y, 316U, PARAM_EDITOR_H, UI_COLOR_PANEL);
  draw_text_center(82U, 524U, 316U, 24U, selected->name, 1U, UI_COLOR_MUTED);
  format_param_value(buf, sizeof(buf), selected);
  draw_text_center(82U, 564U, 316U, 38U, buf, 3U, UI_COLOR_TEXT);
}

void draw_params_page(void)
{
  uint16_t start = (uint16_t)(g_ui.param_page * UI_PARAMS_PER_PAGE);
  uint16_t end = (uint16_t)(start + UI_PARAMS_PER_PAGE);
  uint8_t row;
  char buf[48];
  ui_param_t *selected;

  if (end > UI_PARAM_COUNT)
  {
    end = UI_PARAM_COUNT;
  }

  draw_button(0U, 118U, 72U, 44U, "PREV", 1U, (uint8_t)(g_ui.param_page == 0U));
  (void)snprintf(buf, sizeof(buf), "PARAM %02u-%02u / %u",
                 (unsigned)(start + 1U), (unsigned)end, (unsigned)UI_PARAM_COUNT);
  draw_cell(72U, 118U, 336U, 44U, UI_COLOR_PANEL);
  draw_text_center(72U, 118U, 336U, 44U, buf, 1U, UI_COLOR_TEXT);
  draw_button(408U, 118U, 72U, 44U, "NEXT", 1U, (uint8_t)(g_ui.param_page >= (UI_PARAMS_PAGE_COUNT - 1U)));

  for (row = 0U; row < UI_PARAMS_PER_PAGE; ++row)
  {
    draw_param_row(row);
  }

  selected = &g_params[param_index(g_ui.param_selected)];
  (void)selected;
  draw_params_editor_detail();
  draw_params_stepbar();

  draw_cell(0U, PARAM_FOOTER_Y, UI_W, PARAM_FOOTER_H, UI_COLOR_BG);
  draw_text(14U, 724U, "PARAM TABLE", 1U, UI_COLOR_MUTED);
  draw_text_right(318U, 724U, 148U, "RW / EO", 1U, UI_COLOR_MUTED);
}

static void adjust_selected_param(int8_t dir)
{
  ui_param_t *p = &g_params[param_index(g_ui.param_selected)];
  if (G474_Remote_IsSaveInProgress() != 0U)
  {
    return;
  }
  float step;
  if (UI_ParamIsEditable(p) == 0U)
  {
    return;
  }
  step = (g_ui.step_index == 0U) ? param_fine_step(p) : param_coarse_step(p);
  {
    float value = p->value + (float)dir * step;
    if (value < p->min) { value = p->min; }
    if (value > p->max) { value = p->max; }
    p->value = value;
    s_dirty_param_mask |= ((uint64_t)1U << param_index(g_ui.param_selected));
  }
}

void handle_params_repeat(uint8_t target)
{
  ui_rect_t rects[3];
  uint16_t button_x = (target == 5U) ? PARAM_DEC_X : PARAM_INC_X;
  uint16_t button_w = (target == 5U) ? PARAM_DEC_W : PARAM_INC_W;

  adjust_selected_param((target == 5U) ? -1 : 1);
  draw_param_row(g_ui.param_selected);
  draw_params_editor_value();
  rects[0].x = 388U;
  rects[0].y = (uint16_t)(PARAM_ROW_Y + g_ui.param_selected * PARAM_ROW_H);
  rects[0].w = 92U;
  rects[0].h = PARAM_ROW_H;
  rects[1].x = 82U;
  rects[1].y = PARAM_EDITOR_Y;
  rects[1].w = 316U;
  rects[1].h = PARAM_EDITOR_H;
  rects[2].x = button_x;
  rects[2].y = PARAM_EDITOR_Y;
  rects[2].w = button_w;
  rects[2].h = PARAM_EDITOR_H;
  sync_dirty_rects(rects, 3U);
}

uint8_t UI_Params_IsDirty(uint8_t id)
{
  for (uint8_t i = 0U; i < UI_PARAM_COUNT; ++i)
  {
    if ((uint8_t)g_params[i].id == id)
    {
      return (s_dirty_param_mask & ((uint64_t)1U << i)) != 0U ? 1U : 0U;
    }
  }

  return 0U;
}

void UI_Params_ClearDirty(void)
{
  s_dirty_param_mask = 0U;
}

void UI_Params_NotifyRemoteUpdate(uint8_t id)
{
  for (uint8_t i = 0U; i < UI_PARAM_COUNT; ++i)
  {
    if ((uint8_t)g_params[i].id == id)
    {
      s_remote_param_update_mask |= ((uint64_t)1U << i);
      return;
    }
  }
}

void UI_Params_RefreshRemote(void)
{
  uint64_t update_mask;
  uint16_t first;
  uint8_t row;
  uint8_t count = 0U;
  ui_rect_t rects[UI_PARAMS_PER_PAGE + 1U];

  if (s_remote_param_update_mask == 0U || g_ui.page != UI_PAGE_PARAMS ||
      g_ui.touch_down != 0U)
  {
    return;
  }

  update_mask = s_remote_param_update_mask;
  s_remote_param_update_mask = 0U;
  first = (uint16_t)(g_ui.param_page * UI_PARAMS_PER_PAGE);

  for (row = 0U; row < UI_PARAMS_PER_PAGE; ++row)
  {
    uint16_t index = (uint16_t)(first + row);
    if (index < UI_PARAM_COUNT && (update_mask & ((uint64_t)1U << index)) != 0U)
    {
      draw_param_row(row);
      rects[count].x = 388U;
      rects[count].y = (uint16_t)(PARAM_ROW_Y + row * PARAM_ROW_H);
      rects[count].w = 92U;
      rects[count].h = PARAM_ROW_H;
      ++count;

      if (row == g_ui.param_selected)
      {
        draw_params_editor_value();
        rects[count].x = PARAM_DEC_W;
        rects[count].y = PARAM_EDITOR_Y;
        rects[count].w = 316U;
        rects[count].h = PARAM_EDITOR_H;
        ++count;
      }
    }
  }

  if (count != 0U)
  {
    sync_dirty_rects(rects, count);
  }
}

void handle_params_save_result(void)
{
  const uint8_t result = G474_Remote_ConsumeSaveResult();

  if (result == 0U)
  {
    return;
  }
  if (result == 1U)
  {
    s_dirty_param_mask = 0U;
    GUI_LogWrite("PARAM SAVE OK");
    GUI_PopupShow("SAVE OK", "G474 FLASH WRITTEN", 1500U);
  }
  else
  {
    GUI_LogWrite("PARAM SAVE FAILED");
    GUI_PopupShow("SAVE FAIL", "G474 SAVE RETRY", 1500U);
  }
  if (g_ui.page == UI_PAGE_PARAMS)
  {
    refresh_params_save_button();
  }
}

void handle_params_touch(uint16_t x, uint16_t y, uint8_t is_press)
{
  uint8_t slot = 0U;
  param_touch_zone_t zone;

  if (!is_press)
  {
    return;
  }

  zone = param_touch_zone(x, y, &slot);

  switch (zone)
  {
  case PARAM_TOUCH_PAGE_PREV:
    if (g_ui.param_page > 0U)
    {
      --g_ui.param_page;
      g_ui.param_selected = 0U;
      redraw_content_nav();
    }
    break;

  case PARAM_TOUCH_PAGE_NEXT:
    if (g_ui.param_page < (UI_PARAMS_PAGE_COUNT - 1U))
    {
      ++g_ui.param_page;
      g_ui.param_selected = 0U;
      redraw_content_nav();
    }
    break;

  case PARAM_TOUCH_ROW:
    if (param_at_row(slot) != 0)
    {
      uint8_t old_selected = g_ui.param_selected;
      g_ui.param_selected = slot;
      if (old_selected != g_ui.param_selected)
      {
        ui_rect_t rects[5];

        draw_param_row(old_selected);
        draw_param_row(g_ui.param_selected);
        draw_params_editor_detail();
        draw_params_stepbar();

        rects[0].x = 0U;
        rects[0].y = (uint16_t)(PARAM_ROW_Y + old_selected * PARAM_ROW_H);
        rects[0].w = UI_W;
        rects[0].h = PARAM_ROW_H;
        rects[1].x = 0U;
        rects[1].y = (uint16_t)(PARAM_ROW_Y + g_ui.param_selected * PARAM_ROW_H);
        rects[1].w = UI_W;
        rects[1].h = PARAM_ROW_H;
        rects[2].x = 0U;
        rects[2].y = PARAM_EDITOR_Y;
        rects[2].w = UI_W;
        rects[2].h = PARAM_EDITOR_H;
        rects[3].x = 0U;
        rects[3].y = PARAM_INFO_Y;
        rects[3].w = UI_W;
        rects[3].h = PARAM_INFO_H;
        rects[4].x = 0U;
        rects[4].y = PARAM_STEP_Y;
        rects[4].w = UI_W;
        rects[4].h = PARAM_STEP_H;
        sync_dirty_rects(rects, 5U);
      }
    }
    break;

  case PARAM_TOUCH_DEC:
    g_ui.hover_target = UI_HOVER_PARAM_DEC;
    draw_params_hover_button(g_ui.hover_target, 1U);
    g_ui.repeat_target = 5U;
    handle_params_repeat(g_ui.repeat_target);
    break;

  case PARAM_TOUCH_INC:
    g_ui.hover_target = UI_HOVER_PARAM_INC;
    draw_params_hover_button(g_ui.hover_target, 1U);
    g_ui.repeat_target = 6U;
    handle_params_repeat(g_ui.repeat_target);
    break;

  case PARAM_TOUCH_STEP:
    if (slot < 2U && g_ui.step_index != slot)
    {
      g_ui.step_index = slot;
      draw_params_stepbar();
      sync_dirty(0U, PARAM_STEP_Y, UI_W, PARAM_STEP_H);
    }
    break;

  case PARAM_TOUCH_SAVE:
    if (UI_Esp32ModeIsActive() != 0U)
    {
      GUI_PopupShow("PARAM LOCKED", "ESP32 SERVICE ACTIVE", 1500U);
    }
    else if (s_dirty_param_mask != 0U && G474_Remote_IsSaveInProgress() == 0U)
    {
      uint8_t ids[UI_PARAM_COUNT];
      float values[UI_PARAM_COUNT];
      uint8_t count = 0U;

      for (uint8_t i = 0U; i < UI_PARAM_COUNT; ++i)
      {
        if ((s_dirty_param_mask & ((uint64_t)1U << i)) != 0U)
        {
          ids[count] = (uint8_t)g_params[i].id;
          values[count] = g_params[i].value;
          ++count;
        }
      }
      if (G474_Remote_SaveParameters(ids, values, count) != 0U)
      {
        GUI_LogPrintf("PARAM SAVE REQUEST %u ITEMS", (unsigned)count);
        refresh_params_save_button();
        GUI_PopupShow("SAVE", "WRITE / FLASH", 1500U);
      }
      else
      {
        GUI_LogWrite("PARAM SAVE BUSY");
        GUI_PopupShow("SAVE", "G474 BUSY", 1500U);
      }
    }
    else if (G474_Remote_IsSaveInProgress() == 0U)
    {
      GUI_PopupShow("SAVE", "NO CHANGES", 1500U);
    }
    break;

  case PARAM_TOUCH_NONE:
  default:
    break;
  }
}
