#include "gui_internal.h"

#include "bsp_lcd.h"

#include <stdio.h>

#define GUI_SCOPE_SAMPLE_COUNT       256U
#define GUI_SCOPE_GRID_X             40U
#define GUI_SCOPE_GRID_Y             78U
#define GUI_SCOPE_GRID_W             560U
#define GUI_SCOPE_GRID_H             272U
#define GUI_SCOPE_DIGITAL_Y          358U
#define GUI_SCOPE_DIGITAL_H          38U
#define GUI_SCOPE_HEADER_Y           4U
#define GUI_SCOPE_HEADER_H           54U
#define GUI_SCOPE_MEASURE_Y          402U
#define GUI_SCOPE_MEASURE_H          34U
#define GUI_SCOPE_MENU_Y             440U
#define GUI_SCOPE_MENU_H             36U
#define GUI_SCOPE_GRID_DIV_X         10U
#define GUI_SCOPE_GRID_DIV_Y         8U

#define GUI_SCOPE_FIELD_VSET         0U
#define GUI_SCOPE_FIELD_ISET         1U
#define GUI_SCOPE_FIELD_CH1          2U
#define GUI_SCOPE_FIELD_CH2          3U
#define GUI_SCOPE_FIELD_Y1           4U
#define GUI_SCOPE_FIELD_Y2           5U
#define GUI_SCOPE_FIELD_TIME         6U
#define GUI_SCOPE_FIELD_TRIG         7U
#define GUI_SCOPE_FIELD_HOLD         8U
#define GUI_SCOPE_FIELD_BACK         9U

#define GUI_SCOPE_TRIGGER_AUTO       0U
#define GUI_SCOPE_TRIGGER_FAULT      1U
#define GUI_SCOPE_TRIGGER_STATE      2U

#define GUI_SCOPE_SOURCE_VOUT        0U
#define GUI_SCOPE_SOURCE_IOUT        1U
#define GUI_SCOPE_SOURCE_POUT        2U
#define GUI_SCOPE_SOURCE_VIN         3U
#define GUI_SCOPE_SOURCE_IIN         4U
#define GUI_SCOPE_SOURCE_PIN         5U
#define GUI_SCOPE_SOURCE_VSET        6U
#define GUI_SCOPE_SOURCE_ISET        7U
#define GUI_SCOPE_SOURCE_EFF         8U
#define GUI_SCOPE_SOURCE_FAN         9U

#define GUI_SCOPE_GRID_COLOR         0x2945U
#define GUI_SCOPE_GRID_MAJOR_COLOR   0x4208U
#define GUI_SCOPE_CH1_COLOR          GUI_ACCENT_COLOR
#define GUI_SCOPE_CH2_COLOR          0xFD20U
#define GUI_SCOPE_STATE_COLOR        GUI_OK_COLOR
#define GUI_SCOPE_FAULT_COLOR        GUI_WARN_COLOR

typedef struct
{
    float vin;
    float iin;
    float pin;
    float vout;
    float iout;
    float pout;
    float vset;
    float iset;
    float efficiency;
    float fan;
    uint8_t state;
    uint8_t fault;
} GUI_ScopeSample_t;

static GUI_ScopeSample_t g_scope_samples[GUI_SCOPE_SAMPLE_COUNT];
static uint16_t g_scope_head;
static uint16_t g_scope_count;

static void GUI_ScopePushSample(const GUI_Data_t *data);
static const GUI_ScopeSample_t *GUI_ScopeGetSample(uint16_t display_index);
static const GUI_ScopeSample_t *GUI_ScopeGetVisibleSample(uint8_t timebase, uint16_t display_index);
static uint16_t GUI_ScopeVisibleCount(uint8_t timebase);
static void GUI_DrawScopeHeader(const GUI_Data_t *data);
static void GUI_DrawScopeHeaderTile(const GUI_Rect_t *rect, const char *title, const char *value, uint16_t fill_color, uint8_t selected);
static void GUI_DrawScopeGrid(const GUI_Data_t *data);
static void GUI_DrawScopeAnalogTrace(const GUI_Data_t *data, uint8_t channel, uint16_t color);
static void GUI_DrawScopeDigitalTracks(const GUI_Data_t *data);
static void GUI_DrawScopeMeasurements(const GUI_Data_t *data);
static void GUI_DrawScopeMenu(const GUI_Data_t *data);
static void GUI_DrawScopeMenuTile(const GUI_Rect_t *rect, const char *title, const char *value, uint16_t fill_color, uint8_t selected);
static void GUI_DrawScopeLegend(uint16_t x, uint16_t y, const char *name, uint16_t color, float value, const char *unit, uint8_t selected);
static void GUI_DrawScopeLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
static uint16_t GUI_ScopeMapAnalogY(float value, float full_scale);
static uint16_t GUI_ScopeMapX(uint8_t timebase, uint16_t display_index);
static uint16_t GUI_ScopeNextIndex(uint16_t index);
static float GUI_ScopeSampleValue(const GUI_ScopeSample_t *sample, uint8_t source);
static float GUI_ScopeCurrentValue(const GUI_Data_t *data, uint8_t source);
static float GUI_ScopeFullScale(uint8_t source, uint8_t scale);
static const char *GUI_ScopeSourceText(uint8_t source);
static const char *GUI_ScopeSourceUnit(uint8_t source);
static const char *GUI_ScopeScaleText(uint8_t source, uint8_t scale);
static const char *GUI_ScopeTimebaseText(uint8_t timebase);
static const char *GUI_ScopeTriggerText(uint8_t trigger);
static const char *GUI_ScopeTriggerShortText(uint8_t trigger);
static uint8_t GUI_ScopeSelected(const GUI_Data_t *data, uint8_t field);
static float GUI_ScopeAbs(float value);

