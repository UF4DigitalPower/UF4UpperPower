#ifndef CUSTOM_INTERNAL_H
#define CUSTOM_INTERNAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "custom.h"

LV_FONT_DECLARE(lv_font_Teko_SemiBold_12);
LV_FONT_DECLARE(lv_font_Teko_SemiBold_16);
LV_FONT_DECLARE(lv_font_Teko_SemiBold_20);
LV_FONT_DECLARE(lv_font_Teko_SemiBold_24);
LV_FONT_DECLARE(lv_font_Teko_SemiBold_28);
LV_FONT_DECLARE(lv_font_Teko_SemiBold_36);
LV_FONT_DECLARE(lv_font_Teko_SemiBold_40);
LV_FONT_DECLARE(lv_font_Teko_SemiBold_46);
#define UI_W 480
#define UI_H 800
#define PARAM_COUNT 25
#define PARAMS_PER_PAGE 6
#define SETTING_COUNT 6
#define LOG_CAPACITY 22

typedef enum { PAGE_HOME, PAGE_PARAMS, PAGE_SETTINGS, PAGE_LOG } page_t;
typedef enum {
    ACT_OUTPUT = 1, ACT_NAV, ACT_HOME_ADJUST, ACT_HOME_STEP, ACT_PRESET_SELECT,
    ACT_PRESET_SAVE, ACT_PRESET_APPLY, ACT_PARAM_PAGE, ACT_PARAM_SELECT,
    ACT_PARAM_ADJUST, ACT_PARAM_STEP, ACT_PARAM_SAVE, ACT_SETTING_SELECT,
    ACT_SETTING_NEXT, ACT_BENCHMARK
} action_t;

typedef struct {
    uint16_t bg, ink, accent, panel, muted, disabled, border, text;
} palette_t;

typedef struct {
    uint8_t id;
    const char *name;
    float value, min, max;
    const char *unit;
    uint8_t digits;
} param_t;

typedef struct {
    const char *group;
    const char *name;
    const char *description;
    const char *const *options;
    uint8_t option_count;
    uint8_t current;
} setting_t;

typedef struct { float voltage, current; } preset_t;

typedef struct {
    page_t page;
    bool output_on;
    float output_voltage;
    float output_current;
    float voltage_set;
    float current_limit;
    uint8_t home_step;
    preset_t presets[4];
    uint8_t preset_selected;
    uint8_t param_page;
    uint8_t param_selected;
    uint8_t param_step;
    uint8_t setting_selected;
} ui_state_t;


extern const palette_t palettes[];
extern param_t params[PARAM_COUNT];
extern setting_t settings[SETTING_COUNT];
extern ui_state_t state;
extern const palette_t *palette;
extern lv_obj_t *screen;
extern lv_obj_t *nav_indicator;
extern lv_obj_t *nav_labels[4];
extern char log_lines[LOG_CAPACITY][52];
extern uint8_t log_count;

lv_color_t rgb(uint16_t value);
void log_event(const char *message);
lv_obj_t *cell(lv_obj_t *parent, int x, int y, int w, int h, uint16_t fill);
lv_obj_t *text(lv_obj_t *parent, int x, int y, int w, const char *value,
               const lv_font_t *font, uint16_t fg, lv_text_align_t align);
void centered(lv_obj_t *parent, int w, int h, const char *value,
              const lv_font_t *font, uint16_t fg);
lv_obj_t *button(int x, int y, int w, int h, const char *value,
                 uint16_t fill, uint16_t fg, const lv_font_t *font,
                 action_t action, uint8_t argument);
void static_cell(int x, int y, int w, int h, uint16_t fill);
void strip(int x, int y, int w, uint16_t fill);
void format_value(char *buffer, size_t size, float value, uint8_t digits);
void draw_header(void);
void draw_status(void);
void draw_nav(void);
void draw_home(void);
float param_step(const param_t *param, bool coarse);
void draw_params(void);
void draw_settings(void);
void draw_log(void);
void ui_render_init(lv_obj_t *canvas);
void ui_boot_start(lv_obj_t *canvas);
void ui_request_render(void);
void ui_transition_to(page_t page);
bool ui_transition_active(void);
void clicked_action(uintptr_t data);
void clicked(lv_event_t *event);
void board_ui_refresh(void);
void board_ui_refresh_status(void);
void board_params_refresh(void);

#endif
