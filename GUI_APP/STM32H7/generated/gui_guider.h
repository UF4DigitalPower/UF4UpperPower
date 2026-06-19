/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

typedef struct
{
  
	lv_obj_t *g_kb_top_layer;
	lv_obj_t *PAGE_MAIN;
	bool PAGE_MAIN_del;
	lv_obj_t *PAGE_MAIN_cont_1;
	lv_obj_t *PAGE_MAIN_label_7;
	lv_obj_t *PAGE_MAIN_label_8;
	lv_obj_t *PAGE_MAIN_label_9;
	lv_obj_t *PAGE_MAIN_VIN_LABEL;
	lv_obj_t *PAGE_MAIN_IIN_LABEL;
	lv_obj_t *PAGE_MAIN_PIN_LABEL;
	lv_obj_t *PAGE_MAIN_label_19;
	lv_obj_t *PAGE_MAIN_label_20;
	lv_obj_t *PAGE_MAIN_label_21;
	lv_obj_t *PAGE_MAIN_cont_3;
	lv_obj_t *PAGE_MAIN_label_25;
	lv_obj_t *PAGE_MAIN_VERSION_LABEL;
	lv_obj_t *PAGE_MAIN_chart_1;
	lv_chart_series_t *PAGE_MAIN_chart_1_0;
	lv_obj_t *PAGE_MAIN_cont_4;
	lv_obj_t *PAGE_MAIN_label_27;
	lv_obj_t *PAGE_MAIN_label_28;
	lv_obj_t *PAGE_MAIN_label_61;
	lv_obj_t *PAGE_MAIN_label_62;
	lv_obj_t *PAGE_MAIN_VSET_INPUT;
	lv_obj_t *PAGE_MAIN_ISET_INPUT;
	lv_obj_t *PAGE_MAIN_cont_7;
	lv_obj_t *PAGE_MAIN_label_63;
	lv_obj_t *PAGE_MAIN_label_64;
	lv_obj_t *PAGE_MAIN_label_65;
	lv_obj_t *PAGE_MAIN_label_66;
	lv_obj_t *PAGE_MAIN_OTP_SET_INPUT;
	lv_obj_t *PAGE_MAIN_OVP_SET_INPUT;
	lv_obj_t *PAGE_MAIN_OCP_SET_INPUT;
	lv_obj_t *PAGE_MAIN_UVP_SET_INPUT;
	lv_obj_t *PAGE_MAIN_label_67;
	lv_obj_t *PAGE_MAIN_FAN_SET_INPUT;
	lv_obj_t *PAGE_MAIN_cont_2;
	lv_obj_t *PAGE_MAIN_label_12;
	lv_obj_t *PAGE_MAIN_label_11;
	lv_obj_t *PAGE_MAIN_label_10;
	lv_obj_t *PAGE_MAIN_VOUT_LABEL;
	lv_obj_t *PAGE_MAIN_IOUT_LABEL;
	lv_obj_t *PAGE_MAIN_POUT_LABEL;
	lv_obj_t *PAGE_MAIN_label_22;
	lv_obj_t *PAGE_MAIN_label_23;
	lv_obj_t *PAGE_MAIN_label_24;
	lv_obj_t *PAGE_MAIN_cont_6;
	lv_obj_t *PAGE_MAIN_label_42;
	lv_obj_t *PAGE_MAIN_label_41;
	lv_obj_t *PAGE_MAIN_label_40;
	lv_obj_t *PAGE_MAIN_CORETEMP_LABEL;
	lv_obj_t *PAGE_MAIN_TEMP2_LABEL;
	lv_obj_t *PAGE_MAIN_TEMP1_LABEL;
	lv_obj_t *PAGE_MAIN_label_45;
	lv_obj_t *PAGE_MAIN_img_1;
	lv_obj_t *PAGE_MAIN_cont_5;
	lv_obj_t *PAGE_MAIN_MODE_LABEL;
	lv_obj_t *PAGE_MAIN_label_30;
	lv_obj_t *PAGE_MAIN_label_31;
	lv_obj_t *PAGE_MAIN_label_32;
	lv_obj_t *PAGE_MAIN_label_33;
	lv_obj_t *PAGE_MAIN_TOPO_LABEL;
	lv_obj_t *PAGE_MAIN_FAULT_LABEL;
	lv_obj_t *PAGE_MAIN_FSM_LABEL;
	lv_obj_t *PAGE_MAIN_cont_9;
	lv_obj_t *PAGE_MAIN_label_76;
	lv_obj_t *PAGE_MAIN_label_75;
	lv_obj_t *PAGE_MAIN_label_74;
	lv_obj_t *PAGE_MAIN_PC_CTRL_LABEL;
	lv_obj_t *PAGE_MAIN_CONNECT_TO_PC_LABEL;
	lv_obj_t *PAGE_MAIN_label_70;
	lv_obj_t *PAGE_MAIN_img_2;
	lv_obj_t *PAGE_MAIN_SWITCH_BLE;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, int32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                       uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                       lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_ui(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui guider_ui;


void setup_scr_PAGE_MAIN(lv_ui *ui);
LV_IMG_DECLARE(_ic_fluent_temperature_48_regular_alpha_24x24);
LV_IMG_DECLARE(_ic_fluent_bluetooth_48_regular_alpha_24x24);

LV_FONT_DECLARE(lv_font_blender_32)
LV_FONT_DECLARE(lv_font_blender_24)
LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_blender_16)
LV_FONT_DECLARE(lv_font_blender_14)


#ifdef __cplusplus
}
#endif
#endif
