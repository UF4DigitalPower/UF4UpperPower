/*
 * Copyright 2026 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "custom_internal.h"

static lv_obj_t *s_value_labels[PARAMS_PER_PAGE];
static lv_obj_t *s_editor_label;

void board_params_refresh(void)
{
    if(state.page != PAGE_PARAMS || ui_transition_active()) return;
    for(unsigned row = 0; row < PARAMS_PER_PAGE; ++row) {
        unsigned index = state.param_page * PARAMS_PER_PAGE + row;
        char value[32];
        if(index >= PARAM_COUNT || s_value_labels[row] == NULL) continue;
        format_value(value, sizeof(value), params[index].value, params[index].digits);
        if(strcmp(lv_label_get_text(s_value_labels[row]), value) != 0)
            lv_label_set_text(s_value_labels[row], value);
    }
    unsigned selected = state.param_page * PARAMS_PER_PAGE + state.param_selected;
    if(selected < PARAM_COUNT && s_editor_label) {
        char value[32];
        format_value(value, sizeof(value), params[selected].value, params[selected].digits);
        if(strcmp(lv_label_get_text(s_editor_label), value) != 0)
            lv_label_set_text(s_editor_label, value);
    }
}

float param_step(const param_t *param, bool coarse)
{
    float fine = param->digits == 4 ? 0.0001f : (param->digits == 2 ? 0.01f : 1.0f);
    if(param->id == 0x22) fine = 0.1f;
    if(param->id == 0x1E || param->id == 0x30 || param->id == 0x31 ||
       param->id == 0x32 || param->id == 0x27) fine = 1.0f;
    if(param->max >= 1000.0f) fine = 10.0f;
    if(!coarse) return fine;
    return fine * 10.0f;
}

void draw_params(void)
{
    char buffer[64];
    memset(s_value_labels, 0, sizeof(s_value_labels));
    int first = state.param_page * PARAMS_PER_PAGE;
    int last = first + PARAMS_PER_PAGE;
    if(last > PARAM_COUNT) last = PARAM_COUNT;
    if(state.param_page == 0) {
        lv_obj_t *previous = cell(screen, 0, 118, 72, 44, palette->disabled);
        centered(previous, 72, 44, "PREV", &lv_font_Teko_SemiBold_20, palette->muted);
    }
    else {
        button(0, 118, 72, 44, "PREV", palette->bg, palette->muted,
               &lv_font_Teko_SemiBold_20, ACT_PARAM_PAGE, 0);
    }
    static_cell(72, 118, 336, 44, palette->panel);
    snprintf(buffer, sizeof(buffer), "PARAM %02d-%02d / %d", first + 1, last, PARAM_COUNT);
    text(screen, 74, 126, 332, buffer, &lv_font_Teko_SemiBold_16, palette->text, LV_TEXT_ALIGN_CENTER);
    if(state.param_page == (PARAM_COUNT - 1) / PARAMS_PER_PAGE) {
        lv_obj_t *next = cell(screen, 408, 118, 72, 44, palette->disabled);
        centered(next, 72, 44, "NEXT", &lv_font_Teko_SemiBold_20, palette->muted);
    }
    else {
        button(408, 118, 72, 44, "NEXT", palette->bg, palette->muted,
               &lv_font_Teko_SemiBold_20, ACT_PARAM_PAGE, 1);
    }

    for(int row = 0; row < PARAMS_PER_PAGE; row++) {
        int index = first + row;
        int y = 162 + row * 56;
        if(index >= PARAM_COUNT) {
            static_cell(0, y, 480, 56, palette->bg);
            continue;
        }
        param_t *param = &params[index];
        bool selected = state.param_selected == row;
        uint16_t row_fill = selected ? palette->ink : palette->panel;
        button(0, y, 54, 56, "", selected ? palette->accent : palette->panel,
               palette->muted, &lv_font_Teko_SemiBold_16, ACT_PARAM_SELECT, (uint8_t)row);
        snprintf(buffer, sizeof(buffer), "P%03u", (unsigned)param->id);
        text(screen, 1, y + 16, 52, buffer, &lv_font_Teko_SemiBold_16,
             selected ? palette->ink : palette->muted, LV_TEXT_ALIGN_CENTER);
        button(54, y, 280, 56, "", row_fill, palette->text,
               &lv_font_Teko_SemiBold_16, ACT_PARAM_SELECT, (uint8_t)row);
        text(screen, 64, y + 16, 260, param->name, &lv_font_Teko_SemiBold_16,
             selected ? palette->bg : palette->text, LV_TEXT_ALIGN_LEFT);
        static_cell(334, y, 54, 56, row_fill);
        text(screen, 336, y + 16, 50, "RW", &lv_font_Teko_SemiBold_16,
             selected ? palette->accent : palette->muted, LV_TEXT_ALIGN_CENTER);
        button(388, y, 92, 56, "", palette->accent, palette->ink,
               &lv_font_Teko_SemiBold_16, ACT_PARAM_SELECT, (uint8_t)row);
        format_value(buffer, sizeof(buffer), param->value, param->digits);
        s_value_labels[row] = text(screen, 390, y + 16, 88, buffer, &lv_font_Teko_SemiBold_16,
                                   palette->ink, LV_TEXT_ALIGN_CENTER);
    }

    int selected_index = first + state.param_selected;
    if(selected_index >= PARAM_COUNT) selected_index = PARAM_COUNT - 1;
    param_t *selected = &params[selected_index];
    button(0, 498, 82, 128, "-", palette->bg, palette->muted,
           &lv_font_Teko_SemiBold_28, ACT_PARAM_ADJUST, 0);
    static_cell(82, 498, 316, 128, palette->panel);
    text(screen, 84, 521, 312, selected->name, &lv_font_Teko_SemiBold_16,
         palette->muted, LV_TEXT_ALIGN_CENTER);
    format_value(buffer, sizeof(buffer), selected->value, selected->digits);
    s_editor_label = text(screen, 84, 551, 312, buffer, &lv_font_Teko_SemiBold_28,
                          palette->text, LV_TEXT_ALIGN_CENTER);
    button(398, 498, 82, 128, "+", palette->bg, palette->muted,
           &lv_font_Teko_SemiBold_28, ACT_PARAM_ADJUST, 1);

    for(int i = 0; i < 2; i++) {
        snprintf(buffer, sizeof(buffer), "%.4g", (double)param_step(selected, i != 0));
        button(i * 160, 626, 160, 48, buffer,
               state.param_step == i ? palette->accent : palette->panel,
               state.param_step == i ? palette->text : palette->muted,
               &lv_font_Teko_SemiBold_16, ACT_PARAM_STEP, (uint8_t)i);
    }
    button(320, 626, 160, 48, "SAVE", palette->bg, palette->muted,
           &lv_font_Teko_SemiBold_20, ACT_PARAM_SAVE, 0);
    for(int i = 0; i < 3; i++) static_cell(i * 160, 674, 160, 38, palette->panel);
    text(screen, 12, 681, 67, "ACCESS", &lv_font_Teko_SemiBold_12, palette->muted, LV_TEXT_ALIGN_LEFT);
    text(screen, 80, 681, 68, "RW", &lv_font_Teko_SemiBold_12, palette->text, LV_TEXT_ALIGN_RIGHT);
    text(screen, 172, 681, 55, "RANGE", &lv_font_Teko_SemiBold_12, palette->muted, LV_TEXT_ALIGN_LEFT);
    snprintf(buffer, sizeof(buffer), "%.0f-%.0f", (double)selected->min, (double)selected->max);
    text(screen, 223, 681, 85, buffer, &lv_font_Teko_SemiBold_12, palette->text, LV_TEXT_ALIGN_RIGHT);
    text(screen, 332, 681, 55, "UNIT", &lv_font_Teko_SemiBold_12, palette->muted, LV_TEXT_ALIGN_LEFT);
    text(screen, 376, 681, 92, selected->unit, &lv_font_Teko_SemiBold_12, palette->text, LV_TEXT_ALIGN_RIGHT);
    static_cell(0, 712, 480, 36, palette->bg);
    text(screen, 14, 719, 180, "PARAM TABLE", &lv_font_Teko_SemiBold_12, palette->muted, LV_TEXT_ALIGN_LEFT);
    text(screen, 318, 719, 148, "RW / EO", &lv_font_Teko_SemiBold_12, palette->muted, LV_TEXT_ALIGN_RIGHT);
}
