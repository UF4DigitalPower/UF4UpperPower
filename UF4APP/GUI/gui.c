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

#include "bap_font.h"
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

static GUI_Data_t g_last_data;
static uint8_t g_gui_has_last;

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

static void GUI_DrawBorder(const GUI_Rect_t *rect);
static void GUI_DrawPanel(const GUI_Rect_t *rect, uint32_t color);
static void GUI_DrawCenteredText(const GUI_Rect_t *rect, const char *text, uint16_t font, uint32_t color, uint32_t bg);
static void GUI_DrawSmallValueTile(const GUI_ValueTile_t *tile, float value);
static void GUI_DrawMainValueTile(const GUI_ValueTile_t *tile, float value);
static void GUI_DrawSetValueTile(const GUI_ValueTile_t *tile, float value, uint8_t digit);
static void GUI_DrawTempTile(const GUI_ValueTile_t *tile, float value);
static void GUI_DrawTextTile(const GUI_TextTile_t *tile);
static void GUI_FormatMainValue(char *buf, uint32_t size, float value);
static void GUI_FormatFixed2(char *buf, uint32_t size, float value);
static uint8_t GUI_FloatChanged(float a, float b);

void GUI_Init(void)
{
    GUI_Clear();

    GUI_DrawStatic();
    LCD_Present();
    g_gui_has_last = 0U;
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
    uint8_t dirty = 0U;
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

    if(data == NULL)
    {
        return;
    }

    vin_dirty = (uint8_t)(!g_gui_has_last || GUI_FloatChanged(data->vin, g_last_data.vin));
    iin_dirty = (uint8_t)(!g_gui_has_last || GUI_FloatChanged(data->iin, g_last_data.iin));
    pin_dirty = (uint8_t)(!g_gui_has_last || GUI_FloatChanged(data->pin, g_last_data.pin));
    efficiency_dirty = (uint8_t)(!g_gui_has_last || GUI_FloatChanged(data->efficiency, g_last_data.efficiency));
    fan_dirty = (uint8_t)(!g_gui_has_last || GUI_FloatChanged(data->fan, g_last_data.fan));
    vout_dirty = (uint8_t)(!g_gui_has_last || GUI_FloatChanged(data->vout, g_last_data.vout));
    iout_dirty = (uint8_t)(!g_gui_has_last || GUI_FloatChanged(data->iout, g_last_data.iout));
    pout_dirty = (uint8_t)(!g_gui_has_last || GUI_FloatChanged(data->pout, g_last_data.pout));
    vset_dirty = (uint8_t)(!g_gui_has_last || GUI_FloatChanged(data->vset, g_last_data.vset));
    iset_dirty = (uint8_t)(!g_gui_has_last || GUI_FloatChanged(data->iset, g_last_data.iset));
    vset_digit_dirty = (uint8_t)(!g_gui_has_last || data->vset_digit != g_last_data.vset_digit);
    iset_digit_dirty = (uint8_t)(!g_gui_has_last || data->iset_digit != g_last_data.iset_digit);
    cpu_temp_dirty = (uint8_t)(!g_gui_has_last || GUI_FloatChanged(data->cpu_temp, g_last_data.cpu_temp));
    buck_temp_dirty = (uint8_t)(!g_gui_has_last || GUI_FloatChanged(data->buck_temp, g_last_data.buck_temp));
    boost_temp_dirty = (uint8_t)(!g_gui_has_last || GUI_FloatChanged(data->boost_temp, g_last_data.boost_temp));

    dirty = (uint8_t)(vin_dirty || iin_dirty || pin_dirty || efficiency_dirty || fan_dirty ||
                      vout_dirty || iout_dirty || pout_dirty || vset_dirty || iset_dirty ||
                      vset_digit_dirty || iset_digit_dirty ||
                      cpu_temp_dirty || buck_temp_dirty || boost_temp_dirty);

    if (!dirty)
    {
        return;
    }

    LCD_CopyRectFromFrontToDraw(
        0U,
        0U,
        LCD_DEV.width,
        LCD_DEV.height);

    if(vin_dirty)
    {
        GUI_DrawSmallValueTile(&g_top_tiles[0], data->vin);
    }

    if(iin_dirty)
    {
        GUI_DrawSmallValueTile(&g_top_tiles[1], data->iin);
    }

    if(pin_dirty)
    {
        GUI_DrawSmallValueTile(&g_top_tiles[2], data->pin);
    }

    if(efficiency_dirty)
    {
        GUI_DrawSmallValueTile(&g_top_tiles[3], data->efficiency);
    }

    if(fan_dirty)
    {
        GUI_DrawSmallValueTile(&g_top_tiles[4], data->fan);
    }

    if(vout_dirty)
    {
        GUI_DrawMainValueTile(&g_main_tiles[0], data->vout);
    }

    if(iout_dirty)
    {
        GUI_DrawMainValueTile(&g_main_tiles[1], data->iout);
    }

    if(pout_dirty)
    {
        GUI_DrawMainValueTile(&g_main_tiles[2], data->pout);
    }

    if(vset_dirty || vset_digit_dirty)
    {
        GUI_DrawSetValueTile(&g_set_tiles[0], data->vset, data->vset_digit);
    }

    if(iset_dirty || iset_digit_dirty)
    {
        GUI_DrawSetValueTile(&g_set_tiles[1], data->iset, data->iset_digit);
    }

    if(cpu_temp_dirty)
    {
        GUI_DrawTempTile(&g_temp_tiles[0], data->cpu_temp);
    }

    if(buck_temp_dirty)
    {
        GUI_DrawTempTile(&g_temp_tiles[1], data->buck_temp);
    }

    if(boost_temp_dirty)
    {
        GUI_DrawTempTile(&g_temp_tiles[2], data->boost_temp);
    }

    g_last_data = *data;
    g_gui_has_last = 1U;
    LCD_Present();
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
        (uint16_t)(tile->rect.x + 6U),
        (uint16_t)(tile->rect.y + 20U),
        300U,
        120U,
        buf,
        GUI_FONT_VALUE,
        60U,
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

static uint8_t GUI_FloatChanged(float a, float b)
{
    float diff = a - b;

    if (diff < 0.0F)
    {
        diff = -diff;
    }

    return diff >= 0.005F;
}