void GUI_DrawWavePage(const GUI_Data_t *data)
{
    if (data == NULL)
    {
        return;
    }

    if (data->scope_hold == 0U)
    {
        GUI_ScopePushSample(data);
    }

    GUI_DrawScopeHeader(data);
    GUI_DrawScopeGrid(data);
    if (data->scope_ch1_enabled != 0U)
    {
        GUI_DrawScopeAnalogTrace(data, 0U, GUI_SCOPE_CH1_COLOR);
    }
    if (data->scope_ch2_enabled != 0U)
    {
        GUI_DrawScopeAnalogTrace(data, 1U, GUI_SCOPE_CH2_COLOR);
    }
    if (data->scope_ch1_enabled == 0U && data->scope_ch2_enabled == 0U)
    {
        GUI_DrawCenteredText(&(GUI_Rect_t){GUI_SCOPE_GRID_X, 190U, GUI_SCOPE_GRID_W, 24U},
                             "CH1/CH2 OFF",
                             GUI_FONT_TILE_VALUE,
                             GUI_MUTED_COLOR,
                             GUI_PANEL_DARK);
    }
    GUI_DrawScopeDigitalTracks(data);
    GUI_DrawScopeMeasurements(data);
    GUI_DrawScopeMenu(data);
}

/**
  * @brief Append one GUI telemetry sample to the scope history ring.
  */
static void GUI_ScopePushSample(const GUI_Data_t *data)
{
    g_scope_samples[g_scope_head].vin = data->vin;
    g_scope_samples[g_scope_head].iin = data->iin;
    g_scope_samples[g_scope_head].pin = data->pin;
    g_scope_samples[g_scope_head].vout = data->vout;
    g_scope_samples[g_scope_head].iout = data->iout;
    g_scope_samples[g_scope_head].pout = data->pout;
    g_scope_samples[g_scope_head].vset = data->vset;
    g_scope_samples[g_scope_head].iset = data->iset;
    g_scope_samples[g_scope_head].efficiency = data->efficiency;
    g_scope_samples[g_scope_head].fan = data->fan;
    g_scope_samples[g_scope_head].state = data->state_machine_state;
    g_scope_samples[g_scope_head].fault = data->fault_state;

    g_scope_head = GUI_ScopeNextIndex(g_scope_head);
    if (g_scope_count < GUI_SCOPE_SAMPLE_COUNT)
    {
        ++g_scope_count;
    }
}

/**
  * @brief Return sample in oldest-to-newest display order.
  */
static const GUI_ScopeSample_t *GUI_ScopeGetSample(uint16_t display_index)
{
    uint16_t oldest;
    uint16_t index;

    if (display_index >= g_scope_count)
    {
        return NULL;
    }

    oldest = (g_scope_count < GUI_SCOPE_SAMPLE_COUNT)
        ? 0U
        : g_scope_head;
    index = (uint16_t)(oldest + display_index);
    if (index >= GUI_SCOPE_SAMPLE_COUNT)
    {
        index = (uint16_t)(index - GUI_SCOPE_SAMPLE_COUNT);
    }

    return &g_scope_samples[index];
}

/**
  * @brief Return a sample from the active timebase window.
  */
static const GUI_ScopeSample_t *GUI_ScopeGetVisibleSample(uint8_t timebase, uint16_t display_index)
{
    uint16_t visible_count = GUI_ScopeVisibleCount(timebase);
    uint16_t start_index;

    if (display_index >= visible_count)
    {
        return NULL;
    }

    start_index = (uint16_t)(g_scope_count - visible_count);
    return GUI_ScopeGetSample((uint16_t)(start_index + display_index));
}

/**
  * @brief Convert timebase selection to the number of samples shown on screen.
  */
static uint16_t GUI_ScopeVisibleCount(uint8_t timebase)
{
    uint16_t target_count;

    switch (timebase)
    {
        case 0U: target_count = 64U; break;
        case 1U: target_count = 96U; break;
        case 2U: target_count = 128U; break;
        case 3U: target_count = 192U; break;
        default: target_count = GUI_SCOPE_SAMPLE_COUNT; break;
    }

    return (g_scope_count < target_count) ? g_scope_count : target_count;
}

/**
  * @brief Draw compact run, timebase and channel information.
  */
