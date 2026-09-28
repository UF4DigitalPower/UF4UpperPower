/**
  ******************************************************************************
  * @file    gui_internal.h
  * @author  UF4
  * @date    26-8-14
  * @brief   Internal fixed-pixel UI definitions for UF4 digital power supply.
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
#ifndef GUI_INTERNAL_H
#define GUI_INTERNAL_H

#include "gui.h"
#include "bsp_lcd.h"
#include "Font/Teko_SemiBold_Index.h"
#include "ft5x16.h"
#include "gui_log.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define UI_W                         480U
#define UI_H                         800U
#define UI_HEADER_H                  76U
#define UI_STATUS_Y                  76U
#define UI_STATUS_H                  42U
#define UI_CONTENT_Y                 118U
#define UI_CONTENT_H                 630U
#define UI_NAV_Y                     748U
#define UI_NAV_H                     52U
#define UI_BORDER                    1U

#define UI_COLOR_BG_DEFAULT          0xF7BDU   /* page background: #f6f6f2                     */
#define UI_COLOR_INK_DEFAULT         0x1082U   /* ink / active fill: #101010                   */
#define UI_COLOR_ACCENT_DEFAULT      0xD7E8U   /* accent: #d9ff3f (lime)                       */
#define UI_COLOR_PANEL_DEFAULT       0xFFFFU   /* card / panel fill: #ffffff                   */
#define UI_COLOR_MUTED_DEFAULT       0x6B4CU   /* muted text: #686861                          */
#define UI_COLOR_DISABLED_DEFAULT    0xE73BU   /* disabled / off fill: #e7e7df                  */
#define UI_COLOR_BORDER_DEFAULT      0x1082U
#define UI_COLOR_TEXT_DEFAULT        0x1082U

#define UI_COLOR_BG                  g_ui_color_bg
#define UI_COLOR_INK                 g_ui_color_ink
#define UI_COLOR_ACCENT              g_ui_color_accent
#define UI_COLOR_WHITE               g_ui_color_panel
#define UI_COLOR_MUTED               g_ui_color_muted
#define UI_COLOR_DISABLED            g_ui_color_disabled
#define UI_COLOR_BORDER              g_ui_color_border

#define UI_COLOR_PANEL               g_ui_color_panel
#define UI_COLOR_PANEL_DARK          g_ui_color_disabled
#define UI_COLOR_TEXT                g_ui_color_text

#define UI_PARAM_COUNT               31U
#define UI_PARAMS_PER_PAGE           6U
#define UI_PARAMS_PAGE_COUNT         ((UI_PARAM_COUNT + UI_PARAMS_PER_PAGE - 1U) / UI_PARAMS_PER_PAGE)
#define UI_SETTING_COUNT             6U
#define UI_SETTINGS_PER_PAGE         6U
#define UI_SETTINGS_PAGE_COUNT       ((UI_SETTING_COUNT + UI_SETTINGS_PER_PAGE - 1U) / UI_SETTINGS_PER_PAGE)
#define UI_COLOR_PRESET_COUNT        8U
#define UI_HOME_PRESET_COUNT         4U

#define UI_HOVER_NONE                0U
#define UI_HOVER_HOME_VDEC           10U
#define UI_HOVER_HOME_VINC           11U
#define UI_HOVER_HOME_IDEC           12U
#define UI_HOVER_HOME_IINC           13U
#define UI_HOVER_PARAM_DEC           1U
#define UI_HOVER_PARAM_INC           2U
#define UI_HOVER_SETTING_ACTION      3U
#define UI_HOVER_HOME_PRESET_SELECT   20U
#define UI_HOVER_HOME_PRESET_SAVE     21U
#define UI_HOVER_HOME_PRESET_APPLY    22U

#define ACCESS_RW                    0U
#define ACCESS_EO                    1U

#define UI_SETTING_ID_ESP32_START_MODE 31U
#define UI_SETTING_ID_POPUP_TEST        30U
#define UI_ESP32_START_NONE           0U
#define UI_ESP32_START_WIFI           1U
#define UI_ESP32_START_BLE            2U

typedef enum
{
  UI_PAGE_HOME = 0,
  UI_PAGE_PARAMS,
  UI_PAGE_SETTINGS,
  UI_PAGE_LOG
} ui_page_t;

typedef struct
{
  uint16_t id;
  const char *name;
  uint8_t access;
  float value;
  float min;
  float max;
  const char *unit;
  uint8_t digits;
} ui_param_t;

typedef struct
{
  uint16_t id;
  const char *group;
  const char *name;
  uint8_t access;
  uint8_t current;
  const char * const *options;
  uint8_t option_count;
  const char *description;
} ui_setting_t;

typedef struct
{
  float voltage;
  float current;
} ui_home_preset_t;

