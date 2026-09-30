/*
 * Copyright 2026 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "custom_internal.h"

void draw_log(void)
{
    static_cell(0, 118, 480, 630, palette->bg);
    text(screen, 16, 128, 330, "LIVE EVENTS", &lv_font_Teko_SemiBold_24,
         palette->text, LV_TEXT_ALIGN_LEFT);
    static_cell(428, 130, 20, 20, palette->accent);
    if(log_count == 0) {
        text(screen, 16, 190, 448, "NO EVENTS", &lv_font_Teko_SemiBold_24,
             palette->muted, LV_TEXT_ALIGN_LEFT);
    }
    for(int i = 0; i < log_count; i++) {
        text(screen, 12, 164 + i * 26, 456, log_lines[i], &lv_font_Teko_SemiBold_16,
             palette->text, LV_TEXT_ALIGN_LEFT);
    }
}