static void GUI_DrawScopeHeader(const GUI_Data_t *data)
{
    char buf[32];
    char vset_buf[16];
    char iset_buf[16];
    GUI_Rect_t header_rect = {4U, GUI_SCOPE_HEADER_Y, 632U, GUI_SCOPE_HEADER_H};
    GUI_Rect_t title_tile = {8U, 9U, 78U, 44U};
    GUI_Rect_t vset_tile = {92U, 9U, 112U, 44U};
    GUI_Rect_t iset_tile = {210U, 9U, 112U, 44U};
    GUI_Rect_t time_tile = {328U, 9U, 76U, 44U};
    GUI_Rect_t trig_tile = {410U, 9U, 86U, 44U};
    GUI_Rect_t hold_tile = {502U, 9U, 62U, 44U};
    GUI_Rect_t back_tile = {570U, 9U, 62U, 44U};

    GUI_DrawPanel(&header_rect, GUI_PANEL_DARK);

    snprintf(vset_buf, sizeof(vset_buf), "%.2fV", data->vset);
    snprintf(iset_buf, sizeof(iset_buf), "%.2fA", data->iset);

    GUI_DrawScopeHeaderTile(&title_tile, "SCOPE", data->scope_hold != 0U ? "MEM" : "LIVE", GUI_PANEL_COLOR, 0U);
    GUI_DrawScopeHeaderTile(&vset_tile, "VSET", vset_buf, GUI_PANEL_COLOR, GUI_ScopeSelected(data, GUI_SCOPE_FIELD_VSET));
    GUI_DrawScopeHeaderTile(&iset_tile, "ISET", iset_buf, GUI_PANEL_COLOR, GUI_ScopeSelected(data, GUI_SCOPE_FIELD_ISET));
    GUI_DrawScopeHeaderTile(&time_tile,
                            "TIME",
                            GUI_ScopeTimebaseText(data->scope_timebase),
                            GUI_PANEL_COLOR,
                            GUI_ScopeSelected(data, GUI_SCOPE_FIELD_TIME));
    GUI_DrawScopeHeaderTile(&trig_tile,
                            "TRIG",
                            GUI_ScopeTriggerText(data->scope_trigger),
                            data->scope_trigger == GUI_SCOPE_TRIGGER_FAULT ? GUI_WARN_COLOR : GUI_PANEL_COLOR,
                            GUI_ScopeSelected(data, GUI_SCOPE_FIELD_TRIG));
    GUI_DrawScopeHeaderTile(&hold_tile,
                            "HOLD",
                            data->scope_hold != 0U ? "ON" : "OFF",
                            data->scope_hold != 0U ? GUI_WARN_COLOR : GUI_PANEL_COLOR,
                            GUI_ScopeSelected(data, GUI_SCOPE_FIELD_HOLD));
    GUI_DrawScopeHeaderTile(&back_tile, "BACK", "M", GUI_PANEL_COLOR, GUI_ScopeSelected(data, GUI_SCOPE_FIELD_BACK));

    if (data->scope_ch1_enabled != 0U)
    {
        GUI_DrawScopeLegend(42U,
                            58U,
                            GUI_ScopeSourceText(data->scope_ch1_source),
                            GUI_SCOPE_CH1_COLOR,
                            GUI_ScopeCurrentValue(data, data->scope_ch1_source),
                            GUI_ScopeSourceUnit(data->scope_ch1_source),
                            GUI_ScopeSelected(data, GUI_SCOPE_FIELD_CH1));
    }
    else
    {
        GUI_DrawCenteredText(&(GUI_Rect_t){42U, 58U, 136U, 18U}, "CH1 OFF", GUI_FONT_TOP_LABEL, GUI_MUTED_COLOR, GUI_BG_COLOR);
    }

    if (data->scope_ch2_enabled != 0U)
    {
        GUI_DrawScopeLegend(212U,
                            58U,
                            GUI_ScopeSourceText(data->scope_ch2_source),
                            GUI_SCOPE_CH2_COLOR,
                            GUI_ScopeCurrentValue(data, data->scope_ch2_source),
                            GUI_ScopeSourceUnit(data->scope_ch2_source),
                            GUI_ScopeSelected(data, GUI_SCOPE_FIELD_CH2));
    }
    else
    {
        GUI_DrawCenteredText(&(GUI_Rect_t){212U, 58U, 136U, 18U}, "CH2 OFF", GUI_FONT_TOP_LABEL, GUI_MUTED_COLOR, GUI_BG_COLOR);
    }

    snprintf(buf, sizeof(buf), "STATE %s", (data->valid_flags & GUI_VALID_STATE_MACHINE_STATE) != 0U ? GUI_StateText(data->state_machine_state) : "NA");
    GUI_DrawCenteredText(&(GUI_Rect_t){382U, 58U, 94U, 18U}, buf, GUI_FONT_TOP_LABEL, GUI_MUTED_COLOR, GUI_BG_COLOR);

    snprintf(buf, sizeof(buf), "FAULT %s", (data->valid_flags & GUI_VALID_FAULT_STATE) != 0U ? GUI_FaultText(data->fault_state) : "NA");
    GUI_DrawCenteredText(&(GUI_Rect_t){500U, 58U, 100U, 18U}, buf, GUI_FONT_TOP_LABEL, data->fault_state != 0U ? GUI_WARN_COLOR : GUI_MUTED_COLOR, GUI_BG_COLOR);
}

/**
  * @brief Draw a compact two-line header tile for narrow scope controls.
  */
static void GUI_DrawScopeHeaderTile(const GUI_Rect_t *rect, const char *title, const char *value, uint16_t fill_color, uint8_t selected)
{
    GUI_Rect_t title_rect = {rect->x, (uint16_t)(rect->y + 3U), rect->w, 18U};
    GUI_Rect_t value_rect = {rect->x, (uint16_t)(rect->y + 20U), rect->w, 22U};

    GUI_DrawPanel(rect, fill_color);
    GUI_DrawCenteredText(&title_rect, title, GUI_FONT_TOP_LABEL, GUI_MUTED_COLOR, fill_color);
    GUI_DrawCenteredText(&value_rect, value, GUI_FONT_TOP_LABEL, GUI_TEXT_COLOR, fill_color);
    if (selected != 0U)
    {
        GUI_DrawPanelBorder(rect, GUI_ACCENT_COLOR);
    }
}

