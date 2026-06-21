/**
  ******************************************************************************
  * @file    gui_power_app.c
  * @author  UF4
  * @date    26-6-20 下午6:22
  * @brief   UF4GUI digital power supply application interface.
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
#include "gui.h"

#include <stdio.h>

enum {
    PAGE_HOME = 1,
    PAGE_PARAM,
    PAGE_WAVE,
    PAGE_INFO
};

static GUI_Widget g_home_root;
static GUI_Widget g_title;
static GUI_Widget g_voltage;
static GUI_Widget g_current;
static GUI_Widget g_power;
static GUI_Widget g_temp;
static GUI_Widget g_mode;
static GUI_Widget g_wave;
static GUI_Widget g_output_sw;
static GUI_Widget g_param_btn;
static GUI_Widget g_wave_btn;
static GUI_Widget g_info_btn;

static GUI_Widget g_param_root;
static GUI_Widget g_param_window;
static GUI_Widget g_param_menu;
static GUI_Widget g_set_voltage_slider;
static GUI_Widget g_set_current_slider;

static GUI_Widget g_wave_root;
static GUI_Widget g_wave_full;

static GUI_Widget g_info_root;
static GUI_Widget g_info_window;
static GUI_Widget g_info_label;

static char g_voltage_text[24];
static char g_current_text[24];
static char g_power_text[24];
static char g_temp_text[24];
static char g_mode_text[12];

static void PowerUi_DrawPanel(const GUI_Rect *rect, const char *title, GUI_Color accent)
{
    const GUI_Theme *theme = GUI_GetTheme();
    GUI_Rect title_bar = {rect->x, rect->y, rect->w, 18};
    GUI_Rect accent_line = {rect->x, rect->y, 4, rect->h};

    GUI_FillRect(rect, theme->panel);
    GUI_FillRect(&title_bar, GUI_Blend565(theme->panel, theme->bg, 150U));
    GUI_FillRect(&accent_line, accent);
    GUI_DrawRect(rect, theme->border);
    GUI_DrawString((int16_t)(rect->x + 10), (int16_t)(rect->y + 5), title, theme->muted, 1U);
}

static void PowerUi_GoParamPage(GUI_Widget *w)
{
    (void)w;
    (void)GUI_Page_Switch(PAGE_PARAM);
}

static void PowerUi_GoWavePage(GUI_Widget *w)
{
    (void)w;
    (void)GUI_Page_Switch(PAGE_WAVE);
}

static void PowerUi_GoInfoPage(GUI_Widget *w)
{
    (void)w;
    (void)GUI_Page_Switch(PAGE_INFO);
}

static void PowerUi_DrawBackground(const GUI_Rect *clip)
{
    const GUI_Theme *theme = GUI_GetTheme();
    GUI_Rect top_bar = {0, 0, GUI_SCREEN_WIDTH, 46};
    GUI_Rect top_line = {0, 44, GUI_SCREEN_WIDTH, 2};
    GUI_Rect left_rail = {0, 46, 5, (int16_t)(GUI_SCREEN_HEIGHT - 46)};

    (void)clip;
    GUI_FillRect(&(GUI_Rect){0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT}, theme->bg);
    GUI_FillRect(&top_bar, theme->panel);
    GUI_FillRect(&top_line, theme->accent);
    GUI_FillRect(&left_rail, theme->accent);
    GUI_DrawString(18, 6, "UF4 DIGITAL POWER", theme->text, 2U);
    GUI_DrawString(490, 10, "REMOTE  SENSE", theme->muted, 1U);

    PowerUi_DrawPanel(&(GUI_Rect){16, 58, 214, 88}, "OUTPUT VOLTAGE", theme->accent);
    PowerUi_DrawPanel(&(GUI_Rect){244, 58, 214, 88}, "OUTPUT CURRENT", theme->accent2);
    PowerUi_DrawPanel(&(GUI_Rect){472, 58, 152, 88}, "OUTPUT STATE", theme->success);
    PowerUi_DrawPanel(&(GUI_Rect){16, 158, 214, 64}, "OUTPUT POWER", theme->warning);
    PowerUi_DrawPanel(&(GUI_Rect){244, 158, 214, 64}, "TEMPERATURE", theme->danger);
    PowerUi_DrawPanel(&(GUI_Rect){472, 158, 152, 64}, "CONTROL", theme->accent);
    PowerUi_DrawPanel(&(GUI_Rect){16, 236, 608, 214}, "REALTIME WAVEFORM", theme->accent2);
}

static void PowerUi_HomeEnter(void)
{
    GUI_InvalidateRect(&(GUI_Rect){0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT});
}

static void PowerUi_HomeUpdate(uint32_t elapsed_ms)
{
    static uint32_t acc;

    acc += elapsed_ms;
    if (acc >= 30U) {
        acc = 0U;
        GUI_InvalidateWidget(&g_wave);
    }
}

static GUI_Page g_pages[] = {
    {PAGE_HOME, "Home", &g_home_root, PowerUi_HomeEnter, 0, PowerUi_HomeUpdate, PowerUi_DrawBackground},
    {PAGE_PARAM, "Parameters", &g_param_root, 0, 0, 0, PowerUi_DrawBackground},
    {PAGE_WAVE, "Wave", &g_wave_root, 0, 0, 0, PowerUi_DrawBackground},
    {PAGE_INFO, "System", &g_info_root, 0, 0, 0, PowerUi_DrawBackground}
};

/**
 * @brief  Initializes the digital power supply application UI.
 * @retval None
 */
