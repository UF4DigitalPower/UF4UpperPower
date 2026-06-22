/**
  ******************************************************************************
  * @file    gui.c
  * @author  UF4
  * @date    26-6-21 下午9:57
  * @brief
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
#include <string.h>

#include "bsp_font.h"
#include "bsp_lcd.h"

#define GUI_BG_COLOR        0x0000
#define GUI_PANEL_COLOR     0x1082
#define GUI_PANEL_DARK      0x0841
#define GUI_BORDER_COLOR    0x39E7
#define GUI_TEXT_COLOR      0xFFFF
#define GUI_MUTED_COLOR     0xBDF7
#define GUI_ACCENT_COLOR    0x07FF

#define GUI_FONT_TOP_LABEL  18U
#define GUI_FONT_TOP_VALUE  34U
#define GUI_FONT_TILE_TITLE 18U
#define GUI_FONT_TILE_VALUE 24U
#define GUI_FONT_SET_VALUE  34U
#define GUI_FONT_LABEL      24U
#define GUI_FONT_VALUE      144U
#define GUI_SET_CELL_W      16U
#define GUI_SET_TEXT_W      (5U * GUI_SET_CELL_W)

#define GUI_DIRTY_VIN       (1UL << 0)
#define GUI_DIRTY_IIN       (1UL << 1)
#define GUI_DIRTY_PIN       (1UL << 2)
#define GUI_DIRTY_EFF       (1UL << 3)
#define GUI_DIRTY_FAN       (1UL << 4)
#define GUI_DIRTY_VOUT      (1UL << 5)
#define GUI_DIRTY_IOUT      (1UL << 6)
#define GUI_DIRTY_POUT      (1UL << 7)
#define GUI_DIRTY_VSET      (1UL << 8)
#define GUI_DIRTY_ISET      (1UL << 9)
#define GUI_DIRTY_VSET_DIG  (1UL << 10)
#define GUI_DIRTY_ISET_DIG  (1UL << 11)
#define GUI_DIRTY_CPU_TEMP  (1UL << 12)
#define GUI_DIRTY_BUCK_TEMP (1UL << 13)
#define GUI_DIRTY_BOOST_TEMP (1UL << 14)
#define GUI_DIRTY_PERF      (1UL << 15)

#define GUI_TOP_Y           4U
#define GUI_TOP_H           54U
#define GUI_BODY_Y          66U
#define GUI_ROW_H           132U
#define GUI_ROW_GAP         6U
#define GUI_LEFT_X          4U
#define GUI_LEFT_W          360U
#define GUI_RIGHT_X         372U
#define GUI_RIGHT_COL_W     126U
#define GUI_RIGHT_GAP       6U
#define GUI_RIGHT_COL2_X    (GUI_RIGHT_X + GUI_RIGHT_COL_W + GUI_RIGHT_GAP)
#define GUI_RIGHT_COL2_W    132U
#define GUI_SIDE_TILE_H     42U
#define GUI_SIDE_TILE_GAP   3U
#define GUI_PERF_Y          (GUI_BODY_Y + 2U * (GUI_ROW_H + GUI_ROW_GAP) + GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP)
#define GUI_PERF_H          (2U * GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP)
#define GUI_MAIN_VALUE_X_OFFSET 6U
#define GUI_MAIN_VALUE_Y_OFFSET 20U
#define GUI_MAIN_VALUE_W        300U
#define GUI_MAIN_VALUE_H        104U
#define GUI_MAIN_VALUE_CELL_W   60U

static GUI_Data_t g_last_data;
static uint8_t g_gui_has_last;
static uint32_t g_draw_buffer_stale_mask;

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
} GUI_Rect_t;

typedef struct
{
    GUI_Rect_t rect;
    const char *title;
    const char *unit;
} GUI_ValueTile_t;

typedef struct
{
    GUI_Rect_t rect;
    const char *title;
    const char *value;
} GUI_TextTile_t;

static const GUI_ValueTile_t g_top_tiles[] =
{
    {{  4, GUI_TOP_Y, 124, GUI_TOP_H}, "VIN", "V"},
    {{132, GUI_TOP_Y, 124, GUI_TOP_H}, "IIN", "A"},
    {{260, GUI_TOP_Y, 124, GUI_TOP_H}, "PIN", "W"},
    {{388, GUI_TOP_Y, 124, GUI_TOP_H}, "EFF", "%"},
    {{512, GUI_TOP_Y, 124, GUI_TOP_H}, "FAN", "%"},
};

static const GUI_ValueTile_t g_main_tiles[] =
{
    {{GUI_LEFT_X, GUI_BODY_Y, GUI_LEFT_W, GUI_ROW_H}, "VO", "V"},
    {{GUI_LEFT_X, GUI_BODY_Y + GUI_ROW_H + GUI_ROW_GAP, GUI_LEFT_W, GUI_ROW_H}, "IO", "A"},
    {{GUI_LEFT_X, GUI_BODY_Y + 2U * (GUI_ROW_H + GUI_ROW_GAP), GUI_LEFT_W, GUI_ROW_H}, "PO", "W"},
};

static const GUI_ValueTile_t g_set_tiles[] =
{
    {{GUI_RIGHT_X, GUI_BODY_Y, GUI_RIGHT_COL_W, 63}, "VSET", "V"},
    {{GUI_RIGHT_X, GUI_BODY_Y + 69U, GUI_RIGHT_COL_W, 63}, "ISET", "A"},
};

static const GUI_TextTile_t g_status_tiles[] =
{
    {{GUI_RIGHT_COL2_X, GUI_BODY_Y, GUI_RIGHT_COL2_W, GUI_SIDE_TILE_H}, "OTP", "00.00"},
    {{GUI_RIGHT_COL2_X, GUI_BODY_Y + GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP, GUI_RIGHT_COL2_W, GUI_SIDE_TILE_H}, "OVP", "00.00"},
    {{GUI_RIGHT_COL2_X, GUI_BODY_Y + 2U * (GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP), GUI_RIGHT_COL2_W, GUI_SIDE_TILE_H}, "OCP", "00.00"},
    {{GUI_RIGHT_X, GUI_BODY_Y + GUI_ROW_H + GUI_ROW_GAP, GUI_RIGHT_COL_W, GUI_SIDE_TILE_H}, "OUT", "OFF"},
    {{GUI_RIGHT_X, GUI_BODY_Y + GUI_ROW_H + GUI_ROW_GAP + GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP, GUI_RIGHT_COL_W, GUI_SIDE_TILE_H}, "TOPO", "NA"},
    {{GUI_RIGHT_X, GUI_BODY_Y + GUI_ROW_H + GUI_ROW_GAP + 2U * (GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP), GUI_RIGHT_COL_W, GUI_SIDE_TILE_H}, "MODE", "CV"},
    {{GUI_RIGHT_COL2_X, GUI_BODY_Y + GUI_ROW_H + GUI_ROW_GAP, GUI_RIGHT_COL2_W, GUI_SIDE_TILE_H}, "CMD", "PASS"},
    {{GUI_RIGHT_COL2_X, GUI_BODY_Y + GUI_ROW_H + GUI_ROW_GAP + GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP, GUI_RIGHT_COL2_W, GUI_SIDE_TILE_H}, "STATE", "INIT"},
    {{GUI_RIGHT_COL2_X, GUI_BODY_Y + GUI_ROW_H + GUI_ROW_GAP + 2U * (GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP), GUI_RIGHT_COL2_W, GUI_SIDE_TILE_H}, "FAULT", "NA"},
    {{GUI_RIGHT_COL2_X, GUI_BODY_Y + 2U * (GUI_ROW_H + GUI_ROW_GAP), GUI_RIGHT_COL2_W, GUI_SIDE_TILE_H}, "BLE", "NA"},
};

static const GUI_ValueTile_t g_temp_tiles[] =
{
    {{GUI_RIGHT_X, GUI_BODY_Y + 2U * (GUI_ROW_H + GUI_ROW_GAP), GUI_RIGHT_COL_W, GUI_SIDE_TILE_H}, "CPU", "C"},
    {{GUI_RIGHT_X, GUI_BODY_Y + 2U * (GUI_ROW_H + GUI_ROW_GAP) + GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP, GUI_RIGHT_COL_W, GUI_SIDE_TILE_H}, "BUCK", "C"},
    {{GUI_RIGHT_X, GUI_BODY_Y + 2U * (GUI_ROW_H + GUI_ROW_GAP) + 2U * (GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP), GUI_RIGHT_COL_W, GUI_SIDE_TILE_H}, "BOOST", "C"},
};

static const GUI_Rect_t g_perf_rect =
{
    GUI_RIGHT_COL2_X,
    GUI_PERF_Y,
    GUI_RIGHT_COL2_W,
    GUI_PERF_H
};

static void GUI_DrawBorder(const GUI_Rect_t *rect);
static void GUI_DrawPanel(const GUI_Rect_t *rect, uint32_t color);
static void GUI_DrawCenteredText(const GUI_Rect_t *rect, const char *text, uint16_t font, uint32_t color, uint32_t bg);
static void GUI_DrawSmallValueTile(const GUI_ValueTile_t *tile, float value);
static void GUI_DrawMainValueTile(const GUI_ValueTile_t *tile, float value);
static void GUI_DrawMainValueTileDelta(const GUI_ValueTile_t *tile, float value, float previous_value);
static void GUI_DrawSetValueTile(const GUI_ValueTile_t *tile, float value, uint8_t digit);
static void GUI_DrawTempTile(const GUI_ValueTile_t *tile, float value);
static void GUI_DrawTextTile(const GUI_TextTile_t *tile);
static void GUI_DrawPerfTile(uint16_t fps, uint8_t cpu_usage);
static void GUI_DrawDirtyTiles(const GUI_Data_t *data, uint32_t dirty_mask);
static void GUI_CopyDirtyTilesFromFront(uint32_t dirty_mask);
static void GUI_CopyRectFromFront(const GUI_Rect_t *rect);
static void GUI_CopyMainValueFromFront(const GUI_ValueTile_t *tile);
static void GUI_FormatMainValue(char *buf, uint32_t size, float value);
static void GUI_FormatFixed2(char *buf, uint32_t size, float value);
static uint8_t GUI_FormattedMainChanged(float current, float previous);
static uint8_t GUI_FormattedFixed2Changed(float current, float previous);
static uint8_t GUI_FloatChanged(float a, float b);
static uint8_t GUI_FloatChangedBy(float a, float b, float threshold);

void GUI_Init(void)
{
    GUI_Clear();

    GUI_DrawStatic();
    LCD_Present();
    GUI_Clear();
    GUI_DrawStatic();
    g_gui_has_last = 0U;
    g_draw_buffer_stale_mask = 0U;
}

void GUI_Clear(void)
{
    LCD_Clear(GUI_BG_COLOR);
}

void GUI_DrawStatic(void)
{
    uint32_t i;

    for (i = 0U; i < sizeof(g_top_tiles) / sizeof(g_top_tiles[0]); ++i)
    {
        GUI_DrawPanel(
            &g_top_tiles[i].rect,
            GUI_PANEL_COLOR);
    }

    for (i = 0U; i < sizeof(g_main_tiles) / sizeof(g_main_tiles[0]); ++i)
    {
        GUI_DrawPanel(
            &g_main_tiles[i].rect,
            GUI_PANEL_DARK);
    }

    for (i = 0U; i < sizeof(g_set_tiles) / sizeof(g_set_tiles[0]); ++i)
    {
        GUI_DrawPanel(
            &g_set_tiles[i].rect,
            GUI_PANEL_COLOR);
    }

    for (i = 0U; i < sizeof(g_status_tiles) / sizeof(g_status_tiles[0]); ++i)
    {
        GUI_DrawTextTile(&g_status_tiles[i]);
    }

    for (i = 0U; i < sizeof(g_temp_tiles) / sizeof(g_temp_tiles[0]); ++i)
    {
        GUI_DrawPanel(
            &g_temp_tiles[i].rect,
            GUI_PANEL_COLOR);
    }
}

void GUI_Update(
        const GUI_Data_t *data)
{
    uint32_t dirty_mask = 0U;
    uint8_t vin_dirty;
    uint8_t iin_dirty;
    uint8_t pin_dirty;
    uint8_t efficiency_dirty;
    uint8_t fan_dirty;
    uint8_t vout_dirty;
    uint8_t iout_dirty;
    uint8_t pout_dirty;
    uint8_t vset_dirty;
    uint8_t iset_dirty;
    uint8_t vset_digit_dirty;
    uint8_t iset_digit_dirty;
    uint8_t cpu_temp_dirty;
    uint8_t buck_temp_dirty;
    uint8_t boost_temp_dirty;
    uint8_t perf_dirty;

    if(data == NULL)
    {
        return;
    }

    vin_dirty = (uint8_t)(!g_gui_has_last || GUI_FormattedMainChanged(data->vin, g_last_data.vin));
    iin_dirty = (uint8_t)(!g_gui_has_last || GUI_FormattedMainChanged(data->iin, g_last_data.iin));
    pin_dirty = (uint8_t)(!g_gui_has_last || GUI_FormattedMainChanged(data->pin, g_last_data.pin));
    efficiency_dirty = (uint8_t)(!g_gui_has_last || GUI_FormattedMainChanged(data->efficiency, g_last_data.efficiency));
    fan_dirty = (uint8_t)(!g_gui_has_last || GUI_FormattedMainChanged(data->fan, g_last_data.fan));
    vout_dirty = (uint8_t)(!g_gui_has_last || GUI_FormattedMainChanged(data->vout, g_last_data.vout));
    iout_dirty = (uint8_t)(!g_gui_has_last || GUI_FormattedMainChanged(data->iout, g_last_data.iout));
    pout_dirty = (uint8_t)(!g_gui_has_last || GUI_FormattedMainChanged(data->pout, g_last_data.pout));
    vset_dirty = (uint8_t)(!g_gui_has_last || GUI_FloatChanged(data->vset, g_last_data.vset));
    iset_dirty = (uint8_t)(!g_gui_has_last || GUI_FloatChanged(data->iset, g_last_data.iset));
    vset_digit_dirty = (uint8_t)(!g_gui_has_last || data->vset_digit != g_last_data.vset_digit);
    iset_digit_dirty = (uint8_t)(!g_gui_has_last || data->iset_digit != g_last_data.iset_digit);
    cpu_temp_dirty = (uint8_t)(!g_gui_has_last || GUI_FormattedFixed2Changed(data->cpu_temp, g_last_data.cpu_temp));
    buck_temp_dirty = (uint8_t)(!g_gui_has_last || GUI_FormattedFixed2Changed(data->buck_temp, g_last_data.buck_temp));
    boost_temp_dirty = (uint8_t)(!g_gui_has_last || GUI_FormattedFixed2Changed(data->boost_temp, g_last_data.boost_temp));
    perf_dirty = (uint8_t)(!g_gui_has_last ||
                           data->fps != g_last_data.fps ||
                           data->cpu_usage != g_last_data.cpu_usage);

    if (vin_dirty) { dirty_mask |= GUI_DIRTY_VIN; }
    if (iin_dirty) { dirty_mask |= GUI_DIRTY_IIN; }
    if (pin_dirty) { dirty_mask |= GUI_DIRTY_PIN; }
    if (efficiency_dirty) { dirty_mask |= GUI_DIRTY_EFF; }
    if (fan_dirty) { dirty_mask |= GUI_DIRTY_FAN; }
    if (vout_dirty) { dirty_mask |= GUI_DIRTY_VOUT; }
    if (iout_dirty) { dirty_mask |= GUI_DIRTY_IOUT; }
    if (pout_dirty) { dirty_mask |= GUI_DIRTY_POUT; }
    if (vset_dirty) { dirty_mask |= GUI_DIRTY_VSET; }
    if (iset_dirty) { dirty_mask |= GUI_DIRTY_ISET; }
    if (vset_digit_dirty) { dirty_mask |= GUI_DIRTY_VSET_DIG; }
    if (iset_digit_dirty) { dirty_mask |= GUI_DIRTY_ISET_DIG; }
    if (cpu_temp_dirty) { dirty_mask |= GUI_DIRTY_CPU_TEMP; }
    if (buck_temp_dirty) { dirty_mask |= GUI_DIRTY_BUCK_TEMP; }
    if (boost_temp_dirty) { dirty_mask |= GUI_DIRTY_BOOST_TEMP; }
    if (perf_dirty) { dirty_mask |= GUI_DIRTY_PERF; }

    if (dirty_mask == 0U)
    {
        return;
    }

    if (g_draw_buffer_stale_mask != 0U)
    {
        GUI_CopyDirtyTilesFromFront(g_draw_buffer_stale_mask);
        g_draw_buffer_stale_mask = 0U;
    }

    GUI_DrawDirtyTiles(data, dirty_mask);
    g_last_data = *data;
    LCD_Present();
    if (g_gui_has_last)
    {
        g_draw_buffer_stale_mask |= dirty_mask;
    }
    else
    {
        g_draw_buffer_stale_mask = 0U;
        GUI_DrawDirtyTiles(data, dirty_mask);
        g_gui_has_last = 1U;
    }
}

static void GUI_DrawPanel(const GUI_Rect_t *rect, uint32_t color)
{
    LCD_Rect_Fill(
        rect->x,
        rect->y,
        (uint16_t)(rect->x + rect->w - 1U),
        (uint16_t)(rect->y + rect->h - 1U),
        color);

    GUI_DrawBorder(rect);
}

static void GUI_DrawBorder(const GUI_Rect_t *rect)
{
    uint16_t x2 =
        (uint16_t)(rect->x + rect->w - 1U);
    uint16_t y2 =
        (uint16_t)(rect->y + rect->h - 1U);

    LCD_Rect_Fill(
        rect->x,
        rect->y,
        x2,
        rect->y,
        GUI_BORDER_COLOR);
    LCD_Rect_Fill(
        rect->x,
        y2,
        x2,
        y2,
        GUI_BORDER_COLOR);
    LCD_Rect_Fill(
        rect->x,
        rect->y,
        rect->x,
        y2,
        GUI_BORDER_COLOR);
    LCD_Rect_Fill(
        x2,
        rect->y,
        x2,
        y2,
        GUI_BORDER_COLOR);
}

static void GUI_DrawCenteredText(
        const GUI_Rect_t *rect,
        const char *text,
        uint16_t font,
        uint32_t color,
        uint32_t bg)
{
    uint16_t text_w =
        LCD_MeasureFontString(
            text,
            font);
    uint16_t x =
        rect->x;
    uint16_t y =
        (uint16_t)(rect->y + (rect->h > font ? (rect->h - font) / 2U : 0U));

    if (text_w < rect->w)
    {
        x = (uint16_t)(rect->x + (rect->w - text_w) / 2U);
    }

    LCD_DrawFontStringDMA(
        x,
        y,
        (uint16_t)(rect->x + rect->w - x),
        font,
        text,
        font,
        color,
        bg);
}

static void GUI_DrawSmallValueTile(const GUI_ValueTile_t *tile, float value)
{
    char buf[16];
    GUI_Rect_t title_rect;
    GUI_Rect_t value_rect;
    GUI_Rect_t unit_rect;

    GUI_FormatMainValue(buf, sizeof(buf), value);

    GUI_DrawPanel(&tile->rect, GUI_PANEL_COLOR);

    title_rect.x = tile->rect.x;
    title_rect.y = (uint16_t)(tile->rect.y + 2U);
    title_rect.w = tile->rect.w;
    title_rect.h = 18U;
    GUI_DrawCenteredText(&title_rect, tile->title, GUI_FONT_TOP_LABEL, GUI_MUTED_COLOR, GUI_PANEL_COLOR);

    value_rect.x = (uint16_t)(tile->rect.x + 6U);
    value_rect.y = (uint16_t)(tile->rect.y + 18U);
    value_rect.w = (uint16_t)(tile->rect.w - 30U);
    value_rect.h = 34U;
    LCD_DrawFontStringDMA(value_rect.x, value_rect.y, value_rect.w, value_rect.h, buf, GUI_FONT_TOP_VALUE, GUI_TEXT_COLOR, GUI_PANEL_COLOR);

    unit_rect.x = (uint16_t)(tile->rect.x + tile->rect.w - 22U);
    unit_rect.y = (uint16_t)(tile->rect.y + 24U);
    unit_rect.w = 18U;
    unit_rect.h = 24U;
    GUI_DrawCenteredText(&unit_rect, tile->unit, GUI_FONT_LABEL, GUI_ACCENT_COLOR, GUI_PANEL_COLOR);
}

static void GUI_DrawMainValueTile(const GUI_ValueTile_t *tile, float value)
{
    char buf[16];
    GUI_Rect_t title_rect;
    GUI_Rect_t unit_rect;

    GUI_FormatMainValue(buf, sizeof(buf), value);

    GUI_DrawPanel(&tile->rect, GUI_PANEL_DARK);
    LCD_DrawFontStringFixedDMA(
        (uint16_t)(tile->rect.x + GUI_MAIN_VALUE_X_OFFSET),
        (uint16_t)(tile->rect.y + GUI_MAIN_VALUE_Y_OFFSET),
        GUI_MAIN_VALUE_W,
        GUI_MAIN_VALUE_H,
        buf,
        GUI_FONT_VALUE,
        GUI_MAIN_VALUE_CELL_W,
        GUI_TEXT_COLOR,
        GUI_PANEL_DARK);

    title_rect.x = (uint16_t)(tile->rect.x + tile->rect.w - 40U);
    title_rect.y = (uint16_t)(tile->rect.y + 42U);
    title_rect.w = 34U;
    title_rect.h = 24U;
    GUI_DrawCenteredText(&title_rect, tile->title, GUI_FONT_LABEL, GUI_MUTED_COLOR, GUI_PANEL_DARK);

    unit_rect.x = (uint16_t)(tile->rect.x + tile->rect.w - 38U);
    unit_rect.y = (uint16_t)(tile->rect.y + 74U);
    unit_rect.w = 32U;
    unit_rect.h = 40U;
    GUI_DrawCenteredText(&unit_rect, tile->unit, GUI_FONT_LABEL, GUI_ACCENT_COLOR, GUI_PANEL_DARK);

    GUI_DrawBorder(&tile->rect);
}

static void GUI_DrawMainValueTileDelta(const GUI_ValueTile_t *tile, float value, float previous_value)
{
    char buf[16];
    char last_buf[16];
    uint32_t i;
    uint8_t changed = 0U;
    const uint16_t text_x = (uint16_t)(tile->rect.x + GUI_MAIN_VALUE_X_OFFSET);
    const uint16_t text_y = (uint16_t)(tile->rect.y + GUI_MAIN_VALUE_Y_OFFSET);
    const uint16_t text_h = GUI_MAIN_VALUE_H;
    const uint16_t cell_w = GUI_MAIN_VALUE_CELL_W;
    const uint16_t max_cells = 5U;

    GUI_FormatMainValue(buf, sizeof(buf), value);
    GUI_FormatMainValue(last_buf, sizeof(last_buf), previous_value);

    if (strlen(buf) != strlen(last_buf))
    {
        GUI_DrawMainValueTile(tile, value);
        return;
    }

    for (i = 0U; i < max_cells && buf[i] != '\0'; ++i)
    {
        char cell_text[2];

        if (buf[i] == last_buf[i])
        {
            continue;
        }

        cell_text[0] = buf[i];
        cell_text[1] = '\0';

        LCD_Rect_Fill(
            (uint16_t)(text_x + i * cell_w),
            text_y,
            (uint16_t)(text_x + (i + 1U) * cell_w - 1U),
            (uint16_t)(text_y + text_h - 1U),
            GUI_PANEL_DARK);

        LCD_DrawFontStringFixedDMA(
            (uint16_t)(text_x + i * cell_w),
            text_y,
            cell_w,
            text_h,
            cell_text,
            GUI_FONT_VALUE,
            cell_w,
            GUI_TEXT_COLOR,
            GUI_PANEL_DARK);

        changed = 1U;
    }

    if (changed != 0U)
    {
        GUI_DrawBorder(&tile->rect);
    }
}

static void GUI_DrawSetValueTile(const GUI_ValueTile_t *tile, float value, uint8_t digit)
{
    char buf[16];
    GUI_Rect_t title_rect;
    GUI_Rect_t value_rect;
    GUI_Rect_t unit_rect;
    uint16_t mark_x;
    uint8_t char_index;

    GUI_FormatFixed2(buf, sizeof(buf), value);

    GUI_DrawPanel(&tile->rect, GUI_PANEL_COLOR);

    title_rect.x = tile->rect.x;
    title_rect.y = (uint16_t)(tile->rect.y + 2U);
    title_rect.w = tile->rect.w;
    title_rect.h = 18U;
    GUI_DrawCenteredText(&title_rect, tile->title, GUI_FONT_TILE_TITLE, GUI_MUTED_COLOR, GUI_PANEL_COLOR);

    value_rect.x = (uint16_t)(tile->rect.x + 10U);
    value_rect.y = (uint16_t)(tile->rect.y + tile->rect.h - 39U);
    value_rect.w = GUI_SET_TEXT_W;
    value_rect.h = 34U;
    LCD_DrawFontStringFixedDMA(
        value_rect.x,
        value_rect.y,
        value_rect.w,
        value_rect.h,
        buf,
        GUI_FONT_SET_VALUE,
        GUI_SET_CELL_W,
        GUI_TEXT_COLOR,
        GUI_PANEL_COLOR);

    unit_rect.x = (uint16_t)(tile->rect.x + tile->rect.w - 24U);
    unit_rect.y = (uint16_t)(value_rect.y + 5U);
    unit_rect.w = 22U;
    unit_rect.h = 24U;
    GUI_DrawCenteredText(&unit_rect, tile->unit, GUI_FONT_TILE_VALUE, GUI_ACCENT_COLOR, GUI_PANEL_COLOR);

    if (digit < 4U)
    {
        char_index = (digit < 2U) ? digit : (uint8_t)(digit + 1U);
        mark_x = (uint16_t)(value_rect.x + (char_index * GUI_SET_CELL_W));
        LCD_Rect_Fill(
            mark_x,
            (uint16_t)(tile->rect.y + tile->rect.h - 4U),
            (uint16_t)(mark_x + GUI_SET_CELL_W - 2U),
            (uint16_t)(tile->rect.y + tile->rect.h - 3U),
            GUI_ACCENT_COLOR);
    }
}

static void GUI_DrawTempTile(const GUI_ValueTile_t *tile, float value)
{
    char buf[16];
    GUI_Rect_t title_rect;
    GUI_Rect_t value_rect;

    GUI_FormatFixed2(buf, sizeof(buf), value);

    GUI_DrawPanel(&tile->rect, GUI_PANEL_COLOR);

    title_rect.x = (uint16_t)(tile->rect.x + 4U);
    title_rect.y = (uint16_t)(tile->rect.y + 11U);
    title_rect.w = 52U;
    title_rect.h = 20U;
    GUI_DrawCenteredText(&title_rect, tile->title, GUI_FONT_TILE_TITLE, GUI_MUTED_COLOR, GUI_PANEL_COLOR);

    value_rect.x = (uint16_t)(tile->rect.x + 56U);
    value_rect.y = (uint16_t)(tile->rect.y + 9U);
    value_rect.w = (uint16_t)(tile->rect.w - 60U);
    value_rect.h = 24U;
    GUI_DrawCenteredText(&value_rect, buf, GUI_FONT_TILE_VALUE, GUI_TEXT_COLOR, GUI_PANEL_COLOR);
}

static void GUI_DrawTextTile(const GUI_TextTile_t *tile)
{
    GUI_Rect_t title_rect;
    GUI_Rect_t value_rect;
    uint16_t title_w;

    GUI_DrawPanel(&tile->rect, GUI_PANEL_COLOR);

    title_w = (tile->rect.w > 128U) ? 60U : 56U;

    title_rect.x = (uint16_t)(tile->rect.x + 4U);
    title_rect.y = (uint16_t)(tile->rect.y + 11U);
    title_rect.w = (uint16_t)(title_w - 4U);
    title_rect.h = 20U;
    GUI_DrawCenteredText(&title_rect, tile->title, GUI_FONT_TILE_TITLE, GUI_MUTED_COLOR, GUI_PANEL_COLOR);

    value_rect.x = (uint16_t)(tile->rect.x + title_w);
    value_rect.y = (uint16_t)(tile->rect.y + 9U);
    value_rect.w = (uint16_t)(tile->rect.w - title_w - 4U);
    value_rect.h = 24U;
    GUI_DrawCenteredText(&value_rect, tile->value, GUI_FONT_TILE_VALUE, GUI_TEXT_COLOR, GUI_PANEL_COLOR);
}

static void GUI_DrawPerfTile(uint16_t fps, uint8_t cpu_usage)
{
    char fps_buf[16];
    char cpu_buf[16];
    GUI_Rect_t fps_label_rect;
    GUI_Rect_t fps_value_rect;
    GUI_Rect_t cpu_label_rect;
    GUI_Rect_t cpu_value_rect;

    if (cpu_usage > 100U)
    {
        cpu_usage = 100U;
    }

    snprintf(fps_buf, sizeof(fps_buf), "%03u", (unsigned int)fps);
    snprintf(cpu_buf, sizeof(cpu_buf), "%03u%%", (unsigned int)cpu_usage);

    GUI_DrawPanel(&g_perf_rect, GUI_PANEL_COLOR);

    fps_label_rect.x = (uint16_t)(g_perf_rect.x + 6U);
    fps_label_rect.y = (uint16_t)(g_perf_rect.y + 10U);
    fps_label_rect.w = 48U;
    fps_label_rect.h = 20U;
    GUI_DrawCenteredText(&fps_label_rect, "FPS", GUI_FONT_TILE_TITLE, GUI_MUTED_COLOR, GUI_PANEL_COLOR);

    fps_value_rect.x = (uint16_t)(g_perf_rect.x + 56U);
    fps_value_rect.y = (uint16_t)(g_perf_rect.y + 8U);
    fps_value_rect.w = (uint16_t)(g_perf_rect.w - 62U);
    fps_value_rect.h = 24U;
    GUI_DrawCenteredText(&fps_value_rect, fps_buf, GUI_FONT_TILE_VALUE, GUI_TEXT_COLOR, GUI_PANEL_COLOR);

    cpu_label_rect.x = (uint16_t)(g_perf_rect.x + 6U);
    cpu_label_rect.y = (uint16_t)(g_perf_rect.y + GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP + 10U);
    cpu_label_rect.w = 48U;
    cpu_label_rect.h = 20U;
    GUI_DrawCenteredText(&cpu_label_rect, "CPU", GUI_FONT_TILE_TITLE, GUI_MUTED_COLOR, GUI_PANEL_COLOR);

    cpu_value_rect.x = (uint16_t)(g_perf_rect.x + 56U);
    cpu_value_rect.y = (uint16_t)(g_perf_rect.y + GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP + 8U);
    cpu_value_rect.w = (uint16_t)(g_perf_rect.w - 62U);
    cpu_value_rect.h = 24U;
    GUI_DrawCenteredText(&cpu_value_rect, cpu_buf, GUI_FONT_TILE_VALUE, GUI_TEXT_COLOR, GUI_PANEL_COLOR);
}

static void GUI_DrawDirtyTiles(const GUI_Data_t *data, uint32_t dirty_mask)
{
    if ((dirty_mask & GUI_DIRTY_VIN) != 0U)
    {
        GUI_DrawSmallValueTile(&g_top_tiles[0], data->vin);
    }

    if ((dirty_mask & GUI_DIRTY_IIN) != 0U)
    {
        GUI_DrawSmallValueTile(&g_top_tiles[1], data->iin);
    }

    if ((dirty_mask & GUI_DIRTY_PIN) != 0U)
    {
        GUI_DrawSmallValueTile(&g_top_tiles[2], data->pin);
    }

    if ((dirty_mask & GUI_DIRTY_EFF) != 0U)
    {
        GUI_DrawSmallValueTile(&g_top_tiles[3], data->efficiency);
    }

    if ((dirty_mask & GUI_DIRTY_FAN) != 0U)
    {
        GUI_DrawSmallValueTile(&g_top_tiles[4], data->fan);
    }

    if ((dirty_mask & GUI_DIRTY_VOUT) != 0U)
    {
        if (g_gui_has_last)
        {
            GUI_DrawMainValueTileDelta(&g_main_tiles[0], data->vout, g_last_data.vout);
        }
        else
        {
            GUI_DrawMainValueTile(&g_main_tiles[0], data->vout);
        }
    }

    if ((dirty_mask & GUI_DIRTY_IOUT) != 0U)
    {
        if (g_gui_has_last)
        {
            GUI_DrawMainValueTileDelta(&g_main_tiles[1], data->iout, g_last_data.iout);
        }
        else
        {
            GUI_DrawMainValueTile(&g_main_tiles[1], data->iout);
        }
    }

    if ((dirty_mask & GUI_DIRTY_POUT) != 0U)
    {
        if (g_gui_has_last)
        {
            GUI_DrawMainValueTileDelta(&g_main_tiles[2], data->pout, g_last_data.pout);
        }
        else
        {
            GUI_DrawMainValueTile(&g_main_tiles[2], data->pout);
        }
    }

    if ((dirty_mask & (GUI_DIRTY_VSET | GUI_DIRTY_VSET_DIG)) != 0U)
    {
        GUI_DrawSetValueTile(&g_set_tiles[0], data->vset, data->vset_digit);
    }

    if ((dirty_mask & (GUI_DIRTY_ISET | GUI_DIRTY_ISET_DIG)) != 0U)
    {
        GUI_DrawSetValueTile(&g_set_tiles[1], data->iset, data->iset_digit);
    }

    if ((dirty_mask & GUI_DIRTY_CPU_TEMP) != 0U)
    {
        GUI_DrawTempTile(&g_temp_tiles[0], data->cpu_temp);
    }

    if ((dirty_mask & GUI_DIRTY_BUCK_TEMP) != 0U)
    {
        GUI_DrawTempTile(&g_temp_tiles[1], data->buck_temp);
    }

    if ((dirty_mask & GUI_DIRTY_BOOST_TEMP) != 0U)
    {
        GUI_DrawTempTile(&g_temp_tiles[2], data->boost_temp);
    }

    if ((dirty_mask & GUI_DIRTY_PERF) != 0U)
    {
        GUI_DrawPerfTile(data->fps, data->cpu_usage);
    }
}

static void GUI_CopyDirtyTilesFromFront(uint32_t dirty_mask)
{
    if ((dirty_mask & GUI_DIRTY_VIN) != 0U)
    {
        GUI_CopyRectFromFront(&g_top_tiles[0].rect);
    }

    if ((dirty_mask & GUI_DIRTY_IIN) != 0U)
    {
        GUI_CopyRectFromFront(&g_top_tiles[1].rect);
    }

    if ((dirty_mask & GUI_DIRTY_PIN) != 0U)
    {
        GUI_CopyRectFromFront(&g_top_tiles[2].rect);
    }

    if ((dirty_mask & GUI_DIRTY_EFF) != 0U)
    {
        GUI_CopyRectFromFront(&g_top_tiles[3].rect);
    }

    if ((dirty_mask & GUI_DIRTY_FAN) != 0U)
    {
        GUI_CopyRectFromFront(&g_top_tiles[4].rect);
    }

    if ((dirty_mask & GUI_DIRTY_VOUT) != 0U)
    {
        GUI_CopyMainValueFromFront(&g_main_tiles[0]);
    }

    if ((dirty_mask & GUI_DIRTY_IOUT) != 0U)
    {
        GUI_CopyMainValueFromFront(&g_main_tiles[1]);
    }

    if ((dirty_mask & GUI_DIRTY_POUT) != 0U)
    {
        GUI_CopyMainValueFromFront(&g_main_tiles[2]);
    }

    if ((dirty_mask & (GUI_DIRTY_VSET | GUI_DIRTY_VSET_DIG)) != 0U)
    {
        GUI_CopyRectFromFront(&g_set_tiles[0].rect);
    }

    if ((dirty_mask & (GUI_DIRTY_ISET | GUI_DIRTY_ISET_DIG)) != 0U)
    {
        GUI_CopyRectFromFront(&g_set_tiles[1].rect);
    }

    if ((dirty_mask & GUI_DIRTY_CPU_TEMP) != 0U)
    {
        GUI_CopyRectFromFront(&g_temp_tiles[0].rect);
    }

    if ((dirty_mask & GUI_DIRTY_BUCK_TEMP) != 0U)
    {
        GUI_CopyRectFromFront(&g_temp_tiles[1].rect);
    }

    if ((dirty_mask & GUI_DIRTY_BOOST_TEMP) != 0U)
    {
        GUI_CopyRectFromFront(&g_temp_tiles[2].rect);
    }

    if ((dirty_mask & GUI_DIRTY_PERF) != 0U)
    {
        GUI_CopyRectFromFront(&g_perf_rect);
    }
}

static void GUI_CopyRectFromFront(const GUI_Rect_t *rect)
{
    LCD_CopyRectFromFrontToDraw(rect->x, rect->y, rect->w, rect->h);
}

static void GUI_CopyMainValueFromFront(const GUI_ValueTile_t *tile)
{
    LCD_CopyRectFromFrontToDraw(
        (uint16_t)(tile->rect.x + GUI_MAIN_VALUE_X_OFFSET),
        (uint16_t)(tile->rect.y + GUI_MAIN_VALUE_Y_OFFSET),
        GUI_MAIN_VALUE_W,
        GUI_MAIN_VALUE_H);
}

static void GUI_FormatMainValue(char *buf, uint32_t size, float value)
{
    float abs_value = value;

    if (abs_value < 0.0F)
    {
        abs_value = -abs_value;
    }

    if (abs_value >= 100.0F)
    {
        snprintf(buf, size, "%05.1f", value);
    }
    else
    {
        snprintf(buf, size, "%05.2f", value);
    }
}

static void GUI_FormatFixed2(char *buf, uint32_t size, float value)
{
    snprintf(buf, size, "%05.2f", value);
}

static uint8_t GUI_FormattedMainChanged(float current, float previous)
{
    char current_buf[16];
    char previous_buf[16];

    GUI_FormatMainValue(current_buf, sizeof(current_buf), current);
    GUI_FormatMainValue(previous_buf, sizeof(previous_buf), previous);

    return (uint8_t)(strcmp(current_buf, previous_buf) != 0);
}

static uint8_t GUI_FormattedFixed2Changed(float current, float previous)
{
    char current_buf[16];
    char previous_buf[16];

    GUI_FormatFixed2(current_buf, sizeof(current_buf), current);
    GUI_FormatFixed2(previous_buf, sizeof(previous_buf), previous);

    return (uint8_t)(strcmp(current_buf, previous_buf) != 0);
}

static uint8_t GUI_FloatChanged(float a, float b)
{
    return GUI_FloatChangedBy(a, b, 0.005F);
}

static uint8_t GUI_FloatChangedBy(float a, float b, float threshold)
{
    float diff = a - b;

    if (diff < 0.0F)
    {
        diff = -diff;
    }

    return diff >= threshold;
}
