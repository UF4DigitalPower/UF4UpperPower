/*
 * Copyright 2026 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "custom_internal.h"
#include "g474_remote.h"

static lv_obj_t *s_output_label;
static lv_obj_t *s_runtime_label;
static lv_obj_t *s_runtime_cell;
static lv_obj_t *s_status_labels[5];

static void status_text(lv_obj_t *label, const char *value)
{
    if(label && strcmp(lv_label_get_text(label), value) != 0)
        lv_label_set_text(label, value);
}

#define BUTTON_ACTION_DELAY_MS 16U

static void button_action_timer_cb(lv_timer_t *timer)
{
    uintptr_t data = (uintptr_t)lv_timer_get_user_data(timer);
    lv_timer_delete(timer);
    clicked_action(data);
}

static void button_pressed(lv_event_t *event)
{
    uintptr_t data = (uintptr_t)lv_event_get_user_data(event);
    lv_timer_t *timer = lv_timer_create(button_action_timer_cb,
                                        BUTTON_ACTION_DELAY_MS,
                                        (void *)data);

    if(timer != NULL) {
        lv_timer_set_repeat_count(timer, 1);
    }
    else {
        /* Keep the command functional if the small deferred timer allocation
         * cannot be satisfied. */
        clicked_action(data);
    }
}

void board_ui_refresh_status(void)
{
    uint16_t raw;
    const uint8_t online = G474_Remote_IsOnline();
    uint8_t running = 0U;
    const char *output = G474_Remote_GetOutputCommanded() ? "ON" : "OFF";
    const char *runtime = "OFFLINE";
    const char *cc_cv = "--";
    const char *converter = "--";
    const char *direction = "--";
    const char *fault = "NORMAL";
    if(online && G474_Remote_Read(UF4_ID_POWER_STATE, &raw)) {
        switch(raw) {
        case 0U: runtime = "CHECK"; break;
        case 1U: runtime = "IDLE"; break;
        case 2U: runtime = "START"; break;
        case 3U: runtime = "RUN"; running = 1U; break;
        case 4U: runtime = "STOP"; break;
        case 5U: runtime = "SWITCH"; break;
        case 6U: runtime = "FAULT"; break;
        default: runtime = "UNKNOWN"; break;
        }
    }
    if(online && G474_Remote_Read(UF4_ID_CC_CV_MODE, &raw)) cc_cv = raw ? "CC" : "CV";
    if(online && G474_Remote_Read(UF4_ID_POWER_CONVERTER_MODE, &raw)) {
        converter = raw == 0U ? "BUCK" : (raw == 1U ? "MIX" : "BOOST");
    }
    if(online && G474_Remote_Read(UF4_ID_POWER_DIRECTION_STATUS, &raw))
        direction = raw ? "REVERSE" : "FORWARD";
    if(online && G474_Remote_Read(UF4_ID_FAULT_STATE, &raw) && raw != 0U) {
        switch(raw) {
        case 1U: fault = "SELF CHK"; break;
        case 2U: fault = "VOUT OVP"; break;
        case 3U: fault = "OCP"; break;
        case 4U: fault = "ADC"; break;
        case 5U: fault = "PWM"; break;
        case 6U: fault = "COMM"; break;
        case 7U: fault = "EXT"; break;
        case 8U: fault = "OTP"; break;
        default: fault = "FAULT"; break;
        }
    }
    status_text(s_status_labels[1], online ? cc_cv : "--");
    status_text(s_status_labels[2], online ? converter : "--");
    status_text(s_status_labels[3], online ? direction : "--");
    status_text(s_status_labels[4], online ? fault : "NO LINK");
    if(s_output_label && strcmp(lv_label_get_text(s_output_label), output) != 0)
        lv_label_set_text(s_output_label, output);
    if(s_runtime_label && strcmp(lv_label_get_text(s_runtime_label), runtime) != 0) {
        lv_label_set_text(s_runtime_label, runtime);
        lv_obj_set_style_text_color(s_runtime_label,
                                    rgb(running ? palette->ink : palette->muted), 0);
        lv_obj_set_style_bg_color(s_runtime_cell,
                                  rgb(running ? palette->accent : palette->panel), 0);
    }
}

lv_color_t rgb(uint16_t value)
{
    return lv_color_make((uint8_t)(((value >> 11) & 31U) * 255U / 31U),
                         (uint8_t)(((value >> 5) & 63U) * 255U / 63U),
                         (uint8_t)((value & 31U) * 255U / 31U));
}

lv_obj_t *cell(lv_obj_t *parent, int x, int y, int w, int h, uint16_t fill)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, rgb(fill), 0);
    lv_obj_set_style_border_width(obj, 1, 0);
    lv_obj_set_style_border_color(obj, rgb(palette->border), 0);
    lv_obj_set_style_text_font(obj, &lv_font_Teko_SemiBold_16, 0);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    return obj;
}

