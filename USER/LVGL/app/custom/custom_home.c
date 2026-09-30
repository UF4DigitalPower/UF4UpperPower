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
#include "uf4com.h"

static lv_obj_t *s_output_voltage;
static lv_obj_t *s_output_current;
static lv_obj_t *s_monitor[6];

static void set_text_if_changed(lv_obj_t *label, const char *value)
{
    if(label && strcmp(lv_label_get_text(label), value) != 0)
        lv_label_set_text(label, value);
}

static void set_float_label(lv_obj_t *label, float value, const char *unit)
{
    char buffer[24];
    if(label == NULL) return;
    snprintf(buffer, sizeof(buffer), "%.2f%s", (double)value, unit);
    set_text_if_changed(label, buffer);
}

void board_ui_refresh(void)
{
    static const uint8_t monitor_ids[4] = {
        UF4_ID_CORE_TEMPERATURE, UF4_ID_TEMP1_TEMPERATURE,
        UF4_ID_TEMP2_TEMPERATURE, UF4_ID_FAN_SPEED
    };
    float value;
    uint16_t raw;
    if(ui_transition_active()) return;
    board_ui_refresh_status();
    board_params_refresh();
    if(state.page != PAGE_HOME) return;
    set_float_label(s_output_voltage, state.output_voltage, "");
    set_float_label(s_output_current, state.output_current, "");
    for(unsigned i = 0; i < 4; ++i) {
        if(G474_Remote_ReadFloat(monitor_ids[i], &value))
            set_float_label(s_monitor[i], value, i == 3 ? " RPM" : " C");
        else set_text_if_changed(s_monitor[i], "--");
    }
    if(s_monitor[4]) {
        if(G474_Remote_Read(UF4_ID_FAULT_STATE, &raw))
            set_text_if_changed(s_monitor[4], raw == 0 ? "NONE" : "FAULT");
        else set_text_if_changed(s_monitor[4], "--");
    }
    set_text_if_changed(s_monitor[5],
        G474_Remote_IsOnline() ? (state.output_on ? "RUN" : "IDLE") : "OFFLINE");
}

static lv_obj_t *draw_big_value(int y, const char *title, const char *badge,
                           const char *unit, const char *meta_left, const char *meta_right,
                           const char *side_title, const char *side_value, bool inverted)
{
    uint16_t fill = inverted ? palette->ink : palette->panel;
    uint16_t fg = inverted ? palette->bg : palette->text;
    uint16_t sub = inverted ? palette->bg : palette->muted;
    static_cell(0, y, 480, 134, fill);
    text(screen, 16, y + 7, 340, title, &lv_font_Teko_SemiBold_16, sub, LV_TEXT_ALIGN_LEFT);
    lv_obj_t *badge_cell = cell(screen, 408, y + 8, 56, 24, palette->accent);
    lv_obj_set_style_border_width(badge_cell, 0, 0);
    centered(badge_cell, 56, 24, badge, &lv_font_Teko_SemiBold_16, palette->ink);
    lv_obj_t *reading = text(screen, 16, y + 31, 220, "--", &lv_font_Teko_SemiBold_46, fg, LV_TEXT_ALIGN_LEFT);
    text(screen, 217, y + 31, 55, unit, &lv_font_Teko_SemiBold_46, fg, LV_TEXT_ALIGN_LEFT);
    text(screen, 356, y + 35, 105, side_title, &lv_font_Teko_SemiBold_16, sub, LV_TEXT_ALIGN_LEFT);
    text(screen, 356, y + 56, 105, side_value, &lv_font_Teko_SemiBold_24, fg, LV_TEXT_ALIGN_LEFT);
    strip(1, y + 108, 478, inverted ? palette->bg : palette->border);
    text(screen, 16, y + 110, 225, meta_left, &lv_font_Teko_SemiBold_16, sub, LV_TEXT_ALIGN_LEFT);
    text(screen, 240, y + 110, 224, meta_right, &lv_font_Teko_SemiBold_16, sub, LV_TEXT_ALIGN_RIGHT);
    return reading;
}