typedef struct
{
  ui_page_t page;
  uint8_t output_on;
  uint8_t step_index;
  float voltage_set;
  float current_limit;
  float output_voltage;
  float output_current;
  uint8_t param_page;
  uint8_t param_selected;
  uint8_t setting_page;
  uint8_t setting_selected;
  uint8_t touch_down;
  uint8_t repeat_target;
  uint8_t hover_target;
  uint16_t touch_x;
  uint16_t touch_y;
  uint32_t down_tick;
  uint32_t last_repeat_tick;
  uint8_t heartbeat_led;
} ui_state_t;

typedef struct
{
  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;
} ui_rect_t;

extern ui_state_t g_ui;
extern ui_param_t g_params[UI_PARAM_COUNT];
extern ui_setting_t g_settings[UI_SETTING_COUNT];
extern const float g_steps[3];
extern uint16_t g_ui_color_bg;
extern uint16_t g_ui_color_ink;
extern uint16_t g_ui_color_accent;
extern uint16_t g_ui_color_panel;
extern uint16_t g_ui_color_muted;
extern uint16_t g_ui_color_disabled;
extern uint16_t g_ui_color_border;
extern uint16_t g_ui_color_text;
void ui_apply_color_preset(uint8_t preset);
uint8_t ui_set_color_preset(uint8_t preset, uint8_t persist);
uint8_t ui_load_color_preset(void);
void ui_home_export_state(uint8_t *step_index,
                          ui_home_preset_t *presets,
                          uint8_t *selected);
void ui_home_import_state(uint8_t step_index,
                          const ui_home_preset_t *presets,
                          uint8_t selected);
uint32_t ui_color_preset_flash_error(void);
uint32_t ui_color_preset_flash_sector_error(void);
uint8_t UI_Esp32ModeIsActive(void);
uint8_t UI_ParamIsEditable(const ui_param_t *param);
uint8_t UI_SettingIsEditable(const ui_setting_t *setting);
void ui_popup_tick(uint32_t now);
uint8_t ui_popup_is_active(void);
void gui_draw_init(void);
void gui_draw_current_frame(void);
void gui_draw_full(void);
void gui_render_all_page_caches(void);
void gui_present_cached_page(ui_page_t page);
uint8_t gui_page_transition_active(void);
uint8_t gui_page_transition_tick(void);

uint8_t pt_in(uint16_t x, uint16_t y, uint16_t rx, uint16_t ry, uint16_t rw, uint16_t rh);
void fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void border(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void format_float(char *buf, uint32_t len, float value, uint8_t digits);
void format_param_value(char *buf, uint32_t len, const ui_param_t *param);
void draw_text(uint16_t x, uint16_t y, const char *s, uint8_t style, uint16_t fg);
void draw_text_center(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char *s, uint8_t style, uint16_t fg);
void draw_text_right(uint16_t x, uint16_t y, uint16_t w, const char *s, uint8_t style, uint16_t fg);
void draw_cell(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t fill_color);
void draw_button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char *label, uint8_t active, uint8_t disabled);
void draw_big_value(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char *title, const char *badge, const char *value, const char *unit, const char *meta_l, const char *meta_r, uint8_t inverted, const char *side_label, const char *side_value);
void sync_dirty(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void sync_dirty_rects(const ui_rect_t *rects, uint8_t count);
void sync_dirty_union(uint16_t x1, uint16_t y1, uint16_t w1, uint16_t h1,
                      uint16_t x2, uint16_t y2, uint16_t w2, uint16_t h2);
void sync_dirty_union3(uint16_t x1, uint16_t y1, uint16_t w1, uint16_t h1,
                       uint16_t x2, uint16_t y2, uint16_t w2, uint16_t h2,
                       uint16_t x3, uint16_t y3, uint16_t w3, uint16_t h3);
void redraw_content_nav(void);
void redraw_header_content_nav(void);
void draw_header(void);
void draw_header_output(void);
void draw_status_bar(void);
void draw_nav(void);
void draw_content(void);
void draw_home_page(void);
void draw_home_telemetry(void);
void draw_home_live_content(void);
void draw_params_page(void);
void draw_settings_page(void);
void draw_log_page(void);
uint8_t handle_nav_touch(uint16_t x, uint16_t y);
void handle_home_touch(uint16_t x, uint16_t y, uint8_t is_press);
void handle_params_touch(uint16_t x, uint16_t y, uint8_t is_press);
void handle_settings_touch(uint16_t x, uint16_t y, uint8_t is_press);
void handle_home_repeat(uint8_t target);
void handle_home_button_hover(uint8_t target, uint8_t active);
void handle_home_preset_hover(uint8_t target, uint8_t active);
void handle_params_repeat(uint8_t target);
void handle_params_button_hover(uint8_t target, uint8_t active);
void handle_params_save_result(void);
uint8_t UI_Params_IsDirty(uint8_t id);
void UI_Params_ClearDirty(void);
void UI_Params_NotifyRemoteUpdate(uint8_t id);
void UI_Params_RefreshRemote(void);
void handle_settings_action_hover(uint8_t active);

#endif /* GUI_INTERNAL_H */