lv_obj_t *text(lv_obj_t *parent, int x, int y, int w, const char *value,
                      const lv_font_t *font, uint16_t fg, lv_text_align_t align)
{
    lv_obj_t *obj = lv_label_create(parent);
    lv_obj_remove_style_all(obj);
    lv_label_set_text(obj, value);
    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, font->line_height);
    lv_obj_set_style_text_font(obj, font, 0);
    lv_obj_set_style_text_color(obj, rgb(fg), 0);
    lv_obj_set_style_text_align(obj, align, 0);
    return obj;
}

void centered(lv_obj_t *parent, int w, int h, const char *value,
                     const lv_font_t *font, uint16_t fg)
{
    text(parent, 2, (h - font->line_height) / 2, w - 4, value, font, fg, LV_TEXT_ALIGN_CENTER);
}

lv_obj_t *button(int x, int y, int w, int h, const char *value,
                        uint16_t fill, uint16_t fg, const lv_font_t *font,
                        action_t action, uint8_t argument)
{
    lv_obj_t *obj = cell(screen, x, y, w, h, fill);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(obj, rgb(palette->ink), LV_STATE_PRESSED);
    centered(obj, w, h, value, font, fg);
    /* Let LVGL present the pressed style for one frame before the action can
     * rebuild or replace the pressed object. */
    lv_obj_add_event_cb(obj, button_pressed, LV_EVENT_PRESSED,
                        (void *)(uintptr_t)(((uint16_t)action << 8) | argument));
    return obj;
}

void static_cell(int x, int y, int w, int h, uint16_t fill)
{
    (void)cell(screen, x, y, w, h, fill);
}

void strip(int x, int y, int w, uint16_t fill)
{
    lv_obj_t *obj = cell(screen, x, y, w, 1, fill);
    lv_obj_set_style_border_width(obj, 0, 0);
}

void format_value(char *buffer, size_t size, float value, uint8_t digits)
{
    switch(digits) {
    case 0: snprintf(buffer, size, "%.0f", (double)value); break;
    case 2: snprintf(buffer, size, "%.2f", (double)value); break;
    case 4: snprintf(buffer, size, "%.4f", (double)value); break;
    default: snprintf(buffer, size, "%.1f", (double)value); break;
    }
}

void draw_header(void)
{
    static const char *small[] = {"UF4 DIGITAL POWER", "POWER PARAMETERS", "SYSTEM SETTINGS", "RUNTIME MONITOR"};
    static const char *large[] = {"DC POWER SUPPLY", "POWER PARAMETERS", "DEVICE CONFIG", "EVENT LOG"};
    const uint8_t output_commanded = G474_Remote_GetOutputCommanded();
    static_cell(0, 0, UI_W, 76, palette->bg);
    text(screen, 16, 10, 330, small[state.page], &lv_font_Teko_SemiBold_20,
         palette->muted, LV_TEXT_ALIGN_LEFT);
    text(screen, 16, 35, 330, large[state.page], &lv_font_Teko_SemiBold_28,
         palette->text, LV_TEXT_ALIGN_LEFT);
    lv_obj_t *output_button = button(362, 0, 118, 76, output_commanded ? "ON" : "OFF",
           output_commanded ? palette->ink : palette->disabled,
           output_commanded ? palette->bg : palette->muted,
           &lv_font_Teko_SemiBold_24, ACT_OUTPUT, 0);
    s_output_label = lv_obj_get_child(output_button, 0);
}

void draw_status(void)
{
    const char *items[5] = {state.output_on ? "RUN" : "IDLE", "CV", "BUCK", "FORWARD", "NO LINK"};
    for(int i = 0; i < 5; i++) {
        bool highlighted = i == 0 && state.output_on && G474_Remote_IsOnline();
        lv_obj_t *status_cell = cell(screen, i * 96, 76, 96, 42,
                                     highlighted ? palette->accent : palette->panel);
        lv_obj_t *label = text(screen, i * 96 + 2, 85, 92, items[i], &lv_font_Teko_SemiBold_16,
                               highlighted ? palette->ink : palette->muted, LV_TEXT_ALIGN_CENTER);
        if(i == 0) {
            s_runtime_cell = status_cell;
            s_runtime_label = label;
        }
        s_status_labels[i] = label;
    }
    board_ui_refresh_status();
}

void draw_nav(void)
{
    static const char *names[] = {"HOME", "PARAMS", "SETTINGS", "LOG"};
    for(int i = 0; i < 4; i++) {
        button(i * 120, 748, 120, 52, "", palette->bg, palette->muted,
               &lv_font_Teko_SemiBold_24, ACT_NAV, (uint8_t)i);
    }
    nav_indicator = cell(screen, state.page * 120, 748, 120, 52, palette->ink);
    lv_obj_set_style_border_width(nav_indicator, 0, 0);
    for(int i = 0; i < 4; i++) {
        nav_labels[i] = text(screen, i * 120 + 2, 748 + (52 - lv_font_Teko_SemiBold_24.line_height) / 2,
                             116, names[i], &lv_font_Teko_SemiBold_24,
                             state.page == i ? palette->bg : palette->muted, LV_TEXT_ALIGN_CENTER);
    }
}