/**
  * @brief Draw scope panel, grid and axis labels.
  */
static void GUI_DrawScopeGrid(const GUI_Data_t *data)
{
    uint16_t i;
    char buf[16];
    GUI_Rect_t grid_rect = {GUI_SCOPE_GRID_X, GUI_SCOPE_GRID_Y, GUI_SCOPE_GRID_W, GUI_SCOPE_GRID_H};
    GUI_Rect_t digital_rect = {GUI_SCOPE_GRID_X, GUI_SCOPE_DIGITAL_Y, GUI_SCOPE_GRID_W, GUI_SCOPE_DIGITAL_H};
    float ch1_fs = GUI_ScopeFullScale(data->scope_ch1_source, data->scope_ch1_scale);
    float ch2_fs = GUI_ScopeFullScale(data->scope_ch2_source, data->scope_ch2_scale);

    GUI_DrawPanel(&grid_rect, GUI_PANEL_DARK);
    GUI_DrawPanel(&digital_rect, GUI_PANEL_DARK);

    for (i = 1U; i < GUI_SCOPE_GRID_DIV_X; ++i)
    {
        uint16_t x = (uint16_t)(GUI_SCOPE_GRID_X + (GUI_SCOPE_GRID_W * i) / GUI_SCOPE_GRID_DIV_X);
        uint16_t color = (i == 5U) ? GUI_SCOPE_GRID_MAJOR_COLOR : GUI_SCOPE_GRID_COLOR;
        LCD_Rect_Fill(x, GUI_SCOPE_GRID_Y + 1U, x, (uint16_t)(GUI_SCOPE_GRID_Y + GUI_SCOPE_GRID_H - 2U), color);
    }

    for (i = 1U; i < GUI_SCOPE_GRID_DIV_Y; ++i)
    {
        uint16_t y = (uint16_t)(GUI_SCOPE_GRID_Y + (GUI_SCOPE_GRID_H * i) / GUI_SCOPE_GRID_DIV_Y);
        uint16_t color = (i == 4U) ? GUI_SCOPE_GRID_MAJOR_COLOR : GUI_SCOPE_GRID_COLOR;
        LCD_Rect_Fill(GUI_SCOPE_GRID_X + 1U, y, (uint16_t)(GUI_SCOPE_GRID_X + GUI_SCOPE_GRID_W - 2U), y, color);
    }

    snprintf(buf, sizeof(buf), "%.0f%s", ch1_fs, GUI_ScopeSourceUnit(data->scope_ch1_source));
    GUI_DrawCenteredText(&(GUI_Rect_t){2U, 82U, 36U, 18U}, buf, GUI_FONT_TOP_LABEL, GUI_SCOPE_CH1_COLOR, GUI_BG_COLOR);
    GUI_DrawCenteredText(&(GUI_Rect_t){2U, 210U, 36U, 18U}, "Y1", GUI_FONT_TOP_LABEL, GUI_ScopeSelected(data, GUI_SCOPE_FIELD_Y1) ? GUI_ACCENT_COLOR : GUI_MUTED_COLOR, GUI_BG_COLOR);
    GUI_DrawCenteredText(&(GUI_Rect_t){2U, 334U, 36U, 18U}, "0", GUI_FONT_TOP_LABEL, GUI_MUTED_COLOR, GUI_BG_COLOR);

    snprintf(buf, sizeof(buf), "%.0f%s", ch2_fs, GUI_ScopeSourceUnit(data->scope_ch2_source));
    GUI_DrawCenteredText(&(GUI_Rect_t){602U, 82U, 36U, 18U}, buf, GUI_FONT_TOP_LABEL, GUI_SCOPE_CH2_COLOR, GUI_BG_COLOR);
    GUI_DrawCenteredText(&(GUI_Rect_t){602U, 210U, 36U, 18U}, "Y2", GUI_FONT_TOP_LABEL, GUI_ScopeSelected(data, GUI_SCOPE_FIELD_Y2) ? GUI_ACCENT_COLOR : GUI_MUTED_COLOR, GUI_BG_COLOR);
    GUI_DrawCenteredText(&(GUI_Rect_t){602U, 334U, 36U, 18U}, "0", GUI_FONT_TOP_LABEL, GUI_MUTED_COLOR, GUI_BG_COLOR);
}

/**
  * @brief Draw one analog history trace. Channel 0 is VOUT, channel 1 is IOUT.
  */
