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

static void demo_go_param(GUI_Widget *w)
{
    (void)w;
    (void)GUI_Page_Switch(PAGE_PARAM);
}

static void demo_page_bg(const GUI_Rect *clip)
{
    (void)clip;
    GUI_FillRect(&(GUI_Rect){0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT}, GUI_GetTheme()->bg);
}

static void demo_home_enter(void)
{
    GUI_InvalidateRect(&(GUI_Rect){0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT});
}

static void demo_home_update(uint32_t elapsed_ms)
{
    static uint32_t acc;
    acc += elapsed_ms;
    if (acc >= 80U) {
        acc = 0U;
        GUI_InvalidateWidget(&g_wave);
    }
}

static GUI_Page g_pages[] = {
    {PAGE_HOME, "Home", &g_home_root, demo_home_enter, 0, demo_home_update, demo_page_bg},
    {PAGE_PARAM, "Parameters", &g_param_root, 0, 0, 0, demo_page_bg},
    {PAGE_WAVE, "Wave", &g_wave_root, 0, 0, 0, demo_page_bg},
    {PAGE_INFO, "System", &g_info_root, 0, 0, 0, demo_page_bg}
};

void GUI_DemoPower_Init(void)
{
    static const char *menu_items[] = {"Voltage", "Current", "Protection", "Back"};

    GUI_Init();
    GUI_SetTheme(GUI_THEME_INDUSTRIAL);

    GUI_Widget_Init(&g_home_root, GUI_WIDGET_WINDOW, &(GUI_Rect){0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT});
    g_home_root.draw = 0;
    GUI_Label_Create(&g_title, &(GUI_Rect){16, 12, 220, 24}, "UF4 DIGITAL POWER");
    g_title.data.label.scale = 2U;
    GUI_Label_Create(&g_voltage, &(GUI_Rect){20, 64, 190, 44}, g_voltage_text);
    g_voltage.data.label.scale = 3U;
    GUI_Label_Create(&g_current, &(GUI_Rect){240, 64, 190, 44}, g_current_text);
    g_current.data.label.scale = 3U;
    GUI_Label_Create(&g_power, &(GUI_Rect){20, 132, 190, 36}, g_power_text);
    g_power.data.label.scale = 2U;
    GUI_Label_Create(&g_temp, &(GUI_Rect){240, 132, 190, 36}, g_temp_text);
    g_temp.data.label.scale = 2U;
    GUI_Label_Create(&g_mode, &(GUI_Rect){500, 20, 70, 22}, g_mode_text);
    g_mode.data.label.color = GUI_GetTheme()->accent;
    g_mode.data.label.scale = 2U;
    GUI_WaveView_Create(&g_wave, &(GUI_Rect){20, 200, 600, 190}, 0, 500);
    GUI_Switch_Create(&g_output_sw, &(GUI_Rect){500, 72, 92, 34}, 0U);
    GUI_Button_Create(&g_param_btn, &(GUI_Rect){470, 126, 132, 42}, "PARAM", demo_go_param);
    GUI_Widget_AddChild(&g_home_root, &g_title);
    GUI_Widget_AddChild(&g_home_root, &g_voltage);
    GUI_Widget_AddChild(&g_home_root, &g_current);
    GUI_Widget_AddChild(&g_home_root, &g_power);
    GUI_Widget_AddChild(&g_home_root, &g_temp);
    GUI_Widget_AddChild(&g_home_root, &g_mode);
    GUI_Widget_AddChild(&g_home_root, &g_wave);
    GUI_Widget_AddChild(&g_home_root, &g_output_sw);
    GUI_Widget_AddChild(&g_home_root, &g_param_btn);

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

void GUI_DemoPower_Update(int32_t mv, int32_t ma, int32_t mw, int32_t temp_c10, uint8_t cc_mode)
{
    (void)snprintf(g_voltage_text, sizeof(g_voltage_text), "%02ld.%02ldV", mv / 1000L, (mv % 1000L) / 10L);
    (void)snprintf(g_current_text, sizeof(g_current_text), "%ld.%03ldA", ma / 1000L, ma % 1000L);
    (void)snprintf(g_power_text, sizeof(g_power_text), "P %03ld.%01ldW", mw / 1000L, (mw % 1000L) / 100L);
    (void)snprintf(g_temp_text, sizeof(g_temp_text), "T %ld.%ldC", temp_c10 / 10L, temp_c10 % 10L);
    (void)snprintf(g_mode_text, sizeof(g_mode_text), "%s", cc_mode ? "CC" : "CV");

    GUI_WaveView_Push(&g_wave, (int16_t)(mv / 100), (int16_t)(ma / 20), (int16_t)(temp_c10 / 2), (int16_t)(mw / 200));
    GUI_WaveView_Push(&g_wave_full, (int16_t)(mv / 100), (int16_t)(ma / 20), (int16_t)(temp_c10 / 2), (int16_t)(mw / 200));
    GUI_InvalidateWidget(&g_voltage);
    GUI_InvalidateWidget(&g_current);
    GUI_InvalidateWidget(&g_power);
    GUI_InvalidateWidget(&g_temp);
    GUI_InvalidateWidget(&g_mode);
}
