/**
  ******************************************************************************
  * @file    gui_wave.c
  * @author  UF4
  * @date    26-6-20 下午6:22
  * @brief   UF4GUI waveform widget with fixed ring buffers.
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

static int16_t gui_wave_map_y(const GUI_Widget *w, int16_t value)
{
    int32_t den = (int32_t)w->data.wave.max - w->data.wave.min;
    int32_t y;

    if (den == 0) {
        return (int16_t)(w->obj.rect.y + w->obj.rect.h / 2);
    }
    if (value < w->data.wave.min) {
        value = w->data.wave.min;
    }
    if (value > w->data.wave.max) {
        value = w->data.wave.max;
    }
    y = (int32_t)w->obj.rect.y + w->obj.rect.h - 10 -
        (((int32_t)value - w->data.wave.min) * (w->obj.rect.h - 20)) / den;
    return (int16_t)y;
}

static uint16_t gui_wave_index(const GUI_Widget *w, uint16_t pos)
{
    uint16_t start = (w->data.wave.head + GUI_WAVE_MAX_POINTS - w->data.wave.count) % GUI_WAVE_MAX_POINTS;
    return (uint16_t)((start + pos) % GUI_WAVE_MAX_POINTS);
}

static void gui_wave_draw_curve(GUI_Widget *w, const int16_t *data, GUI_Color color)
{
    uint16_t i;
    int16_t last_x = 0;
    int16_t last_y = 0;

    if (w->data.wave.count < 2U) {
        return;
    }

    for (i = 0U; i < w->data.wave.count; ++i) {
        uint16_t idx = gui_wave_index(w, i);
        int16_t x = (int16_t)(w->obj.rect.x + 6 +
                              ((uint32_t)i * (uint32_t)(w->obj.rect.w - 12)) / (w->data.wave.count - 1U));
        int16_t y = gui_wave_map_y(w, data[idx]);

        if (i != 0U) {
            GUI_DrawLine(last_x, last_y, x, y, color);
        }
        last_x = x;
        last_y = y;
    }
}

static void gui_draw_wave(GUI_Widget *w, const GUI_Rect *clip)
{
    const GUI_Theme *t = GUI_GetTheme();
    GUI_Rect r = w->obj.rect;
    uint8_t i;

    (void)clip;
    GUI_FillRect(&r, t->bg);
    GUI_DrawRect(&r, t->border);

    for (i = 1U; i < 4U; ++i) {
        int16_t y = (int16_t)(r.y + ((uint16_t)r.h * i) / 4U);
        GUI_DrawLine(r.x, y, (int16_t)(r.x + r.w - 1), y, t->border);
    }
    for (i = 1U; i < 8U; ++i) {
        int16_t x = (int16_t)(r.x + ((uint16_t)r.w * i) / 8U);
        GUI_DrawLine(x, r.y, x, (int16_t)(r.y + r.h - 1), t->border);
    }

    if (w->data.wave.show_voltage != 0U) {
        gui_wave_draw_curve(w, w->data.wave.data_v, t->accent);
    }
    if (w->data.wave.show_current != 0U) {
        gui_wave_draw_curve(w, w->data.wave.data_i, t->accent2);
    }
    if (w->data.wave.show_temp != 0U) {
        gui_wave_draw_curve(w, w->data.wave.data_t, t->warning);
    }
    if (w->data.wave.show_power != 0U) {
        gui_wave_draw_curve(w, w->data.wave.data_p, t->success);
    }
}

/**
  * @brief  Creates a waveform widget with fixed ring buffers.
  * @param  w Pointer to widget storage.
  * @param  r Pointer to widget rectangle.
  * @param  min Minimum waveform display value.
  * @param  max Maximum waveform display value.
  * @retval None
  */
void GUI_WaveView_Create(GUI_Widget *w, const GUI_Rect *r, int16_t min, int16_t max)
{
    uint16_t i;

    GUI_Widget_Init(w, GUI_WIDGET_WAVE, r);
    w->draw = gui_draw_wave;
    w->data.wave.head = 0U;
    w->data.wave.count = 0U;
    w->data.wave.min = min;
    w->data.wave.max = max;
    w->data.wave.show_voltage = 1U;
    w->data.wave.show_current = 1U;
    w->data.wave.show_temp = 0U;
    w->data.wave.show_power = 1U;

    for (i = 0U; i < GUI_WAVE_MAX_POINTS; ++i) {
        w->data.wave.data_v[i] = 0;
        w->data.wave.data_i[i] = 0;
        w->data.wave.data_t[i] = 0;
        w->data.wave.data_p[i] = 0;
    }
}

/**
  * @brief  Pushes one sample set into a waveform widget.
  * @param  w Pointer to waveform widget.
  * @param  voltage Voltage curve sample.
  * @param  current Current curve sample.
  * @param  temp Temperature curve sample.
  * @param  power Power curve sample.
  * @retval None
  */
void GUI_WaveView_Push(GUI_Widget *w, int16_t voltage, int16_t current, int16_t temp, int16_t power)
{
    if ((w == 0) || (w->type != GUI_WIDGET_WAVE)) {
        return;
    }

    w->data.wave.data_v[w->data.wave.head] = voltage;
    w->data.wave.data_i[w->data.wave.head] = current;
    w->data.wave.data_t[w->data.wave.head] = temp;
    w->data.wave.data_p[w->data.wave.head] = power;
    w->data.wave.head = (uint16_t)((w->data.wave.head + 1U) % GUI_WAVE_MAX_POINTS);
    if (w->data.wave.count < GUI_WAVE_MAX_POINTS) {
        w->data.wave.count++;
    }
    GUI_InvalidateWidget(w);
}