static void GUI_DrawScopeAnalogTrace(const GUI_Data_t *data, uint8_t channel, uint16_t color)
{
    uint16_t i;
    uint16_t visible_count;
    const GUI_ScopeSample_t *last_sample;
    uint8_t source = (channel == 0U) ? data->scope_ch1_source : data->scope_ch2_source;
    uint8_t scale = (channel == 0U) ? data->scope_ch1_scale : data->scope_ch2_scale;
    float full_scale = GUI_ScopeFullScale(source, scale);
    uint16_t last_x;
    uint16_t last_y;

    visible_count = GUI_ScopeVisibleCount(data->scope_timebase);
    if (visible_count < 2U)
    {
        return;
    }

    last_sample = GUI_ScopeGetVisibleSample(data->scope_timebase, 0U);
    if (last_sample == NULL)
    {
        return;
    }

    last_x = GUI_ScopeMapX(data->scope_timebase, 0U);
    last_y = GUI_ScopeMapAnalogY(GUI_ScopeSampleValue(last_sample, source), full_scale);

    for (i = 1U; i < visible_count; ++i)
    {
        const GUI_ScopeSample_t *sample = GUI_ScopeGetVisibleSample(data->scope_timebase, i);
        uint16_t x;
        uint16_t y;

        if (sample == NULL)
        {
            break;
        }

        x = GUI_ScopeMapX(data->scope_timebase, i);
        y = GUI_ScopeMapAnalogY(GUI_ScopeSampleValue(sample, source), full_scale);
        GUI_DrawScopeLine(last_x, last_y, x, y, color);
        last_x = x;
        last_y = y;
    }
}

/**
  * @brief Draw state and fault as digital timing tracks below the analog grid.
  */
static void GUI_DrawScopeDigitalTracks(const GUI_Data_t *data)
{
    uint16_t i;
    uint16_t visible_count;
    uint16_t state_y = (uint16_t)(GUI_SCOPE_DIGITAL_Y + 12U);
    uint16_t fault_y = (uint16_t)(GUI_SCOPE_DIGITAL_Y + 27U);

    GUI_DrawCenteredText(&(GUI_Rect_t){4U, GUI_SCOPE_DIGITAL_Y + 3U, 32U, 18U}, "ST", GUI_FONT_TOP_LABEL, GUI_MUTED_COLOR, GUI_BG_COLOR);
    GUI_DrawCenteredText(&(GUI_Rect_t){4U, GUI_SCOPE_DIGITAL_Y + 20U, 32U, 18U}, "FT", GUI_FONT_TOP_LABEL, GUI_MUTED_COLOR, GUI_BG_COLOR);

    visible_count = GUI_ScopeVisibleCount(data->scope_timebase);
    if (visible_count < 2U)
    {
        return;
    }

    for (i = 1U; i < visible_count; ++i)
    {
        const GUI_ScopeSample_t *prev = GUI_ScopeGetVisibleSample(data->scope_timebase, (uint16_t)(i - 1U));
        const GUI_ScopeSample_t *sample = GUI_ScopeGetVisibleSample(data->scope_timebase, i);
        uint16_t x0 = GUI_ScopeMapX(data->scope_timebase, (uint16_t)(i - 1U));
        uint16_t x1 = GUI_ScopeMapX(data->scope_timebase, i);
        uint16_t y0;
        uint16_t y1;

        if (prev == NULL || sample == NULL)
        {
            break;
        }

        y0 = (uint16_t)(state_y - ((prev->state != 0U) ? 6U : 0U));
        y1 = (uint16_t)(state_y - ((sample->state != 0U) ? 6U : 0U));
        GUI_DrawScopeLine(x0, y0, x1, y0, GUI_SCOPE_STATE_COLOR);
        GUI_DrawScopeLine(x1, y0, x1, y1, GUI_SCOPE_STATE_COLOR);

        y0 = (uint16_t)(fault_y - ((prev->fault != 0U) ? 6U : 0U));
        y1 = (uint16_t)(fault_y - ((sample->fault != 0U) ? 6U : 0U));
        GUI_DrawScopeLine(x0, y0, x1, y0, GUI_SCOPE_FAULT_COLOR);
        GUI_DrawScopeLine(x1, y0, x1, y1, GUI_SCOPE_FAULT_COLOR);
    }
}

/**
  * @brief Draw simple min/max/ripple and communication counters.
  */