void GUI_PowerApp_Init(void)
{
    static const char *menu_items[] = {"Voltage", "Current", "Protection", "Back"};

    GUI_Init();
    GUI_SetTheme(GUI_THEME_INDUSTRIAL);

    GUI_Widget_Init(&g_home_root, GUI_WIDGET_WINDOW, &(GUI_Rect){0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT});
    g_home_root.draw = 0;
    GUI_Label_Create(&g_title, &(GUI_Rect){18, 30, 160, 12}, "H743 RGB565 DMA2D");
    g_title.data.label.color = GUI_GetTheme()->muted;
    g_title.data.label.scale = 1U;
    GUI_Label_Create(&g_voltage, &(GUI_Rect){34, 88, 178, 34}, g_voltage_text);
    g_voltage.data.label.scale = 3U;
    g_voltage.data.label.color = GUI_GetTheme()->accent;
    GUI_Label_Create(&g_current, &(GUI_Rect){262, 88, 178, 34}, g_current_text);
    g_current.data.label.scale = 3U;
    g_current.data.label.color = GUI_GetTheme()->accent2;
    GUI_Label_Create(&g_power, &(GUI_Rect){34, 184, 172, 26}, g_power_text);
    g_power.data.label.scale = 2U;
    g_power.data.label.color = GUI_GetTheme()->warning;
    GUI_Label_Create(&g_temp, &(GUI_Rect){262, 184, 172, 26}, g_temp_text);
    g_temp.data.label.scale = 2U;
    g_temp.data.label.color = GUI_GetTheme()->danger;
    GUI_Label_Create(&g_mode, &(GUI_Rect){512, 90, 72, 22}, g_mode_text);
    g_mode.data.label.color = GUI_GetTheme()->success;
    g_mode.data.label.scale = 3U;
    GUI_WaveView_Create(&g_wave, &(GUI_Rect){24, 262, 592, 176}, 0, 500);
    GUI_Switch_Create(&g_output_sw, &(GUI_Rect){500, 184, 96, 28}, 0U);
    GUI_Button_Create(&g_param_btn, &(GUI_Rect){476, 232, 70, 28}, "SET", PowerUi_GoParamPage);
    GUI_Button_Create(&g_wave_btn, &(GUI_Rect){550, 232, 70, 28}, "WAVE", PowerUi_GoWavePage);
    GUI_Button_Create(&g_info_btn, &(GUI_Rect){550, 12, 70, 26}, "INFO", PowerUi_GoInfoPage);
    GUI_Widget_AddChild(&g_home_root, &g_title);
    GUI_Widget_AddChild(&g_home_root, &g_voltage);
    GUI_Widget_AddChild(&g_home_root, &g_current);
    GUI_Widget_AddChild(&g_home_root, &g_power);
    GUI_Widget_AddChild(&g_home_root, &g_temp);
    GUI_Widget_AddChild(&g_home_root, &g_mode);
    GUI_Widget_AddChild(&g_home_root, &g_wave);
    GUI_Widget_AddChild(&g_home_root, &g_output_sw);
    GUI_Widget_AddChild(&g_home_root, &g_param_btn);
    GUI_Widget_AddChild(&g_home_root, &g_wave_btn);
    GUI_Widget_AddChild(&g_home_root, &g_info_btn);

    GUI_Widget_Init(&g_param_root, GUI_WIDGET_WINDOW, &(GUI_Rect){0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT});
    g_param_root.draw = 0;
    GUI_Window_Create(&g_param_window, &(GUI_Rect){20, 24, 600, 420}, "PARAMETER SETUP");
    GUI_Menu_Create(&g_param_menu, &(GUI_Rect){38, 72, 160, 260}, menu_items, 4U);
    GUI_Slider_Create(&g_set_voltage_slider, &(GUI_Rect){230, 110, 330, 36}, 0, 600, 120);
    GUI_Slider_Create(&g_set_current_slider, &(GUI_Rect){230, 190, 330, 36}, 0, 300, 50);
    GUI_Widget_AddChild(&g_param_root, &g_param_window);
    GUI_Widget_AddChild(&g_param_root, &g_param_menu);
    GUI_Widget_AddChild(&g_param_root, &g_set_voltage_slider);
    GUI_Widget_AddChild(&g_param_root, &g_set_current_slider);

    GUI_Widget_Init(&g_wave_root, GUI_WIDGET_WINDOW, &(GUI_Rect){0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT});
    g_wave_root.draw = 0;
    GUI_WaveView_Create(&g_wave_full, &(GUI_Rect){18, 36, 604, 400}, 0, 500);
    GUI_Widget_AddChild(&g_wave_root, &g_wave_full);

    GUI_Widget_Init(&g_info_root, GUI_WIDGET_WINDOW, &(GUI_Rect){0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT});
    g_info_root.draw = 0;
    GUI_Window_Create(&g_info_window, &(GUI_Rect){40, 50, 560, 360}, "SYSTEM INFO");
    GUI_Label_Create(&g_info_label, &(GUI_Rect){70, 120, 460, 32}, "STM32H743ZI  RGB565  UF4GUI");
    g_info_label.data.label.scale = 2U;
    GUI_Widget_AddChild(&g_info_root, &g_info_window);
    GUI_Widget_AddChild(&g_info_root, &g_info_label);

    (void)snprintf(g_voltage_text, sizeof(g_voltage_text), "00.00V");
    (void)snprintf(g_current_text, sizeof(g_current_text), "0.000A");
    (void)snprintf(g_power_text, sizeof(g_power_text), "P 000.0W");
    (void)snprintf(g_temp_text, sizeof(g_temp_text), "T 25.0C");
    (void)snprintf(g_mode_text, sizeof(g_mode_text), "CV");

    (void)GUI_Page_Register(&g_pages[0]);
    (void)GUI_Page_Register(&g_pages[1]);
    (void)GUI_Page_Register(&g_pages[2]);
    (void)GUI_Page_Register(&g_pages[3]);
    (void)GUI_Page_Switch(PAGE_HOME);
}

