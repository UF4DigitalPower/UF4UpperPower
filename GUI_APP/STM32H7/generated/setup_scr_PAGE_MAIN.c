/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_PAGE_MAIN(lv_ui *ui)
{
    //Write codes PAGE_MAIN
    ui->PAGE_MAIN = lv_obj_create(NULL);
    lv_obj_set_size(ui->PAGE_MAIN, 480, 640);
    lv_obj_set_scrollbar_mode(ui->PAGE_MAIN, LV_SCROLLBAR_MODE_OFF);

    //Write style for PAGE_MAIN, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN, lv_color_hex(0xcccccc), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_cont_1
    ui->PAGE_MAIN_cont_1 = lv_obj_create(ui->PAGE_MAIN);
    lv_obj_set_pos(ui->PAGE_MAIN_cont_1, 16, 57);
    lv_obj_set_size(ui->PAGE_MAIN_cont_1, 220, 140);
    lv_obj_set_scrollbar_mode(ui->PAGE_MAIN_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for PAGE_MAIN_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_cont_1, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_7
    ui->PAGE_MAIN_label_7 = lv_label_create(ui->PAGE_MAIN_cont_1);
    lv_label_set_text(ui->PAGE_MAIN_label_7, "VIN:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_7, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_7, 10, 10);
    lv_obj_set_size(ui->PAGE_MAIN_label_7, 60, 32);

    //Write style for PAGE_MAIN_label_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_7, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_7, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_7, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_8
    ui->PAGE_MAIN_label_8 = lv_label_create(ui->PAGE_MAIN_cont_1);
    lv_label_set_text(ui->PAGE_MAIN_label_8, "IIN:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_8, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_8, 10, 52);
    lv_obj_set_size(ui->PAGE_MAIN_label_8, 60, 32);

    //Write style for PAGE_MAIN_label_8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_8, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_8, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_8, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_9
    ui->PAGE_MAIN_label_9 = lv_label_create(ui->PAGE_MAIN_cont_1);
    lv_label_set_text(ui->PAGE_MAIN_label_9, "PIN:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_9, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_9, 10, 94);
    lv_obj_set_size(ui->PAGE_MAIN_label_9, 60, 32);

    //Write style for PAGE_MAIN_label_9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_9, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_9, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_9, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_VIN_LABEL
    ui->PAGE_MAIN_VIN_LABEL = lv_label_create(ui->PAGE_MAIN_cont_1);
    lv_label_set_text(ui->PAGE_MAIN_VIN_LABEL, "00.00");
    lv_label_set_long_mode(ui->PAGE_MAIN_VIN_LABEL, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_VIN_LABEL, 70, 10);
    lv_obj_set_size(ui->PAGE_MAIN_VIN_LABEL, 93, 32);

    //Write style for PAGE_MAIN_VIN_LABEL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_VIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_VIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_VIN_LABEL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_VIN_LABEL, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_VIN_LABEL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_VIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_VIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_VIN_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_VIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_VIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_VIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_VIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_VIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_VIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_IIN_LABEL
    ui->PAGE_MAIN_IIN_LABEL = lv_label_create(ui->PAGE_MAIN_cont_1);
    lv_label_set_text(ui->PAGE_MAIN_IIN_LABEL, "00.00");
    lv_label_set_long_mode(ui->PAGE_MAIN_IIN_LABEL, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_IIN_LABEL, 70, 51);
    lv_obj_set_size(ui->PAGE_MAIN_IIN_LABEL, 93, 32);

    //Write style for PAGE_MAIN_IIN_LABEL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_IIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_IIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_IIN_LABEL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_IIN_LABEL, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_IIN_LABEL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_IIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_IIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_IIN_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_IIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_IIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_IIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_IIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_IIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_IIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_PIN_LABEL
    ui->PAGE_MAIN_PIN_LABEL = lv_label_create(ui->PAGE_MAIN_cont_1);
    lv_label_set_text(ui->PAGE_MAIN_PIN_LABEL, "00.00");
    lv_label_set_long_mode(ui->PAGE_MAIN_PIN_LABEL, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_PIN_LABEL, 70, 94);
    lv_obj_set_size(ui->PAGE_MAIN_PIN_LABEL, 93, 32);

    //Write style for PAGE_MAIN_PIN_LABEL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_PIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_PIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_PIN_LABEL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_PIN_LABEL, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_PIN_LABEL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_PIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_PIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_PIN_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_PIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_PIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_PIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_PIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_PIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_PIN_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_19
    ui->PAGE_MAIN_label_19 = lv_label_create(ui->PAGE_MAIN_cont_1);
    lv_label_set_text(ui->PAGE_MAIN_label_19, "V");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_19, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_19, 171, 9);
    lv_obj_set_size(ui->PAGE_MAIN_label_19, 30, 32);

    //Write style for PAGE_MAIN_label_19, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_19, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_19, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_19, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_19, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_19, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_20
    ui->PAGE_MAIN_label_20 = lv_label_create(ui->PAGE_MAIN_cont_1);
    lv_label_set_text(ui->PAGE_MAIN_label_20, "A");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_20, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_20, 171, 52);
    lv_obj_set_size(ui->PAGE_MAIN_label_20, 30, 32);

    //Write style for PAGE_MAIN_label_20, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_20, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_20, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_20, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_20, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_21
    ui->PAGE_MAIN_label_21 = lv_label_create(ui->PAGE_MAIN_cont_1);
    lv_label_set_text(ui->PAGE_MAIN_label_21, "W");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_21, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_21, 171, 95);
    lv_obj_set_size(ui->PAGE_MAIN_label_21, 30, 32);

    //Write style for PAGE_MAIN_label_21, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_21, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_21, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_21, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_21, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_21, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_cont_3
    ui->PAGE_MAIN_cont_3 = lv_obj_create(ui->PAGE_MAIN);
    lv_obj_set_pos(ui->PAGE_MAIN_cont_3, 16, 15);
    lv_obj_set_size(ui->PAGE_MAIN_cont_3, 450, 32);
    lv_obj_set_scrollbar_mode(ui->PAGE_MAIN_cont_3, LV_SCROLLBAR_MODE_OFF);

    //Write style for PAGE_MAIN_cont_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_cont_3, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_cont_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_cont_3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_cont_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_25
    ui->PAGE_MAIN_label_25 = lv_label_create(ui->PAGE_MAIN_cont_3);
    lv_label_set_text(ui->PAGE_MAIN_label_25, "UF4DigitalPower");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_25, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_25, 4, 3);
    lv_obj_set_size(ui->PAGE_MAIN_label_25, 180, 24);

    //Write style for PAGE_MAIN_label_25, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_25, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_25, &lv_font_blender_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_25, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_25, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_25, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_VERSION_LABEL
    ui->PAGE_MAIN_VERSION_LABEL = lv_label_create(ui->PAGE_MAIN_cont_3);
    lv_label_set_text(ui->PAGE_MAIN_VERSION_LABEL, "V1.0.0.112");
    lv_label_set_long_mode(ui->PAGE_MAIN_VERSION_LABEL, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_VERSION_LABEL, 231, 3);
    lv_obj_set_size(ui->PAGE_MAIN_VERSION_LABEL, 180, 24);

    //Write style for PAGE_MAIN_VERSION_LABEL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_VERSION_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_VERSION_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_VERSION_LABEL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_VERSION_LABEL, &lv_font_blender_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_VERSION_LABEL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_VERSION_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_VERSION_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_VERSION_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_VERSION_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_VERSION_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_VERSION_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_VERSION_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_VERSION_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_VERSION_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_chart_1
    ui->PAGE_MAIN_chart_1 = lv_chart_create(ui->PAGE_MAIN);
    lv_chart_set_type(ui->PAGE_MAIN_chart_1, LV_CHART_TYPE_NONE);
    lv_chart_set_div_line_count(ui->PAGE_MAIN_chart_1, 2, 2);
    lv_chart_set_point_count(ui->PAGE_MAIN_chart_1, 5);
    lv_chart_set_range(ui->PAGE_MAIN_chart_1, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_chart_set_range(ui->PAGE_MAIN_chart_1, LV_CHART_AXIS_SECONDARY_Y, 0, 50);
    lv_chart_set_axis_tick(ui->PAGE_MAIN_chart_1, LV_CHART_AXIS_SECONDARY_Y, 6, 1, 6, 1, true, 45);
    lv_chart_set_axis_tick(ui->PAGE_MAIN_chart_1, LV_CHART_AXIS_PRIMARY_X, 4, 5, 5, 4, true, 40);
    lv_chart_set_zoom_x(ui->PAGE_MAIN_chart_1, 256);
    lv_chart_set_zoom_y(ui->PAGE_MAIN_chart_1, 256);
    ui->PAGE_MAIN_chart_1_0 = lv_chart_add_series(ui->PAGE_MAIN_chart_1, lv_color_hex(0x000000), LV_CHART_AXIS_PRIMARY_Y);
#if LV_USE_FREEMASTER == 0
    lv_chart_set_next_value(ui->PAGE_MAIN_chart_1, ui->PAGE_MAIN_chart_1_0, 1);
    lv_chart_set_next_value(ui->PAGE_MAIN_chart_1, ui->PAGE_MAIN_chart_1_0, 20);
    lv_chart_set_next_value(ui->PAGE_MAIN_chart_1, ui->PAGE_MAIN_chart_1_0, 30);
    lv_chart_set_next_value(ui->PAGE_MAIN_chart_1, ui->PAGE_MAIN_chart_1_0, 40);
    lv_chart_set_next_value(ui->PAGE_MAIN_chart_1, ui->PAGE_MAIN_chart_1_0, 5);
#endif
    lv_obj_set_pos(ui->PAGE_MAIN_chart_1, 16, 207);
    lv_obj_set_size(ui->PAGE_MAIN_chart_1, 424, 190);
    lv_obj_set_scrollbar_mode(ui->PAGE_MAIN_chart_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for PAGE_MAIN_chart_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_chart_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_chart_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_chart_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->PAGE_MAIN_chart_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_chart_1, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_width(ui->PAGE_MAIN_chart_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->PAGE_MAIN_chart_1, lv_color_hex(0xe8e8e8), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->PAGE_MAIN_chart_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_chart_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for PAGE_MAIN_chart_1, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->PAGE_MAIN_chart_1, lv_color_hex(0x151212), LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_chart_1, &lv_font_montserratMedium_12, LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_chart_1, 255, LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_line_width(ui->PAGE_MAIN_chart_1, 2, LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->PAGE_MAIN_chart_1, lv_color_hex(0xe8e8e8), LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->PAGE_MAIN_chart_1, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_cont_4
    ui->PAGE_MAIN_cont_4 = lv_obj_create(ui->PAGE_MAIN);
    lv_obj_set_pos(ui->PAGE_MAIN_cont_4, 16, 424);
    lv_obj_set_size(ui->PAGE_MAIN_cont_4, 234, 90);
    lv_obj_set_scrollbar_mode(ui->PAGE_MAIN_cont_4, LV_SCROLLBAR_MODE_OFF);

    //Write style for PAGE_MAIN_cont_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_cont_4, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_cont_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_cont_4, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_cont_4, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_27
    ui->PAGE_MAIN_label_27 = lv_label_create(ui->PAGE_MAIN_cont_4);
    lv_label_set_text(ui->PAGE_MAIN_label_27, "ISET:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_27, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_27, 3, 47);
    lv_obj_set_size(ui->PAGE_MAIN_label_27, 93, 32);

    //Write style for PAGE_MAIN_label_27, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_27, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_27, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_27, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_27, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_27, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_28
    ui->PAGE_MAIN_label_28 = lv_label_create(ui->PAGE_MAIN_cont_4);
    lv_label_set_text(ui->PAGE_MAIN_label_28, "VSET:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_28, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_28, 4, 10);
    lv_obj_set_size(ui->PAGE_MAIN_label_28, 93, 32);

    //Write style for PAGE_MAIN_label_28, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_28, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_28, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_28, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_28, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_28, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_61
    ui->PAGE_MAIN_label_61 = lv_label_create(ui->PAGE_MAIN_cont_4);
    lv_label_set_text(ui->PAGE_MAIN_label_61, "V");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_61, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_61, 203, 10);
    lv_obj_set_size(ui->PAGE_MAIN_label_61, 24, 32);

    //Write style for PAGE_MAIN_label_61, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_61, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_61, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_61, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_61, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_61, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_61, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_61, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_61, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_61, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_61, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_61, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_61, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_61, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_61, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_62
    ui->PAGE_MAIN_label_62 = lv_label_create(ui->PAGE_MAIN_cont_4);
    lv_label_set_text(ui->PAGE_MAIN_label_62, "A");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_62, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_62, 203, 47);
    lv_obj_set_size(ui->PAGE_MAIN_label_62, 24, 31);

    //Write style for PAGE_MAIN_label_62, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_62, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_62, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_62, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_62, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_62, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_62, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_62, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_62, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_62, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_62, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_62, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_62, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_62, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_62, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_VSET_INPUT
    ui->PAGE_MAIN_VSET_INPUT = lv_textarea_create(ui->PAGE_MAIN_cont_4);
    lv_textarea_set_text(ui->PAGE_MAIN_VSET_INPUT, "45.00");
    lv_textarea_set_placeholder_text(ui->PAGE_MAIN_VSET_INPUT, "");
    lv_textarea_set_password_bullet(ui->PAGE_MAIN_VSET_INPUT, "*");
    lv_textarea_set_password_mode(ui->PAGE_MAIN_VSET_INPUT, false);
    lv_textarea_set_one_line(ui->PAGE_MAIN_VSET_INPUT, true);
    lv_textarea_set_accepted_chars(ui->PAGE_MAIN_VSET_INPUT, "1234567890.");
    lv_textarea_set_max_length(ui->PAGE_MAIN_VSET_INPUT, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->PAGE_MAIN_VSET_INPUT, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->PAGE_MAIN_VSET_INPUT, 97, 10);
    lv_obj_set_size(ui->PAGE_MAIN_VSET_INPUT, 107, 35);

    //Write style for PAGE_MAIN_VSET_INPUT, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->PAGE_MAIN_VSET_INPUT, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_VSET_INPUT, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_VSET_INPUT, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_VSET_INPUT, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_VSET_INPUT, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_VSET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->PAGE_MAIN_VSET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_VSET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_VSET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_VSET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_VSET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_VSET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for PAGE_MAIN_VSET_INPUT, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_VSET_INPUT, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_VSET_INPUT, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_VSET_INPUT, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_VSET_INPUT, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_ISET_INPUT
    ui->PAGE_MAIN_ISET_INPUT = lv_textarea_create(ui->PAGE_MAIN_cont_4);
    lv_textarea_set_text(ui->PAGE_MAIN_ISET_INPUT, "45.00");
    lv_textarea_set_placeholder_text(ui->PAGE_MAIN_ISET_INPUT, "");
    lv_textarea_set_password_bullet(ui->PAGE_MAIN_ISET_INPUT, "*");
    lv_textarea_set_password_mode(ui->PAGE_MAIN_ISET_INPUT, false);
    lv_textarea_set_one_line(ui->PAGE_MAIN_ISET_INPUT, true);
    lv_textarea_set_accepted_chars(ui->PAGE_MAIN_ISET_INPUT, "");
    lv_textarea_set_max_length(ui->PAGE_MAIN_ISET_INPUT, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->PAGE_MAIN_ISET_INPUT, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->PAGE_MAIN_ISET_INPUT, 97, 47);
    lv_obj_set_size(ui->PAGE_MAIN_ISET_INPUT, 107, 31);

    //Write style for PAGE_MAIN_ISET_INPUT, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->PAGE_MAIN_ISET_INPUT, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_ISET_INPUT, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_ISET_INPUT, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_ISET_INPUT, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_ISET_INPUT, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_ISET_INPUT, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_ISET_INPUT, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_ISET_INPUT, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->PAGE_MAIN_ISET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_ISET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_ISET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_ISET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_ISET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_ISET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for PAGE_MAIN_ISET_INPUT, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_ISET_INPUT, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_ISET_INPUT, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_ISET_INPUT, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_ISET_INPUT, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_cont_7
    ui->PAGE_MAIN_cont_7 = lv_obj_create(ui->PAGE_MAIN);
    lv_obj_set_pos(ui->PAGE_MAIN_cont_7, 260, 424);
    lv_obj_set_size(ui->PAGE_MAIN_cont_7, 205, 90);
    lv_obj_set_scrollbar_mode(ui->PAGE_MAIN_cont_7, LV_SCROLLBAR_MODE_OFF);

    //Write style for PAGE_MAIN_cont_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_cont_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_cont_7, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_cont_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_cont_7, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_cont_7, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_cont_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_cont_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_cont_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_cont_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_cont_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_63
    ui->PAGE_MAIN_label_63 = lv_label_create(ui->PAGE_MAIN_cont_7);
    lv_label_set_text(ui->PAGE_MAIN_label_63, "OTP:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_63, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_63, 5, 7);
    lv_obj_set_size(ui->PAGE_MAIN_label_63, 36, 17);

    //Write style for PAGE_MAIN_label_63, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_63, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_63, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_63, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_63, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_63, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_63, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_63, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_63, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_63, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_63, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_63, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_63, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_63, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_63, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_64
    ui->PAGE_MAIN_label_64 = lv_label_create(ui->PAGE_MAIN_cont_7);
    lv_label_set_text(ui->PAGE_MAIN_label_64, "OVP:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_64, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_64, 5, 28);
    lv_obj_set_size(ui->PAGE_MAIN_label_64, 36, 17);

    //Write style for PAGE_MAIN_label_64, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_64, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_64, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_64, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_64, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_64, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_64, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_64, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_64, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_64, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_64, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_64, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_64, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_64, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_64, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_65
    ui->PAGE_MAIN_label_65 = lv_label_create(ui->PAGE_MAIN_cont_7);
    lv_label_set_text(ui->PAGE_MAIN_label_65, "OCP:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_65, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_65, 5, 49);
    lv_obj_set_size(ui->PAGE_MAIN_label_65, 36, 17);

    //Write style for PAGE_MAIN_label_65, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_65, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_65, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_65, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_65, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_65, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_65, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_65, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_65, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_65, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_65, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_65, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_65, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_65, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_65, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_66
    ui->PAGE_MAIN_label_66 = lv_label_create(ui->PAGE_MAIN_cont_7);
    lv_label_set_text(ui->PAGE_MAIN_label_66, "UVP:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_66, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_66, 4, 69);
    lv_obj_set_size(ui->PAGE_MAIN_label_66, 36, 17);

    //Write style for PAGE_MAIN_label_66, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_66, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_66, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_66, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_66, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_66, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_66, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_66, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_66, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_66, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_66, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_66, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_66, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_66, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_66, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_OTP_SET_INPUT
    ui->PAGE_MAIN_OTP_SET_INPUT = lv_textarea_create(ui->PAGE_MAIN_cont_7);
    lv_textarea_set_text(ui->PAGE_MAIN_OTP_SET_INPUT, "80.00");
    lv_textarea_set_placeholder_text(ui->PAGE_MAIN_OTP_SET_INPUT, "");
    lv_textarea_set_password_bullet(ui->PAGE_MAIN_OTP_SET_INPUT, "*");
    lv_textarea_set_password_mode(ui->PAGE_MAIN_OTP_SET_INPUT, false);
    lv_textarea_set_one_line(ui->PAGE_MAIN_OTP_SET_INPUT, true);
    lv_textarea_set_accepted_chars(ui->PAGE_MAIN_OTP_SET_INPUT, "1234567890.");
    lv_textarea_set_max_length(ui->PAGE_MAIN_OTP_SET_INPUT, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->PAGE_MAIN_OTP_SET_INPUT, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->PAGE_MAIN_OTP_SET_INPUT, 43, 6);
    lv_obj_set_size(ui->PAGE_MAIN_OTP_SET_INPUT, 41, 15);

    //Write style for PAGE_MAIN_OTP_SET_INPUT, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->PAGE_MAIN_OTP_SET_INPUT, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_OTP_SET_INPUT, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_OTP_SET_INPUT, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_OTP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_OTP_SET_INPUT, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_OTP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->PAGE_MAIN_OTP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_OTP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_OTP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_OTP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_OTP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_OTP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for PAGE_MAIN_OTP_SET_INPUT, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_OTP_SET_INPUT, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_OTP_SET_INPUT, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_OTP_SET_INPUT, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_OTP_SET_INPUT, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_OVP_SET_INPUT
    ui->PAGE_MAIN_OVP_SET_INPUT = lv_textarea_create(ui->PAGE_MAIN_cont_7);
    lv_textarea_set_text(ui->PAGE_MAIN_OVP_SET_INPUT, "45.00");
    lv_textarea_set_placeholder_text(ui->PAGE_MAIN_OVP_SET_INPUT, "");
    lv_textarea_set_password_bullet(ui->PAGE_MAIN_OVP_SET_INPUT, "*");
    lv_textarea_set_password_mode(ui->PAGE_MAIN_OVP_SET_INPUT, false);
    lv_textarea_set_one_line(ui->PAGE_MAIN_OVP_SET_INPUT, true);
    lv_textarea_set_accepted_chars(ui->PAGE_MAIN_OVP_SET_INPUT, "1234567890.");
    lv_textarea_set_max_length(ui->PAGE_MAIN_OVP_SET_INPUT, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->PAGE_MAIN_OVP_SET_INPUT, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->PAGE_MAIN_OVP_SET_INPUT, 43, 27);
    lv_obj_set_size(ui->PAGE_MAIN_OVP_SET_INPUT, 41, 15);

    //Write style for PAGE_MAIN_OVP_SET_INPUT, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->PAGE_MAIN_OVP_SET_INPUT, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_OVP_SET_INPUT, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_OVP_SET_INPUT, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_OVP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_OVP_SET_INPUT, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_OVP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->PAGE_MAIN_OVP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_OVP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_OVP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_OVP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_OVP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_OVP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for PAGE_MAIN_OVP_SET_INPUT, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_OVP_SET_INPUT, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_OVP_SET_INPUT, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_OVP_SET_INPUT, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_OVP_SET_INPUT, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_OCP_SET_INPUT
    ui->PAGE_MAIN_OCP_SET_INPUT = lv_textarea_create(ui->PAGE_MAIN_cont_7);
    lv_textarea_set_text(ui->PAGE_MAIN_OCP_SET_INPUT, "10.00");
    lv_textarea_set_placeholder_text(ui->PAGE_MAIN_OCP_SET_INPUT, "");
    lv_textarea_set_password_bullet(ui->PAGE_MAIN_OCP_SET_INPUT, "*");
    lv_textarea_set_password_mode(ui->PAGE_MAIN_OCP_SET_INPUT, false);
    lv_textarea_set_one_line(ui->PAGE_MAIN_OCP_SET_INPUT, true);
    lv_textarea_set_accepted_chars(ui->PAGE_MAIN_OCP_SET_INPUT, "1234567890.");
    lv_textarea_set_max_length(ui->PAGE_MAIN_OCP_SET_INPUT, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->PAGE_MAIN_OCP_SET_INPUT, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->PAGE_MAIN_OCP_SET_INPUT, 42, 48);
    lv_obj_set_size(ui->PAGE_MAIN_OCP_SET_INPUT, 41, 15);

    //Write style for PAGE_MAIN_OCP_SET_INPUT, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->PAGE_MAIN_OCP_SET_INPUT, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_OCP_SET_INPUT, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_OCP_SET_INPUT, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_OCP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_OCP_SET_INPUT, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_OCP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->PAGE_MAIN_OCP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_OCP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_OCP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_OCP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_OCP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_OCP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for PAGE_MAIN_OCP_SET_INPUT, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_OCP_SET_INPUT, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_OCP_SET_INPUT, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_OCP_SET_INPUT, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_OCP_SET_INPUT, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_UVP_SET_INPUT
    ui->PAGE_MAIN_UVP_SET_INPUT = lv_textarea_create(ui->PAGE_MAIN_cont_7);
    lv_textarea_set_text(ui->PAGE_MAIN_UVP_SET_INPUT, "05.00");
    lv_textarea_set_placeholder_text(ui->PAGE_MAIN_UVP_SET_INPUT, "");
    lv_textarea_set_password_bullet(ui->PAGE_MAIN_UVP_SET_INPUT, "*");
    lv_textarea_set_password_mode(ui->PAGE_MAIN_UVP_SET_INPUT, false);
    lv_textarea_set_one_line(ui->PAGE_MAIN_UVP_SET_INPUT, true);
    lv_textarea_set_accepted_chars(ui->PAGE_MAIN_UVP_SET_INPUT, "1234567890.");
    lv_textarea_set_max_length(ui->PAGE_MAIN_UVP_SET_INPUT, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->PAGE_MAIN_UVP_SET_INPUT, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->PAGE_MAIN_UVP_SET_INPUT, 42, 68);
    lv_obj_set_size(ui->PAGE_MAIN_UVP_SET_INPUT, 41, 15);

    //Write style for PAGE_MAIN_UVP_SET_INPUT, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->PAGE_MAIN_UVP_SET_INPUT, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_UVP_SET_INPUT, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_UVP_SET_INPUT, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_UVP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_UVP_SET_INPUT, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_UVP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->PAGE_MAIN_UVP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_UVP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_UVP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_UVP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_UVP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_UVP_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for PAGE_MAIN_UVP_SET_INPUT, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_UVP_SET_INPUT, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_UVP_SET_INPUT, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_UVP_SET_INPUT, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_UVP_SET_INPUT, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_67
    ui->PAGE_MAIN_label_67 = lv_label_create(ui->PAGE_MAIN_cont_7);
    lv_label_set_text(ui->PAGE_MAIN_label_67, "FAN:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_67, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_67, 104, 7);
    lv_obj_set_size(ui->PAGE_MAIN_label_67, 36, 17);

    //Write style for PAGE_MAIN_label_67, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_67, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_67, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_67, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_67, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_67, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_67, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_67, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_67, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_67, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_67, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_67, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_67, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_67, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_67, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_FAN_SET_INPUT
    ui->PAGE_MAIN_FAN_SET_INPUT = lv_textarea_create(ui->PAGE_MAIN_cont_7);
    lv_textarea_set_text(ui->PAGE_MAIN_FAN_SET_INPUT, "1000");
    lv_textarea_set_placeholder_text(ui->PAGE_MAIN_FAN_SET_INPUT, "");
    lv_textarea_set_password_bullet(ui->PAGE_MAIN_FAN_SET_INPUT, "*");
    lv_textarea_set_password_mode(ui->PAGE_MAIN_FAN_SET_INPUT, false);
    lv_textarea_set_one_line(ui->PAGE_MAIN_FAN_SET_INPUT, true);
    lv_textarea_set_accepted_chars(ui->PAGE_MAIN_FAN_SET_INPUT, "1234567890.");
    lv_textarea_set_max_length(ui->PAGE_MAIN_FAN_SET_INPUT, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->PAGE_MAIN_FAN_SET_INPUT, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->PAGE_MAIN_FAN_SET_INPUT, 141, 6);
    lv_obj_set_size(ui->PAGE_MAIN_FAN_SET_INPUT, 41, 15);

    //Write style for PAGE_MAIN_FAN_SET_INPUT, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->PAGE_MAIN_FAN_SET_INPUT, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_FAN_SET_INPUT, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_FAN_SET_INPUT, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_FAN_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_FAN_SET_INPUT, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_FAN_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->PAGE_MAIN_FAN_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_FAN_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_FAN_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_FAN_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_FAN_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_FAN_SET_INPUT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for PAGE_MAIN_FAN_SET_INPUT, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_FAN_SET_INPUT, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_FAN_SET_INPUT, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_FAN_SET_INPUT, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_FAN_SET_INPUT, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_cont_2
    ui->PAGE_MAIN_cont_2 = lv_obj_create(ui->PAGE_MAIN);
    lv_obj_set_pos(ui->PAGE_MAIN_cont_2, 247, 57);
    lv_obj_set_size(ui->PAGE_MAIN_cont_2, 220, 140);
    lv_obj_set_scrollbar_mode(ui->PAGE_MAIN_cont_2, LV_SCROLLBAR_MODE_OFF);

    //Write style for PAGE_MAIN_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_cont_2, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_cont_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_cont_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_12
    ui->PAGE_MAIN_label_12 = lv_label_create(ui->PAGE_MAIN_cont_2);
    lv_label_set_text(ui->PAGE_MAIN_label_12, "VOUT:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_12, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_12, 10, 7);
    lv_obj_set_size(ui->PAGE_MAIN_label_12, 80, 32);

    //Write style for PAGE_MAIN_label_12, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_12, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_12, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_12, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_12, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_11
    ui->PAGE_MAIN_label_11 = lv_label_create(ui->PAGE_MAIN_cont_2);
    lv_label_set_text(ui->PAGE_MAIN_label_11, "IOUT :");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_11, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_11, 10, 51);
    lv_obj_set_size(ui->PAGE_MAIN_label_11, 80, 32);

    //Write style for PAGE_MAIN_label_11, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_11, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_11, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_11, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_11, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_10
    ui->PAGE_MAIN_label_10 = lv_label_create(ui->PAGE_MAIN_cont_2);
    lv_label_set_text(ui->PAGE_MAIN_label_10, "POUT:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_10, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_10, 10, 95);
    lv_obj_set_size(ui->PAGE_MAIN_label_10, 80, 32);

    //Write style for PAGE_MAIN_label_10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_10, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_10, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_10, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_10, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_VOUT_LABEL
    ui->PAGE_MAIN_VOUT_LABEL = lv_label_create(ui->PAGE_MAIN_cont_2);
    lv_label_set_text(ui->PAGE_MAIN_VOUT_LABEL, "00.00");
    lv_label_set_long_mode(ui->PAGE_MAIN_VOUT_LABEL, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_VOUT_LABEL, 93, 7);
    lv_obj_set_size(ui->PAGE_MAIN_VOUT_LABEL, 93, 32);

    //Write style for PAGE_MAIN_VOUT_LABEL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_VOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_VOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_VOUT_LABEL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_VOUT_LABEL, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_VOUT_LABEL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_VOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_VOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_VOUT_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_VOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_VOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_VOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_VOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_VOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_VOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_IOUT_LABEL
    ui->PAGE_MAIN_IOUT_LABEL = lv_label_create(ui->PAGE_MAIN_cont_2);
    lv_label_set_text(ui->PAGE_MAIN_IOUT_LABEL, "00.00");
    lv_label_set_long_mode(ui->PAGE_MAIN_IOUT_LABEL, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_IOUT_LABEL, 93, 51);
    lv_obj_set_size(ui->PAGE_MAIN_IOUT_LABEL, 93, 32);

    //Write style for PAGE_MAIN_IOUT_LABEL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_IOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_IOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_IOUT_LABEL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_IOUT_LABEL, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_IOUT_LABEL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_IOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_IOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_IOUT_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_IOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_IOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_IOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_IOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_IOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_IOUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_POUT_LABEL
    ui->PAGE_MAIN_POUT_LABEL = lv_label_create(ui->PAGE_MAIN_cont_2);
    lv_label_set_text(ui->PAGE_MAIN_POUT_LABEL, "00.00");
    lv_label_set_long_mode(ui->PAGE_MAIN_POUT_LABEL, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_POUT_LABEL, 93, 95);
    lv_obj_set_size(ui->PAGE_MAIN_POUT_LABEL, 93, 32);

    //Write style for PAGE_MAIN_POUT_LABEL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_POUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_POUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_POUT_LABEL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_POUT_LABEL, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_POUT_LABEL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_POUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_POUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_POUT_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_POUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_POUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_POUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_POUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_POUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_POUT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_22
    ui->PAGE_MAIN_label_22 = lv_label_create(ui->PAGE_MAIN_cont_2);
    lv_label_set_text(ui->PAGE_MAIN_label_22, "V");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_22, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_22, 182, 7);
    lv_obj_set_size(ui->PAGE_MAIN_label_22, 30, 32);

    //Write style for PAGE_MAIN_label_22, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_22, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_22, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_22, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_22, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_23
    ui->PAGE_MAIN_label_23 = lv_label_create(ui->PAGE_MAIN_cont_2);
    lv_label_set_text(ui->PAGE_MAIN_label_23, "A");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_23, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_23, 182, 52);
    lv_obj_set_size(ui->PAGE_MAIN_label_23, 30, 32);

    //Write style for PAGE_MAIN_label_23, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_23, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_23, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_23, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_23, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_23, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_24
    ui->PAGE_MAIN_label_24 = lv_label_create(ui->PAGE_MAIN_cont_2);
    lv_label_set_text(ui->PAGE_MAIN_label_24, "W");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_24, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_24, 182, 97);
    lv_obj_set_size(ui->PAGE_MAIN_label_24, 30, 32);

    //Write style for PAGE_MAIN_label_24, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_24, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_24, &lv_font_blender_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_24, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_24, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_24, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_cont_6
    ui->PAGE_MAIN_cont_6 = lv_obj_create(ui->PAGE_MAIN);
    lv_obj_set_pos(ui->PAGE_MAIN_cont_6, 137, 525);
    lv_obj_set_size(ui->PAGE_MAIN_cont_6, 113, 100);
    lv_obj_set_scrollbar_mode(ui->PAGE_MAIN_cont_6, LV_SCROLLBAR_MODE_OFF);

    //Write style for PAGE_MAIN_cont_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_cont_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_cont_6, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_cont_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_cont_6, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_cont_6, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_cont_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_cont_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_cont_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_cont_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_cont_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_42
    ui->PAGE_MAIN_label_42 = lv_label_create(ui->PAGE_MAIN_cont_6);
    lv_label_set_text(ui->PAGE_MAIN_label_42, "CORE  :");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_42, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_42, 4, 31);
    lv_obj_set_size(ui->PAGE_MAIN_label_42, 48, 17);

    //Write style for PAGE_MAIN_label_42, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_42, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_42, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_42, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_42, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_42, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_42, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_42, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_42, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_42, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_42, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_42, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_42, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_42, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_42, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_41
    ui->PAGE_MAIN_label_41 = lv_label_create(ui->PAGE_MAIN_cont_6);
    lv_label_set_text(ui->PAGE_MAIN_label_41, "TEMP1:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_41, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_41, 3, 52);
    lv_obj_set_size(ui->PAGE_MAIN_label_41, 49, 17);

    //Write style for PAGE_MAIN_label_41, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_41, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_41, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_41, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_41, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_41, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_41, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_41, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_41, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_41, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_41, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_41, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_41, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_41, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_41, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_40
    ui->PAGE_MAIN_label_40 = lv_label_create(ui->PAGE_MAIN_cont_6);
    lv_label_set_text(ui->PAGE_MAIN_label_40, "TEMP2:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_40, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_40, 3, 73);
    lv_obj_set_size(ui->PAGE_MAIN_label_40, 49, 17);

    //Write style for PAGE_MAIN_label_40, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_40, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_40, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_40, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_40, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_40, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_40, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_40, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_40, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_40, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_40, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_40, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_40, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_40, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_40, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_CORETEMP_LABEL
    ui->PAGE_MAIN_CORETEMP_LABEL = lv_label_create(ui->PAGE_MAIN_cont_6);
    lv_label_set_text(ui->PAGE_MAIN_CORETEMP_LABEL, "25.00");
    lv_label_set_long_mode(ui->PAGE_MAIN_CORETEMP_LABEL, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_CORETEMP_LABEL, 58, 31);
    lv_obj_set_size(ui->PAGE_MAIN_CORETEMP_LABEL, 43, 17);

    //Write style for PAGE_MAIN_CORETEMP_LABEL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_CORETEMP_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_CORETEMP_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_CORETEMP_LABEL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_CORETEMP_LABEL, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_CORETEMP_LABEL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_CORETEMP_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_CORETEMP_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_CORETEMP_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_CORETEMP_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_CORETEMP_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_CORETEMP_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_CORETEMP_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_CORETEMP_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_CORETEMP_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_TEMP2_LABEL
    ui->PAGE_MAIN_TEMP2_LABEL = lv_label_create(ui->PAGE_MAIN_cont_6);
    lv_label_set_text(ui->PAGE_MAIN_TEMP2_LABEL, "25.00\n");
    lv_label_set_long_mode(ui->PAGE_MAIN_TEMP2_LABEL, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_TEMP2_LABEL, 58, 73);
    lv_obj_set_size(ui->PAGE_MAIN_TEMP2_LABEL, 43, 17);

    //Write style for PAGE_MAIN_TEMP2_LABEL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_TEMP2_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_TEMP2_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_TEMP2_LABEL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_TEMP2_LABEL, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_TEMP2_LABEL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_TEMP2_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_TEMP2_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_TEMP2_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_TEMP2_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_TEMP2_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_TEMP2_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_TEMP2_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_TEMP2_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_TEMP2_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_TEMP1_LABEL
    ui->PAGE_MAIN_TEMP1_LABEL = lv_label_create(ui->PAGE_MAIN_cont_6);
    lv_label_set_text(ui->PAGE_MAIN_TEMP1_LABEL, "25.00\n");
    lv_label_set_long_mode(ui->PAGE_MAIN_TEMP1_LABEL, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_TEMP1_LABEL, 58, 52);
    lv_obj_set_size(ui->PAGE_MAIN_TEMP1_LABEL, 43, 17);

    //Write style for PAGE_MAIN_TEMP1_LABEL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_TEMP1_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_TEMP1_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_TEMP1_LABEL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_TEMP1_LABEL, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_TEMP1_LABEL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_TEMP1_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_TEMP1_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_TEMP1_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_TEMP1_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_TEMP1_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_TEMP1_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_TEMP1_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_TEMP1_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_TEMP1_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_45
    ui->PAGE_MAIN_label_45 = lv_label_create(ui->PAGE_MAIN_cont_6);
    lv_label_set_text(ui->PAGE_MAIN_label_45, "Temperature");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_45, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_45, 23, 10);
    lv_obj_set_size(ui->PAGE_MAIN_label_45, 83, 17);

    //Write style for PAGE_MAIN_label_45, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_45, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_45, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_45, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_45, &lv_font_blender_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_45, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_45, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_45, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_45, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_45, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_45, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_45, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_45, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_45, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_45, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_img_1
    ui->PAGE_MAIN_img_1 = lv_img_create(ui->PAGE_MAIN_cont_6);
    lv_obj_add_flag(ui->PAGE_MAIN_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->PAGE_MAIN_img_1, &_ic_fluent_temperature_48_regular_alpha_24x24);
    lv_img_set_pivot(ui->PAGE_MAIN_img_1, 24,24);
    lv_img_set_angle(ui->PAGE_MAIN_img_1, 0);
    lv_obj_set_pos(ui->PAGE_MAIN_img_1, 3, 6);
    lv_obj_set_size(ui->PAGE_MAIN_img_1, 24, 24);

    //Write style for PAGE_MAIN_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->PAGE_MAIN_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->PAGE_MAIN_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->PAGE_MAIN_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_cont_5
    ui->PAGE_MAIN_cont_5 = lv_obj_create(ui->PAGE_MAIN);
    lv_obj_set_pos(ui->PAGE_MAIN_cont_5, 16, 525);
    lv_obj_set_size(ui->PAGE_MAIN_cont_5, 110, 100);
    lv_obj_set_scrollbar_mode(ui->PAGE_MAIN_cont_5, LV_SCROLLBAR_MODE_OFF);

    //Write style for PAGE_MAIN_cont_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_cont_5, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_cont_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_cont_5, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_cont_5, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_MODE_LABEL
    ui->PAGE_MAIN_MODE_LABEL = lv_label_create(ui->PAGE_MAIN_cont_5);
    lv_label_set_text(ui->PAGE_MAIN_MODE_LABEL, "CC/CV");
    lv_label_set_long_mode(ui->PAGE_MAIN_MODE_LABEL, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_MODE_LABEL, 53, 7);
    lv_obj_set_size(ui->PAGE_MAIN_MODE_LABEL, 43, 17);

    //Write style for PAGE_MAIN_MODE_LABEL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_MODE_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_MODE_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_MODE_LABEL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_MODE_LABEL, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_MODE_LABEL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_MODE_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_MODE_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_MODE_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_MODE_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_MODE_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_MODE_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_MODE_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_MODE_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_MODE_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_30
    ui->PAGE_MAIN_label_30 = lv_label_create(ui->PAGE_MAIN_cont_5);
    lv_label_set_text(ui->PAGE_MAIN_label_30, "MODE:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_30, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_30, 8, 8);
    lv_obj_set_size(ui->PAGE_MAIN_label_30, 44, 17);

    //Write style for PAGE_MAIN_label_30, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_30, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_30, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_30, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_30, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_30, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_31
    ui->PAGE_MAIN_label_31 = lv_label_create(ui->PAGE_MAIN_cont_5);
    lv_label_set_text(ui->PAGE_MAIN_label_31, "TOPO :");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_31, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_31, 8, 30);
    lv_obj_set_size(ui->PAGE_MAIN_label_31, 44, 17);

    //Write style for PAGE_MAIN_label_31, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_31, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_31, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_31, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_31, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_31, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_32
    ui->PAGE_MAIN_label_32 = lv_label_create(ui->PAGE_MAIN_cont_5);
    lv_label_set_text(ui->PAGE_MAIN_label_32, "FAULT:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_32, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_32, 8, 52);
    lv_obj_set_size(ui->PAGE_MAIN_label_32, 44, 17);

    //Write style for PAGE_MAIN_label_32, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_32, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_32, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_32, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_32, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_32, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_32, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_32, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_32, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_32, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_32, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_32, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_32, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_32, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_32, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_33
    ui->PAGE_MAIN_label_33 = lv_label_create(ui->PAGE_MAIN_cont_5);
    lv_label_set_text(ui->PAGE_MAIN_label_33, "FSM  :");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_33, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_33, 8, 74);
    lv_obj_set_size(ui->PAGE_MAIN_label_33, 44, 17);

    //Write style for PAGE_MAIN_label_33, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_33, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_33, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_33, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_33, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_33, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_33, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_33, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_33, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_33, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_33, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_33, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_33, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_33, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_33, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_TOPO_LABEL
    ui->PAGE_MAIN_TOPO_LABEL = lv_label_create(ui->PAGE_MAIN_cont_5);
    lv_label_set_text(ui->PAGE_MAIN_TOPO_LABEL, "BUCK\n");
    lv_label_set_long_mode(ui->PAGE_MAIN_TOPO_LABEL, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_TOPO_LABEL, 53, 30);
    lv_obj_set_size(ui->PAGE_MAIN_TOPO_LABEL, 43, 17);

    //Write style for PAGE_MAIN_TOPO_LABEL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_TOPO_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_TOPO_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_TOPO_LABEL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_TOPO_LABEL, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_TOPO_LABEL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_TOPO_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_TOPO_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_TOPO_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_TOPO_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_TOPO_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_TOPO_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_TOPO_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_TOPO_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_TOPO_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_FAULT_LABEL
    ui->PAGE_MAIN_FAULT_LABEL = lv_label_create(ui->PAGE_MAIN_cont_5);
    lv_label_set_text(ui->PAGE_MAIN_FAULT_LABEL, "OTP\n");
    lv_label_set_long_mode(ui->PAGE_MAIN_FAULT_LABEL, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_FAULT_LABEL, 53, 52);
    lv_obj_set_size(ui->PAGE_MAIN_FAULT_LABEL, 43, 17);

    //Write style for PAGE_MAIN_FAULT_LABEL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_FAULT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_FAULT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_FAULT_LABEL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_FAULT_LABEL, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_FAULT_LABEL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_FAULT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_FAULT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_FAULT_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_FAULT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_FAULT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_FAULT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_FAULT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_FAULT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_FAULT_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_FSM_LABEL
    ui->PAGE_MAIN_FSM_LABEL = lv_label_create(ui->PAGE_MAIN_cont_5);
    lv_label_set_text(ui->PAGE_MAIN_FSM_LABEL, "RUN\n");
    lv_label_set_long_mode(ui->PAGE_MAIN_FSM_LABEL, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_FSM_LABEL, 53, 73);
    lv_obj_set_size(ui->PAGE_MAIN_FSM_LABEL, 43, 17);

    //Write style for PAGE_MAIN_FSM_LABEL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_FSM_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_FSM_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_FSM_LABEL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_FSM_LABEL, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_FSM_LABEL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_FSM_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_FSM_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_FSM_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_FSM_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_FSM_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_FSM_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_FSM_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_FSM_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_FSM_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_cont_9
    ui->PAGE_MAIN_cont_9 = lv_obj_create(ui->PAGE_MAIN);
    lv_obj_set_pos(ui->PAGE_MAIN_cont_9, 260, 525);
    lv_obj_set_size(ui->PAGE_MAIN_cont_9, 113, 100);
    lv_obj_set_scrollbar_mode(ui->PAGE_MAIN_cont_9, LV_SCROLLBAR_MODE_OFF);

    //Write style for PAGE_MAIN_cont_9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_cont_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_cont_9, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_cont_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_cont_9, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_cont_9, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_cont_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_cont_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_cont_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_cont_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_cont_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_76
    ui->PAGE_MAIN_label_76 = lv_label_create(ui->PAGE_MAIN_cont_9);
    lv_label_set_text(ui->PAGE_MAIN_label_76, "STATE:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_76, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_76, 4, 30);
    lv_obj_set_size(ui->PAGE_MAIN_label_76, 48, 17);

    //Write style for PAGE_MAIN_label_76, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_76, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_76, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_76, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_76, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_76, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_76, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_76, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_76, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_76, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_76, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_76, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_76, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_76, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_76, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_75
    ui->PAGE_MAIN_label_75 = lv_label_create(ui->PAGE_MAIN_cont_9);
    lv_label_set_text(ui->PAGE_MAIN_label_75, "CNCT:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_75, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_75, 3, 52);
    lv_obj_set_size(ui->PAGE_MAIN_label_75, 49, 17);

    //Write style for PAGE_MAIN_label_75, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_75, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_75, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_75, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_75, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_75, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_75, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_75, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_75, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_75, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_75, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_75, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_75, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_75, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_75, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_74
    ui->PAGE_MAIN_label_74 = lv_label_create(ui->PAGE_MAIN_cont_9);
    lv_label_set_text(ui->PAGE_MAIN_label_74, "PC:");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_74, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_74, 3, 73);
    lv_obj_set_size(ui->PAGE_MAIN_label_74, 49, 17);

    //Write style for PAGE_MAIN_label_74, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_74, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_74, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_74, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_74, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_74, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_74, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_74, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_74, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_74, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_74, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_74, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_74, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_74, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_74, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_PC_CTRL_LABEL
    ui->PAGE_MAIN_PC_CTRL_LABEL = lv_label_create(ui->PAGE_MAIN_cont_9);
    lv_label_set_text(ui->PAGE_MAIN_PC_CTRL_LABEL, "NONE\n");
    lv_label_set_long_mode(ui->PAGE_MAIN_PC_CTRL_LABEL, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_PC_CTRL_LABEL, 58, 73);
    lv_obj_set_size(ui->PAGE_MAIN_PC_CTRL_LABEL, 43, 17);

    //Write style for PAGE_MAIN_PC_CTRL_LABEL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_PC_CTRL_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_PC_CTRL_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_PC_CTRL_LABEL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_PC_CTRL_LABEL, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_PC_CTRL_LABEL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_PC_CTRL_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_PC_CTRL_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_PC_CTRL_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_PC_CTRL_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_PC_CTRL_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_PC_CTRL_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_PC_CTRL_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_PC_CTRL_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_PC_CTRL_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_CONNECT_TO_PC_LABEL
    ui->PAGE_MAIN_CONNECT_TO_PC_LABEL = lv_label_create(ui->PAGE_MAIN_cont_9);
    lv_label_set_text(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, "ON\n");
    lv_label_set_long_mode(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, 56, 52);
    lv_obj_set_size(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, 43, 17);

    //Write style for PAGE_MAIN_CONNECT_TO_PC_LABEL, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, &lv_font_blender_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_CONNECT_TO_PC_LABEL, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_label_70
    ui->PAGE_MAIN_label_70 = lv_label_create(ui->PAGE_MAIN_cont_9);
    lv_label_set_text(ui->PAGE_MAIN_label_70, "Bluetooth");
    lv_label_set_long_mode(ui->PAGE_MAIN_label_70, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->PAGE_MAIN_label_70, 23, 10);
    lv_obj_set_size(ui->PAGE_MAIN_label_70, 83, 17);

    //Write style for PAGE_MAIN_label_70, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->PAGE_MAIN_label_70, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_label_70, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->PAGE_MAIN_label_70, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->PAGE_MAIN_label_70, &lv_font_blender_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->PAGE_MAIN_label_70, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->PAGE_MAIN_label_70, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->PAGE_MAIN_label_70, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->PAGE_MAIN_label_70, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_label_70, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->PAGE_MAIN_label_70, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->PAGE_MAIN_label_70, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->PAGE_MAIN_label_70, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->PAGE_MAIN_label_70, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_label_70, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_img_2
    ui->PAGE_MAIN_img_2 = lv_img_create(ui->PAGE_MAIN_cont_9);
    lv_obj_add_flag(ui->PAGE_MAIN_img_2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->PAGE_MAIN_img_2, &_ic_fluent_bluetooth_48_regular_alpha_24x24);
    lv_img_set_pivot(ui->PAGE_MAIN_img_2, 24,24);
    lv_img_set_angle(ui->PAGE_MAIN_img_2, 0);
    lv_obj_set_pos(ui->PAGE_MAIN_img_2, 3, 6);
    lv_obj_set_size(ui->PAGE_MAIN_img_2, 24, 24);

    //Write style for PAGE_MAIN_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->PAGE_MAIN_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->PAGE_MAIN_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->PAGE_MAIN_img_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes PAGE_MAIN_SWITCH_BLE
    ui->PAGE_MAIN_SWITCH_BLE = lv_switch_create(ui->PAGE_MAIN_cont_9);
    lv_obj_set_pos(ui->PAGE_MAIN_SWITCH_BLE, 63, 30);
    lv_obj_set_size(ui->PAGE_MAIN_SWITCH_BLE, 30, 15);

    //Write style for PAGE_MAIN_SWITCH_BLE, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_SWITCH_BLE, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_SWITCH_BLE, lv_color_hex(0xe6e2e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_SWITCH_BLE, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->PAGE_MAIN_SWITCH_BLE, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_SWITCH_BLE, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->PAGE_MAIN_SWITCH_BLE, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for PAGE_MAIN_SWITCH_BLE, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_SWITCH_BLE, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_SWITCH_BLE, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_SWITCH_BLE, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->PAGE_MAIN_SWITCH_BLE, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

    //Write style for PAGE_MAIN_SWITCH_BLE, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->PAGE_MAIN_SWITCH_BLE, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->PAGE_MAIN_SWITCH_BLE, lv_color_hex(0xffffff), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->PAGE_MAIN_SWITCH_BLE, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->PAGE_MAIN_SWITCH_BLE, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->PAGE_MAIN_SWITCH_BLE, 10, LV_PART_KNOB|LV_STATE_DEFAULT);

    //The custom code of PAGE_MAIN.


    //Update current screen layout.
    lv_obj_update_layout(ui->PAGE_MAIN);

}