static void GUI_DrawScopeMeasurements(const GUI_Data_t *data)
{
    uint16_t i;
    uint16_t visible_count = GUI_ScopeVisibleCount(data->scope_timebase);
    float min_ch1 = GUI_ScopeCurrentValue(data, data->scope_ch1_source);
    float max_ch1 = min_ch1;
    float min_ch2 = GUI_ScopeCurrentValue(data, data->scope_ch2_source);
    float max_ch2 = min_ch2;
    char buf[40];
    GUI_Rect_t measure_rect = {4U, GUI_SCOPE_MEASURE_Y, 632U, GUI_SCOPE_MEASURE_H};

    for (i = 0U; i < visible_count; ++i)
    {
        const GUI_ScopeSample_t *sample = GUI_ScopeGetVisibleSample(data->scope_timebase, i);
        if (sample == NULL)
        {
            break;
        }

        float ch1 = GUI_ScopeSampleValue(sample, data->scope_ch1_source);
        float ch2 = GUI_ScopeSampleValue(sample, data->scope_ch2_source);

        if (ch1 < min_ch1) { min_ch1 = ch1; }
        if (ch1 > max_ch1) { max_ch1 = ch1; }
        if (ch2 < min_ch2) { min_ch2 = ch2; }
        if (ch2 > max_ch2) { max_ch2 = ch2; }
    }

    GUI_DrawPanel(&measure_rect, GUI_PANEL_DARK);

    snprintf(buf, sizeof(buf), "C1 %.2f/%.2f", min_ch1, max_ch1);
    GUI_DrawCenteredText(&(GUI_Rect_t){18U, GUI_SCOPE_MEASURE_Y + 7U, 124U, 20U}, buf, GUI_FONT_TOP_LABEL, GUI_TEXT_COLOR, GUI_PANEL_DARK);

    snprintf(buf, sizeof(buf), "R1 %.2f", GUI_ScopeAbs(max_ch1 - min_ch1));
    GUI_DrawCenteredText(&(GUI_Rect_t){154U, GUI_SCOPE_MEASURE_Y + 7U, 112U, 20U}, buf, GUI_FONT_TOP_LABEL, GUI_SCOPE_CH1_COLOR, GUI_PANEL_DARK);

    snprintf(buf, sizeof(buf), "C2 %.2f/%.2f", min_ch2, max_ch2);
    GUI_DrawCenteredText(&(GUI_Rect_t){278U, GUI_SCOPE_MEASURE_Y + 7U, 124U, 20U}, buf, GUI_FONT_TOP_LABEL, GUI_TEXT_COLOR, GUI_PANEL_DARK);

    snprintf(buf, sizeof(buf), "R2 %.2f", GUI_ScopeAbs(max_ch2 - min_ch2));
    GUI_DrawCenteredText(&(GUI_Rect_t){414U, GUI_SCOPE_MEASURE_Y + 7U, 92U, 20U}, buf, GUI_FONT_TOP_LABEL, GUI_SCOPE_CH2_COLOR, GUI_PANEL_DARK);

    snprintf(buf, sizeof(buf), "RX %lu", (unsigned long)data->comm_rx_frame_count);
    GUI_DrawCenteredText(&(GUI_Rect_t){520U, GUI_SCOPE_MEASURE_Y + 7U, 98U, 20U}, buf, GUI_FONT_TOP_LABEL, GUI_MUTED_COLOR, GUI_PANEL_DARK);
}

/**
  * @brief Draw bottom soft-key menu using the existing state tile style.
  */
static void GUI_DrawScopeMenu(const GUI_Data_t *data)
{
    GUI_Rect_t tile;

    tile = (GUI_Rect_t){4U, GUI_SCOPE_MENU_Y, 74U, GUI_SCOPE_MENU_H};
    GUI_DrawScopeMenuTile(&tile,
                          "CH1",
                          data->scope_ch1_enabled != 0U ? GUI_ScopeSourceText(data->scope_ch1_source) : "OFF",
                          data->scope_ch1_enabled != 0U ? GUI_PANEL_COLOR : GUI_PANEL_DARK,
                          GUI_ScopeSelected(data, GUI_SCOPE_FIELD_CH1));

    tile = (GUI_Rect_t){82U, GUI_SCOPE_MENU_Y, 74U, GUI_SCOPE_MENU_H};
    GUI_DrawScopeMenuTile(&tile,
                          "CH2",
                          data->scope_ch2_enabled != 0U ? GUI_ScopeSourceText(data->scope_ch2_source) : "OFF",
                          data->scope_ch2_enabled != 0U ? GUI_PANEL_COLOR : GUI_PANEL_DARK,
                          GUI_ScopeSelected(data, GUI_SCOPE_FIELD_CH2));

    tile = (GUI_Rect_t){160U, GUI_SCOPE_MENU_Y, 74U, GUI_SCOPE_MENU_H};
    GUI_DrawScopeMenuTile(&tile, "Y1", GUI_ScopeScaleText(data->scope_ch1_source, data->scope_ch1_scale), GUI_PANEL_COLOR, GUI_ScopeSelected(data, GUI_SCOPE_FIELD_Y1));

    tile = (GUI_Rect_t){238U, GUI_SCOPE_MENU_Y, 74U, GUI_SCOPE_MENU_H};
    GUI_DrawScopeMenuTile(&tile, "Y2", GUI_ScopeScaleText(data->scope_ch2_source, data->scope_ch2_scale), GUI_PANEL_COLOR, GUI_ScopeSelected(data, GUI_SCOPE_FIELD_Y2));

    tile = (GUI_Rect_t){316U, GUI_SCOPE_MENU_Y, 74U, GUI_SCOPE_MENU_H};
    GUI_DrawScopeMenuTile(&tile, "TIME", GUI_ScopeTimebaseText(data->scope_timebase), GUI_PANEL_COLOR, GUI_ScopeSelected(data, GUI_SCOPE_FIELD_TIME));

    tile = (GUI_Rect_t){394U, GUI_SCOPE_MENU_Y, 74U, GUI_SCOPE_MENU_H};
    GUI_DrawScopeMenuTile(&tile,
                          "TRIG",
                          GUI_ScopeTriggerShortText(data->scope_trigger),
                          data->scope_trigger == GUI_SCOPE_TRIGGER_FAULT ? GUI_WARN_COLOR : GUI_PANEL_COLOR,
                          GUI_ScopeSelected(data, GUI_SCOPE_FIELD_TRIG));

    tile = (GUI_Rect_t){472U, GUI_SCOPE_MENU_Y, 74U, GUI_SCOPE_MENU_H};
    GUI_DrawScopeMenuTile(&tile,
                          "HOLD",
                          data->scope_hold != 0U ? "ON" : "OFF",
                          data->scope_hold != 0U ? GUI_WARN_COLOR : GUI_PANEL_COLOR,
                          GUI_ScopeSelected(data, GUI_SCOPE_FIELD_HOLD));

    tile = (GUI_Rect_t){550U, GUI_SCOPE_MENU_Y, 86U, GUI_SCOPE_MENU_H};
    GUI_DrawScopeMenuTile(&tile, "BACK", "M", GUI_PANEL_COLOR, GUI_ScopeSelected(data, GUI_SCOPE_FIELD_BACK));
}

