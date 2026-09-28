/**
  ******************************************************************************
  * @file    gui_settings.c
  * @author  UF4
  * @date    26-8-14
  * @brief   Settings page drawing and touch handling.
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
#include "parameter_manager.h"
#include "g474_remote.h"

#define SETTING_PAGE_BUTTON_Y       118U
#define SETTING_PAGE_BUTTON_W       72U
#define SETTING_PAGE_BUTTON_H       44U
#define SETTING_PAGE_PREV_X         0U
#define SETTING_ROW_X               0U
#define SETTING_ROW_Y               162U
#define SETTING_ROW_W               480U
#define SETTING_ROW_H               66U
#define SETTING_DETAIL_Y            624U
#define SETTING_DETAIL_H            88U
#define SETTING_ACTION_X            378U
#define SETTING_ACTION_W            102U
#define SETTING_FOOTER_Y            712U
#define SETTING_FOOTER_H            36U

typedef enum
{
  SETTING_TOUCH_NONE = 0,
  SETTING_TOUCH_PAGE_PREV,
  SETTING_TOUCH_PAGE_NEXT,
  SETTING_TOUCH_ROW,
  SETTING_TOUCH_ACTION
} setting_touch_zone_t;

static uint16_t setting_index(uint8_t row)
{
  return (uint16_t)(g_ui.setting_page * UI_SETTINGS_PER_PAGE + row);
}

static ui_setting_t *setting_at_row(uint8_t row)
{
  uint16_t index = setting_index(row);

  if (index >= UI_SETTING_COUNT)
  {
    return 0;
  }

  return &g_settings[index];
}

static ui_setting_t *selected_setting(void)
{
  return setting_at_row(g_ui.setting_selected);
}

static setting_touch_zone_t setting_touch_zone(uint16_t x, uint16_t y, uint8_t *slot)
{
  if (slot != 0)
  {
    *slot = 0U;
  }

  if (pt_in(x, y, SETTING_PAGE_PREV_X, SETTING_PAGE_BUTTON_Y, SETTING_PAGE_BUTTON_W, SETTING_PAGE_BUTTON_H))
  {
    return SETTING_TOUCH_PAGE_PREV;
  }
  if (pt_in(x, y, SETTING_ROW_X, SETTING_ROW_Y, SETTING_ROW_W, UI_SETTINGS_PER_PAGE * SETTING_ROW_H))
  {
    if (slot != 0)
    {
      *slot = (uint8_t)((y - SETTING_ROW_Y) / SETTING_ROW_H);
    }
    return SETTING_TOUCH_ROW;
  }
  if (pt_in(x, y, SETTING_ACTION_X, SETTING_DETAIL_Y, SETTING_ACTION_W, SETTING_DETAIL_H))
  {
    return SETTING_TOUCH_ACTION;
  }

  return SETTING_TOUCH_NONE;
}

static void draw_setting_row(uint8_t row)
{
  uint16_t index = setting_index(row);
  uint16_t y = (uint16_t)(SETTING_ROW_Y + row * SETTING_ROW_H);
  uint8_t selected_row = (uint8_t)(row == g_ui.setting_selected);
  char buf[48];
  uint16_t row_bg = selected_row ? UI_COLOR_INK : UI_COLOR_PANEL;

  if (index >= UI_SETTING_COUNT)
  {
    draw_cell(0U, y, UI_W, SETTING_ROW_H, UI_COLOR_BG);
    return;
  }

  ui_setting_t *s = &g_settings[index];
  uint8_t editable = UI_SettingIsEditable(s);

  draw_cell(0U, y, 54U, SETTING_ROW_H, selected_row ? UI_COLOR_ACCENT : UI_COLOR_PANEL);
  (void)snprintf(buf, sizeof(buf), "S%03u", (unsigned)(index + 1U));
  draw_text_center(0U, y, 54U, SETTING_ROW_H, buf, 1U, selected_row ? UI_COLOR_INK : UI_COLOR_MUTED);
  draw_cell(54U, y, 324U, SETTING_ROW_H, row_bg);
  (void)snprintf(buf, sizeof(buf), "%s/%s", s->group, editable ? "RW" : "EO");
  draw_text(68U, (uint16_t)(y + 11U), buf, 1U, selected_row ? UI_COLOR_ACCENT : UI_COLOR_MUTED);
  draw_text(68U, (uint16_t)(y + 36U), s->name, 1U, selected_row ? UI_COLOR_BG : UI_COLOR_TEXT);
  draw_cell(SETTING_ACTION_X, y, SETTING_ACTION_W, SETTING_ROW_H, editable ? UI_COLOR_ACCENT : UI_COLOR_BG);
  draw_text_center(SETTING_ACTION_X, y, SETTING_ACTION_W, SETTING_ROW_H, s->options[s->current], 1U, editable ? UI_COLOR_INK : UI_COLOR_MUTED);
}

static void draw_setting_detail(void)
{
  ui_setting_t *selected = selected_setting();
  char buf[80];

  if (selected == 0)
  {
    draw_cell(0U, SETTING_DETAIL_Y, 480U, SETTING_DETAIL_H + SETTING_FOOTER_H, UI_COLOR_BG);
    return;
  }

  draw_cell(0U, SETTING_DETAIL_Y, 378U, SETTING_DETAIL_H, UI_COLOR_PANEL);
  (void)snprintf(buf, sizeof(buf), "%s/%s", selected->group, UI_SettingIsEditable(selected) ? "RW" : "EO");
  draw_text(16U, 644U, buf, 1U, UI_COLOR_MUTED);
  draw_text(16U, 674U, selected->description, 1U, UI_COLOR_TEXT);
  draw_button(SETTING_ACTION_X, SETTING_DETAIL_Y, SETTING_ACTION_W, SETTING_DETAIL_H,
              (selected->id == UI_SETTING_ID_POPUP_TEST) ? "TEST" :
              (UI_SettingIsEditable(selected) ? "NEXT" : "VIEW"),
              0U, UI_SettingIsEditable(selected) ? 0U : 1U);

  draw_cell(0U, SETTING_FOOTER_Y, 480U, SETTING_FOOTER_H, UI_COLOR_BG);
  draw_text(14U, 724U, "RW: EDITABLE", 1U, UI_COLOR_MUTED);
  draw_text_center(0U, 724U, 480U, 18U, "EO: DISPLAY ONLY", 1U, UI_COLOR_MUTED);
  (void)snprintf(buf, sizeof(buf), "%s: %s", selected->name, selected->options[selected->current]);
  draw_text_right(276U, 724U, 190U, buf, 1U, UI_COLOR_MUTED);
}

void handle_settings_action_hover(uint8_t active)
{
  ui_setting_t *selected = selected_setting();

  if (UI_SettingIsEditable(selected) == 0U)
  {
    return;
  }

  draw_button(SETTING_ACTION_X, SETTING_DETAIL_Y, SETTING_ACTION_W, SETTING_DETAIL_H,
              (selected->id == UI_SETTING_ID_POPUP_TEST) ? "TEST" : "NEXT", active, 0U);
  sync_dirty(SETTING_ACTION_X, SETTING_DETAIL_Y, SETTING_ACTION_W, SETTING_DETAIL_H);
}

static void draw_setting_footer_value(void)
{
  ui_setting_t *selected = selected_setting();
  char buf[80];

  if (selected == 0)
  {
    return;
  }

  draw_cell(276U, SETTING_FOOTER_Y, 204U, SETTING_FOOTER_H, UI_COLOR_BG);
  (void)snprintf(buf, sizeof(buf), "%s: %s", selected->name, selected->options[selected->current]);
  draw_text_right(276U, 724U, 190U, buf, 1U, UI_COLOR_MUTED);
}

void draw_settings_page(void)
{
  uint16_t start = (uint16_t)(g_ui.setting_page * UI_SETTINGS_PER_PAGE);
  uint16_t end = (uint16_t)(start + UI_SETTINGS_PER_PAGE);
  uint8_t row;
  char buf[48];
  ui_setting_t *selected;

  if (end > UI_SETTING_COUNT)
  {
    end = UI_SETTING_COUNT;
  }

  draw_button(0U, 118U, 72U, 44U, "PREV", 1U, (uint8_t)(g_ui.setting_page == 0U));
  (void)snprintf(buf, sizeof(buf), "SETTING %02u-%02u / %u",
                 (unsigned)(start + 1U), (unsigned)end, (unsigned)UI_SETTING_COUNT);
  draw_cell(72U, 118U, 408U, 44U, UI_COLOR_PANEL);
  draw_text_center(72U, 118U, 408U, 44U, buf, 1U, UI_COLOR_TEXT);

  for (row = 0U; row < UI_SETTINGS_PER_PAGE; ++row)
  {
    draw_setting_row(row);
  }

  selected = &g_settings[setting_index(g_ui.setting_selected)];
  (void)selected;
  draw_setting_detail();
}

void handle_settings_touch(uint16_t x, uint16_t y, uint8_t is_press)
{
  ui_setting_t *s;
  uint8_t slot = 0U;
  setting_touch_zone_t zone;

  if (!is_press)
  {
    return;
  }

  zone = setting_touch_zone(x, y, &slot);

  switch (zone)
  {
  case SETTING_TOUCH_PAGE_PREV:
    if (g_ui.setting_page > 0U)
    {
      --g_ui.setting_page;
      g_ui.setting_selected = 0U;
      redraw_content_nav();
    }
    break;

  case SETTING_TOUCH_PAGE_NEXT:
    if (g_ui.setting_page < (UI_SETTINGS_PAGE_COUNT - 1U))
    {
      ++g_ui.setting_page;
      g_ui.setting_selected = 0U;
      redraw_content_nav();
    }
    break;

  case SETTING_TOUCH_ROW:
    if (setting_at_row(slot) != 0)
    {
      uint8_t old_selected = g_ui.setting_selected;
      g_ui.setting_selected = slot;
      if (old_selected != g_ui.setting_selected)
      {
        ui_rect_t rects[4];

        draw_setting_row(old_selected);
        draw_setting_row(g_ui.setting_selected);
        draw_setting_detail();

        rects[0].x = 0U;
        rects[0].y = (uint16_t)(SETTING_ROW_Y + old_selected * SETTING_ROW_H);
        rects[0].w = UI_W;
        rects[0].h = SETTING_ROW_H;
        rects[1].x = 0U;
        rects[1].y = (uint16_t)(SETTING_ROW_Y + g_ui.setting_selected * SETTING_ROW_H);
        rects[1].w = UI_W;
        rects[1].h = SETTING_ROW_H;
        rects[2].x = 0U;
        rects[2].y = SETTING_DETAIL_Y;
        rects[2].w = UI_W;
        rects[2].h = SETTING_DETAIL_H;
        rects[3].x = 0U;
        rects[3].y = SETTING_FOOTER_Y;
        rects[3].w = UI_W;
        rects[3].h = SETTING_FOOTER_H;
        sync_dirty_rects(rects, 4U);
      }
    }
    break;

  case SETTING_TOUCH_ACTION:
    s = selected_setting();
    if (s != 0 && UI_SettingIsEditable(s) != 0U && s->option_count > 0U)
    {
      uint8_t next;
      uint8_t ok = 1U;
      ui_rect_t rects[2];
      g_ui.hover_target = UI_HOVER_SETTING_ACTION;
      handle_settings_action_hover(1U);
      next = (uint8_t)((s->current + 1U) % s->option_count);
      ok = GUI_SettingSet(s->id, next);
      GUI_LogPrintf("SETTING %s = %s", s->name, s->options[next]);
      if (s->id == 29U)
      {
        /* The color scheme is used by every cached page. Rebuild all caches
           before presenting the current page, otherwise the next navigation
           restores stale colors from the old cache. */
        GUI_PopupHide();
        gui_render_all_page_caches();
        gui_present_cached_page(g_ui.page);
        g_ui.hover_target = UI_HOVER_NONE;
        break;
      }
      if (s->id == UI_SETTING_ID_ESP32_START_MODE)
      {
        redraw_content_nav();
        break;
      }
      draw_setting_row(g_ui.setting_selected);
      draw_setting_footer_value();
      rects[0].x = SETTING_ACTION_X;
      rects[0].y = (uint16_t)(SETTING_ROW_Y + g_ui.setting_selected * SETTING_ROW_H);
      rects[0].w = SETTING_ACTION_W;
      rects[0].h = SETTING_ROW_H;
      rects[1].x = 276U;
      rects[1].y = SETTING_FOOTER_Y;
      rects[1].w = 204U;
      rects[1].h = SETTING_FOOTER_H;
      sync_dirty_rects(rects, 2U);
      if (s->id == 23U || s->id == 29U)
      {
        char msg[48];
        if (ok != 0U && UF4_ParameterManager_IsReady() != 0U)
        {
          (void)snprintf(msg, sizeof(msg), "W25Q256 write queued");
        }
        else
        {
          (void)snprintf(msg, sizeof(msg), "ERR %08lX RDY %lu",
                         (unsigned long)ui_color_preset_flash_error(),
                         (unsigned long)UF4_ParameterManager_IsReady());
        }
        GUI_PopupShow((ok != 0U && UF4_ParameterManager_IsReady() != 0U) ? "SAVE" : "SAVE FAIL",
                      msg, 3000U);
      }
      else if (s->id == UI_SETTING_ID_POPUP_TEST)
      {
        GUI_PopupShow("POPUP TEST", "NOTIFICATION ACTIVE", 1500U);
      }
    }
    break;

  case SETTING_TOUCH_NONE:
  default:
    break;
  }
}