static void draw_set_row(int y, bool voltage)
{
    char value[24];
    float setpoint = voltage ? state.voltage_set : state.current_limit;
    snprintf(value, sizeof(value), "%05.2f %s", (double)setpoint, voltage ? "V" : "A");
    button(0, y, 56, 78, "-", palette->bg, palette->muted,
           &lv_font_Teko_SemiBold_28, ACT_HOME_ADJUST, voltage ? 0 : 2);
    static_cell(56, y, 208, 78, palette->panel);
    text(screen, 59, y + 5, 202, voltage ? "VOLTAGE SET" : "CURRENT LIMIT",
         &lv_font_Teko_SemiBold_12, palette->muted, LV_TEXT_ALIGN_CENTER);
    text(screen, 59, y + 29, 202, value, &lv_font_Teko_SemiBold_28,
         palette->text, LV_TEXT_ALIGN_CENTER);
    button(264, y, 56, 78, "+", palette->bg, palette->muted,
           &lv_font_Teko_SemiBold_28, ACT_HOME_ADJUST, voltage ? 1 : 3);
}

static void draw_preset(void)
{
    char caption[24], value[24];
    preset_t *preset = &state.presets[state.preset_selected];
    snprintf(caption, sizeof(caption), "PRESET P%u", (unsigned)(state.preset_selected + 1));
    button(320, 386, 160, 40, caption, palette->accent, palette->ink,
           &lv_font_Teko_SemiBold_20, ACT_PRESET_SELECT, 0);
    static_cell(320, 426, 80, 64, palette->panel);
    static_cell(400, 426, 80, 64, palette->panel);
    text(screen, 322, 430, 76, "VSET", &lv_font_Teko_SemiBold_12, palette->muted, LV_TEXT_ALIGN_CENTER);
    text(screen, 402, 430, 76, "ISET", &lv_font_Teko_SemiBold_12, palette->muted, LV_TEXT_ALIGN_CENTER);
    snprintf(value, sizeof(value), "%.1f", (double)preset->voltage);
    text(screen, 322, 447, 76, value, &lv_font_Teko_SemiBold_24, palette->text, LV_TEXT_ALIGN_CENTER);
    snprintf(value, sizeof(value), "%.1f", (double)preset->current);
    text(screen, 402, 447, 76, value, &lv_font_Teko_SemiBold_24, palette->text, LV_TEXT_ALIGN_CENTER);
    button(320, 490, 80, 52, "SAVE", palette->bg, palette->muted,
           &lv_font_Teko_SemiBold_20, ACT_PRESET_SAVE, 0);
    button(400, 490, 80, 52, "APPLY", palette->bg, palette->muted,
           &lv_font_Teko_SemiBold_20, ACT_PRESET_APPLY, 0);
}

void draw_home(void)
{
    static const char *labels[] = {"TEMP MCU", "TEMP NTC1", "TEMP NTC2", "FAN", "FAULT", "STATE"};
    s_output_voltage = draw_big_value(118, "OUTPUT VOLTAGE", "CV", "V", "INPUT VOLTAGE",
                   "INPUT POWER", "ENERGY", "--", true);
    s_output_current = draw_big_value(252, "OUTPUT CURRENT", "LIVE", "A", "INPUT CURRENT",
                   "OUTPUT POWER", "EFF", "--", false);
    draw_set_row(386, true);
    draw_set_row(464, false);
    draw_preset();
    for(int i = 0; i < 3; i++) {
        char step[12];
        snprintf(step, sizeof(step), "%g", (double)(i == 0 ? 0.02f : (i == 1 ? 0.2f : 2.0f)));
        button(i * 160, 542, 160, 44, step,
               state.home_step == i ? palette->accent : palette->panel,
               state.home_step == i ? palette->text : palette->muted,
               &lv_font_Teko_SemiBold_16, ACT_HOME_STEP, (uint8_t)i);
    }
    for(int i = 0; i < 6; i++) {
        int x = (i % 3) * 160;
        int y = 586 + (i / 3) * 68;
        static_cell(x, y, 160, 68, palette->panel);
        text(screen, x + 14, y + 6, 132, labels[i], &lv_font_Teko_SemiBold_12,
             palette->muted, LV_TEXT_ALIGN_LEFT);
        s_monitor[i] = text(screen, x + 14, y + 26, 132, "--",
                            &lv_font_Teko_SemiBold_20, palette->text, LV_TEXT_ALIGN_LEFT);
    }
    static_cell(0, 722, 480, 26, palette->bg);
    text(screen, 14, 725, 105, "CAN 500K", &lv_font_Teko_SemiBold_12, palette->muted, LV_TEXT_ALIGN_LEFT);
    text(screen, 120, 725, 230, "MODE : CV / BUCK", &lv_font_Teko_SemiBold_12,
         palette->muted, LV_TEXT_ALIGN_CENTER);
    text(screen, 360, 725, 106, "00:00:00", &lv_font_Teko_SemiBold_12,
         palette->muted, LV_TEXT_ALIGN_RIGHT);
}