static void GUI_DrawScopeMenuTile(const GUI_Rect_t *rect, const char *title, const char *value, uint16_t fill_color, uint8_t selected)
{
    GUI_Rect_t title_rect = {rect->x, (uint16_t)(rect->y + 2U), rect->w, 15U};
    GUI_Rect_t value_rect = {rect->x, (uint16_t)(rect->y + 17U), rect->w, 18U};

    GUI_DrawPanel(rect, fill_color);
    GUI_DrawCenteredText(&title_rect, title, GUI_FONT_TOP_LABEL, GUI_MUTED_COLOR, fill_color);
    GUI_DrawCenteredText(&value_rect, value, GUI_FONT_TOP_LABEL, GUI_TEXT_COLOR, fill_color);
    if (selected != 0U)
    {
        GUI_DrawPanelBorder(rect, GUI_ACCENT_COLOR);
    }
}

static void GUI_DrawScopeLegend(uint16_t x, uint16_t y, const char *name, uint16_t color, float value, const char *unit, uint8_t selected)
{
    char buf[32];
    GUI_Rect_t text_rect = {(uint16_t)(x + 18U), y, 118U, 18U};

    LCD_Rect_Fill(x, (uint16_t)(y + 6U), (uint16_t)(x + 14U), (uint16_t)(y + 9U), color);
    snprintf(buf, sizeof(buf), "%s %.2f%s", name, value, unit);
    GUI_DrawCenteredText(&text_rect, buf, GUI_FONT_TOP_LABEL, selected != 0U ? GUI_ACCENT_COLOR : GUI_TEXT_COLOR, GUI_BG_COLOR);
}

/**
  * @brief Draw a colored line without touching the global POINT_COLOR.
  */
static void GUI_DrawScopeLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    int32_t dx = (int32_t)x1 - (int32_t)x0;
    int32_t dy = (int32_t)y1 - (int32_t)y0;
    int32_t sx = (dx >= 0) ? 1 : -1;
    int32_t sy = (dy >= 0) ? 1 : -1;
    int32_t err;
    int32_t x = x0;
    int32_t y = y0;

    if (dx < 0) { dx = -dx; }
    if (dy < 0) { dy = -dy; }

    err = dx - dy;
    while (1)
    {
        LCD_DrawPixelColor((uint16_t)x, (uint16_t)y, color);
        if (x == x1 && y == y1)
        {
            break;
        }

        if ((err * 2) > -dy)
        {
            err -= dy;
            x += sx;
        }
        if ((err * 2) < dx)
        {
            err += dx;
            y += sy;
        }
    }
}

static uint16_t GUI_ScopeMapAnalogY(float value, float full_scale)
{
    float normalized;
    uint16_t max_y = (uint16_t)(GUI_SCOPE_GRID_Y + GUI_SCOPE_GRID_H - 4U);
    uint16_t usable_h = (uint16_t)(GUI_SCOPE_GRID_H - 8U);

    if (value < 0.0F)
    {
        value = 0.0F;
    }
    if (value > full_scale)
    {
        value = full_scale;
    }

    normalized = value / full_scale;
    return (uint16_t)(max_y - (uint16_t)(normalized * (float)usable_h));
}

static uint16_t GUI_ScopeMapX(uint8_t timebase, uint16_t display_index)
{
    uint16_t visible_count = GUI_ScopeVisibleCount(timebase);

    if (visible_count <= 1U)
    {
        return GUI_SCOPE_GRID_X;
    }

    return (uint16_t)(GUI_SCOPE_GRID_X + ((uint32_t)display_index * (GUI_SCOPE_GRID_W - 1U)) / (visible_count - 1U));
}

static uint16_t GUI_ScopeNextIndex(uint16_t index)
{
    ++index;
    if (index >= GUI_SCOPE_SAMPLE_COUNT)
    {
        index = 0U;
    }

    return index;
}

static float GUI_ScopeSampleValue(const GUI_ScopeSample_t *sample, uint8_t source)
{
    if (sample == NULL)
    {
        return 0.0F;
    }

    switch (source)
    {
        case GUI_SCOPE_SOURCE_IOUT: return sample->iout;
        case GUI_SCOPE_SOURCE_POUT: return sample->pout;
        case GUI_SCOPE_SOURCE_VIN: return sample->vin;
        case GUI_SCOPE_SOURCE_IIN: return sample->iin;
        case GUI_SCOPE_SOURCE_PIN: return sample->pin;
        case GUI_SCOPE_SOURCE_VSET: return sample->vset;
        case GUI_SCOPE_SOURCE_ISET: return sample->iset;
        case GUI_SCOPE_SOURCE_EFF: return sample->efficiency;
        case GUI_SCOPE_SOURCE_FAN: return sample->fan;
        case GUI_SCOPE_SOURCE_VOUT:
        default:
            return sample->vout;
    }
}