/**
  * @brief  Updates measured values shown by the digital power supply UI.
  * @param  mv Output voltage in millivolts.
  * @param  ma Output current in milliamps.
  * @param  mw Output power in milliwatts.
  * @param  temp_c10 Temperature in 0.1 degree Celsius.
  * @param  cc_mode Constant-current state flag.
  * @retval None
  */
void GUI_PowerApp_Update(int32_t mv, int32_t ma, int32_t mw, int32_t temp_c10, uint8_t cc_mode)
{
    (void)snprintf(g_voltage_text, sizeof(g_voltage_text), "%02ld.%02ldV", mv / 1000L, (mv % 1000L) / 10L);
    (void)snprintf(g_current_text, sizeof(g_current_text), "%ld.%03ldA", ma / 1000L, ma % 1000L);
    (void)snprintf(g_power_text, sizeof(g_power_text), "P %03ld.%01ldW", mw / 1000L, (mw % 1000L) / 100L);
    (void)snprintf(g_temp_text, sizeof(g_temp_text), "T %ld.%ldC", temp_c10 / 10L, temp_c10 % 10L);
    (void)snprintf(g_mode_text, sizeof(g_mode_text), "%s", cc_mode ? "CC" : "CV");

    GUI_WaveView_Push(&g_wave, (int16_t)(mv / 100), (int16_t)(ma / 20), (int16_t)(temp_c10 / 2),
                      (int16_t)(mw / 200));
    GUI_WaveView_Push(&g_wave_full, (int16_t)(mv / 100), (int16_t)(ma / 20), (int16_t)(temp_c10 / 2),
                      (int16_t)(mw / 200));
    GUI_InvalidateWidget(&g_voltage);
    GUI_InvalidateWidget(&g_current);
    GUI_InvalidateWidget(&g_power);
    GUI_InvalidateWidget(&g_temp);
    GUI_InvalidateWidget(&g_mode);
}
