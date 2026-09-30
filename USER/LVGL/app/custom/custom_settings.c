/*
 * Copyright 2026 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "custom_internal.h"

void draw_settings(void)
{
    char buffer[96];
    lv_obj_t *previous = cell(screen, 0, 118, 72, 44, palette->disabled);
    centered(previous, 72, 44, "PREV", &lv_font_Teko_SemiBold_20, palette->muted);
    static_cell(72, 118, 408, 44, palette->panel);
    text(screen, 74, 126, 404, "SETTING 01-06 / 6", &lv_font_Teko_SemiBold_16,
         palette->text, LV_TEXT_ALIGN_CENTER);
    for(int i = 0; i < SETTING_COUNT; i++) {
        setting_t *setting = &settings[i];
        int y = 162 + i * 66;
        bool selected = state.setting_selected == i;
        uint16_t row_fill = selected ? palette->ink : palette->panel;
        button(0, y, 54, 66, "", selected ? palette->accent : palette->panel,
               palette->muted, &lv_font_Teko_SemiBold_16, ACT_SETTING_SELECT, (uint8_t)i);
        snprintf(buffer, sizeof(buffer), "S%03d", i + 1);
        text(screen, 1, y + 20, 52, buffer, &lv_font_Teko_SemiBold_16,
             selected ? palette->ink : palette->muted, LV_TEXT_ALIGN_CENTER);
        button(54, y, 324, 66, "", row_fill, palette->text,
               &lv_font_Teko_SemiBold_16, ACT_SETTING_SELECT, (uint8_t)i);
        snprintf(buffer, sizeof(buffer), "%s/RW", setting->group);
        text(screen, 68, y + 7, 300, buffer, &lv_font_Teko_SemiBold_12,
             selected ? palette->accent : palette->muted, LV_TEXT_ALIGN_LEFT);
        text(screen, 68, y + 31, 300, setting->name, &lv_font_Teko_SemiBold_16,
             selected ? palette->bg : palette->text, LV_TEXT_ALIGN_LEFT);
        button(378, y, 102, 66, setting->options[setting->current],
               palette->accent, palette->ink, &lv_font_Teko_SemiBold_16,
               ACT_SETTING_SELECT, (uint8_t)i);
    }
    setting_t *selected = &settings[state.setting_selected];
    static_cell(0, 624, 378, 88, palette->panel);
    text(screen, 16, 640, 340, selected->group, &lv_font_Teko_SemiBold_12,
         palette->muted, LV_TEXT_ALIGN_LEFT);
    text(screen, 16, 667, 340, selected->description, &lv_font_Teko_SemiBold_16,
         palette->text, LV_TEXT_ALIGN_LEFT);
    button(378, 624, 102, 88, state.setting_selected == 1 ? "TEST" : "NEXT",
           palette->bg, palette->muted, &lv_font_Teko_SemiBold_20, ACT_SETTING_NEXT, 0);
    static_cell(0, 712, 480, 36, palette->bg);
    text(screen, 14, 719, 150, "RW: EDITABLE", &lv_font_Teko_SemiBold_12,
         palette->muted, LV_TEXT_ALIGN_LEFT);
    text(screen, 150, 719, 160, "EO: DISPLAY ONLY", &lv_font_Teko_SemiBold_12,
         palette->muted, LV_TEXT_ALIGN_CENTER);
    snprintf(buffer, sizeof(buffer), "%s: %s", selected->name, selected->options[selected->current]);
    text(screen, 310, 719, 156, buffer, &lv_font_Teko_SemiBold_12,
         palette->muted, LV_TEXT_ALIGN_RIGHT);
}