static float GUI_ScopeCurrentValue(const GUI_Data_t *data, uint8_t source)
{
    switch (source)
    {
        case GUI_SCOPE_SOURCE_IOUT: return data->iout;
        case GUI_SCOPE_SOURCE_POUT: return data->pout;
        case GUI_SCOPE_SOURCE_VIN: return data->vin;
        case GUI_SCOPE_SOURCE_IIN: return data->iin;
        case GUI_SCOPE_SOURCE_PIN: return data->pin;
        case GUI_SCOPE_SOURCE_VSET: return data->vset;
        case GUI_SCOPE_SOURCE_ISET: return data->iset;
        case GUI_SCOPE_SOURCE_EFF: return data->efficiency;
        case GUI_SCOPE_SOURCE_FAN: return data->fan;
        case GUI_SCOPE_SOURCE_VOUT:
        default:
            return data->vout;
    }
}

static float GUI_ScopeFullScale(uint8_t source, uint8_t scale)
{
    float base;
    static const float scale_factor[] =
    {
        0.25F,
        0.5F,
        1.0F,
        2.0F,
        4.0F
    };

    switch (source)
    {
        case GUI_SCOPE_SOURCE_IOUT:
        case GUI_SCOPE_SOURCE_IIN:
        case GUI_SCOPE_SOURCE_ISET:
            base = 20.0F;
            break;
        case GUI_SCOPE_SOURCE_POUT:
        case GUI_SCOPE_SOURCE_PIN:
            base = 600.0F;
            break;
        case GUI_SCOPE_SOURCE_EFF:
        case GUI_SCOPE_SOURCE_FAN:
            base = 100.0F;
            break;
        case GUI_SCOPE_SOURCE_VOUT:
        case GUI_SCOPE_SOURCE_VIN:
        case GUI_SCOPE_SOURCE_VSET:
        default:
            base = 60.0F;
            break;
    }

    if (scale >= (sizeof(scale_factor) / sizeof(scale_factor[0])))
    {
        scale = 2U;
    }

    return base * scale_factor[scale];
}

static const char *GUI_ScopeSourceText(uint8_t source)
{
    switch (source)
    {
        case GUI_SCOPE_SOURCE_IOUT: return "IOUT";
        case GUI_SCOPE_SOURCE_POUT: return "POUT";
        case GUI_SCOPE_SOURCE_VIN: return "VIN";
        case GUI_SCOPE_SOURCE_IIN: return "IIN";
        case GUI_SCOPE_SOURCE_PIN: return "PIN";
        case GUI_SCOPE_SOURCE_VSET: return "VSET";
        case GUI_SCOPE_SOURCE_ISET: return "ISET";
        case GUI_SCOPE_SOURCE_EFF: return "EFF";
        case GUI_SCOPE_SOURCE_FAN: return "FAN";
        case GUI_SCOPE_SOURCE_VOUT:
        default:
            return "VOUT";
    }
}

static const char *GUI_ScopeSourceUnit(uint8_t source)
{
    switch (source)
    {
        case GUI_SCOPE_SOURCE_IOUT:
        case GUI_SCOPE_SOURCE_IIN:
        case GUI_SCOPE_SOURCE_ISET:
            return "A";
        case GUI_SCOPE_SOURCE_POUT:
        case GUI_SCOPE_SOURCE_PIN:
            return "W";
        case GUI_SCOPE_SOURCE_EFF:
        case GUI_SCOPE_SOURCE_FAN:
            return "%";
        case GUI_SCOPE_SOURCE_VOUT:
        case GUI_SCOPE_SOURCE_VIN:
        case GUI_SCOPE_SOURCE_VSET:
        default:
            return "V";
    }
}

static const char *GUI_ScopeScaleText(uint8_t source, uint8_t scale)
{
    static char text[2][8];
    static uint8_t index;
    char *buf;
    float full_scale = GUI_ScopeFullScale(source, scale);

    index = (uint8_t)((index + 1U) & 1U);
    buf = text[index];
    if (full_scale >= 100.0F)
    {
        snprintf(buf, 8U, "%.0f%s", full_scale, GUI_ScopeSourceUnit(source));
    }
    else
    {
        snprintf(buf, 8U, "%.1f%s", full_scale, GUI_ScopeSourceUnit(source));
    }

    return buf;
}

static const char *GUI_ScopeTimebaseText(uint8_t timebase)
{
    switch (timebase)
    {
        case 0U: return "100m";
        case 1U: return "250m";
        case 2U: return "500m";
        case 3U: return "1s";
        default: return "2s";
    }
}

static const char *GUI_ScopeTriggerText(uint8_t trigger)
{
    switch (trigger)
    {
        case GUI_SCOPE_TRIGGER_FAULT: return "FAULT";
        case GUI_SCOPE_TRIGGER_STATE: return "STATE";
        case GUI_SCOPE_TRIGGER_AUTO:
        default:
            return "AUTO";
    }
}

static const char *GUI_ScopeTriggerShortText(uint8_t trigger)
{
    switch (trigger)
    {
        case GUI_SCOPE_TRIGGER_FAULT: return "FLT";
        case GUI_SCOPE_TRIGGER_STATE: return "STA";
        case GUI_SCOPE_TRIGGER_AUTO:
        default:
            return "AUTO";
    }
}

static uint8_t GUI_ScopeSelected(const GUI_Data_t *data, uint8_t field)
{
    return (uint8_t)(data->scope_field == field ? 1U : 0U);
}

static float GUI_ScopeAbs(float value)
{
    return (value < 0.0F) ? -value : value;
}
