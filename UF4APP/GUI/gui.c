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

#define GUI_FONT_LABEL      24U
#define GUI_FONT_VALUE      72U

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

static const GUI_ValueTile_t g_top_tiles[] =
{
    {{  8,  8, 116, 52}, "VIN", "%"},
    {{132,  8, 116, 52}, "IIN", "A"},
    {{256,  8, 116, 52}, "PIN", "W"},
    {{380,  8, 116, 52}, "EFF", "%"},
    {{504,  8, 128, 52}, "FAN", "%"},
};

static const GUI_ValueTile_t g_main_tiles[] =
{
    {{  8,  72, 416, 104}, "VOUT", "V"},
    {{  8, 192, 416, 104}, "IOUT", "A"},
    {{  8, 312, 416, 104}, "POUT", "W"},
};

static const GUI_ValueTile_t g_set_tiles[] =
{
    {{440,  72,  82, 52}, "VSET", "V"},
    {{440, 132,  82, 52}, "ISET", "A"},
};

static const GUI_Rect_t g_status_tiles[] =
{
    {532,  72,  96, 32},
    {532, 112,  96, 32},
    {532, 152,  96, 32},
    {440, 204,  82, 32},
    {440, 244,  82, 32},
    {440, 284,  82, 32},
    {532, 204,  96, 32},
    {532, 244,  96, 32},
    {532, 284,  96, 32},
    {532, 376,  96, 32},
};

static const char * const g_status_labels[] =
{
    "OTP",
    "OVP",
    "OCP",
    "OUT",
    "TOPO",
    "MODE",
    "CMD",
    "FSM",
    "FAULT",
    "BLE",
};

static const GUI_ValueTile_t g_temp_tiles[] =
{
    {{440, 376,  82, 32}, "CPU", "C"},
    {{440, 416,  82, 32}, "BUCK", "C"},
    {{532, 416,  96, 32}, "BOOST", "C"},
};

static void GUI_DrawPanel(const GUI_Rect_t *rect, uint32_t color);
static void GUI_DrawCenteredText(const GUI_Rect_t *rect, const char *text, uint16_t font, uint32_t color, uint32_t bg);
static void GUI_DrawSmallValueTile(const GUI_ValueTile_t *tile, float value);
static void GUI_DrawMainValueTile(const GUI_ValueTile_t *tile, float value);
static void GUI_DrawSetValueTile(const GUI_ValueTile_t *tile, float value);
static void GUI_DrawTempTile(const GUI_ValueTile_t *tile, float value);
static void GUI_DrawStatusTile(const GUI_Rect_t *rect, const char *label);
static uint8_t GUI_FloatChanged(float a, float b);

void GUI_Init(void)
{
    GUI_Clear();

    GUI_DrawStatic();
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
        GUI_DrawStatusTile(
            &g_status_tiles[i],
            g_status_labels[i]);
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
    if(data == NULL)
    {
        return;
    }

    if(!g_gui_has_last || GUI_FloatChanged(data->vin, g_last_data.vin))
    {
        GUI_DrawSmallValueTile(&g_top_tiles[0], data->vin);
    }

    if(!g_gui_has_last || GUI_FloatChanged(data->iin, g_last_data.iin))
    {
        GUI_DrawSmallValueTile(&g_top_tiles[1], data->iin);
    }

    if(!g_gui_has_last || GUI_FloatChanged(data->pin, g_last_data.pin))
    {
        GUI_DrawSmallValueTile(&g_top_tiles[2], data->pin);
    }

    if(!g_gui_has_last || GUI_FloatChanged(data->efficiency, g_last_data.efficiency))
    {
        GUI_DrawSmallValueTile(&g_top_tiles[3], data->efficiency);
    }

    if(!g_gui_has_last || GUI_FloatChanged(data->fan, g_last_data.fan))
    {
        GUI_DrawSmallValueTile(&g_top_tiles[4], data->fan);
    }

    if(!g_gui_has_last || GUI_FloatChanged(data->vout, g_last_data.vout))
    {
        GUI_DrawMainValueTile(&g_main_tiles[0], data->vout);
    }

    if(!g_gui_has_last || GUI_FloatChanged(data->iout, g_last_data.iout))
    {
        GUI_DrawMainValueTile(&g_main_tiles[1], data->iout);
    }

    if(!g_gui_has_last || GUI_FloatChanged(data->pout, g_last_data.pout))
    {
        GUI_DrawMainValueTile(&g_main_tiles[2], data->pout);
    }

    if(!g_gui_has_last || GUI_FloatChanged(data->vset, g_last_data.vset))
    {
        GUI_DrawSetValueTile(&g_set_tiles[0], data->vset);
    }

    if(!g_gui_has_last || GUI_FloatChanged(data->iset, g_last_data.iset))
    {
        GUI_DrawSetValueTile(&g_set_tiles[1], data->iset);
    }

    if(!g_gui_has_last || GUI_FloatChanged(data->cpu_temp, g_last_data.cpu_temp))
    {
        GUI_DrawTempTile(&g_temp_tiles[0], data->cpu_temp);
    }

    if(!g_gui_has_last || GUI_FloatChanged(data->buck_temp, g_last_data.buck_temp))
    {
        GUI_DrawTempTile(&g_temp_tiles[1], data->buck_temp);
    }

    if(!g_gui_has_last || GUI_FloatChanged(data->boost_temp, g_last_data.boost_temp))
    {
        GUI_DrawTempTile(&g_temp_tiles[2], data->boost_temp);
    }

    g_last_data = *data;
    g_gui_has_last = 1U;
}

static void GUI_DrawPanel(const GUI_Rect_t *rect, uint32_t color)
{
    LCD_Rect_Fill(
        rect->x,
        rect->y,
        (uint16_t)(rect->x + rect->w - 1U),
        (uint16_t)(rect->y + rect->h - 1U),
        color);

    POINT_COLOR = GUI_BORDER_COLOR;
    LCD_DrawRectangle(
        rect->x,
        rect->y,
        (uint16_t)(rect->x + rect->w - 1U),
        (uint16_t)(rect->y + rect->h - 1U));
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

    snprintf(buf, sizeof(buf), "%04.1f", value);

    GUI_DrawPanel(&tile->rect, GUI_PANEL_COLOR);

    title_rect.x = tile->rect.x;
    title_rect.y = (uint16_t)(tile->rect.y + 2U);
    title_rect.w = tile->rect.w;
    title_rect.h = 20U;
    GUI_DrawCenteredText(&title_rect, tile->title, GUI_FONT_LABEL, GUI_MUTED_COLOR, GUI_PANEL_COLOR);

    value_rect.x = (uint16_t)(tile->rect.x + 8U);
    value_rect.y = (uint16_t)(tile->rect.y + 24U);
    value_rect.w = (uint16_t)(tile->rect.w - 34U);
    value_rect.h = 24U;
    LCD_DrawFontStringDMA(value_rect.x, value_rect.y, value_rect.w, value_rect.h, buf, GUI_FONT_LABEL, GUI_TEXT_COLOR, GUI_PANEL_COLOR);

    value_rect.x = (uint16_t)(tile->rect.x + tile->rect.w - 24U);
    value_rect.w = 20U;
    GUI_DrawCenteredText(&value_rect, tile->unit, GUI_FONT_LABEL, GUI_ACCENT_COLOR, GUI_PANEL_COLOR);
}

static void GUI_DrawMainValueTile(const GUI_ValueTile_t *tile, float value)
{
    char buf[16];
    GUI_Rect_t title_rect;
    GUI_Rect_t unit_rect;

    snprintf(buf, sizeof(buf), "%05.2f", value);

    GUI_DrawPanel(&tile->rect, GUI_PANEL_DARK);

    title_rect.x = (uint16_t)(tile->rect.x + 12U);
    title_rect.y = (uint16_t)(tile->rect.y + 4U);
    title_rect.w = 96U;
    title_rect.h = 24U;
    LCD_DrawFontStringDMA(title_rect.x, title_rect.y, title_rect.w, title_rect.h, tile->title, GUI_FONT_LABEL, GUI_MUTED_COLOR, GUI_PANEL_DARK);

    LCD_DrawFontStringDMA(
        (uint16_t)(tile->rect.x + 14U),
        (uint16_t)(tile->rect.y + 30U),
        310U,
        72U,
        buf,
        GUI_FONT_VALUE,
        GUI_TEXT_COLOR,
        GUI_PANEL_DARK);

    unit_rect.x = (uint16_t)(tile->rect.x + tile->rect.w - 72U);
    unit_rect.y = (uint16_t)(tile->rect.y + 40U);
    unit_rect.w = 56U;
    unit_rect.h = 40U;
    GUI_DrawCenteredText(&unit_rect, tile->unit, GUI_FONT_LABEL, GUI_ACCENT_COLOR, GUI_PANEL_DARK);
}

static void GUI_DrawSetValueTile(const GUI_ValueTile_t *tile, float value)
{
    char buf[16];
    GUI_Rect_t title_rect;
    GUI_Rect_t unit_rect;

    snprintf(buf, sizeof(buf), "%04.1f", value);

    GUI_DrawPanel(&tile->rect, GUI_PANEL_COLOR);

    title_rect.x = tile->rect.x;
    title_rect.y = (uint16_t)(tile->rect.y + 2U);
    title_rect.w = tile->rect.w;
    title_rect.h = 20U;
    GUI_DrawCenteredText(&title_rect, tile->title, GUI_FONT_LABEL, GUI_MUTED_COLOR, GUI_PANEL_COLOR);

    LCD_DrawFontStringDMA((uint16_t)(tile->rect.x + 8U), (uint16_t)(tile->rect.y + 24U), 48U, 24U, buf, GUI_FONT_LABEL, GUI_TEXT_COLOR, GUI_PANEL_COLOR);

    unit_rect.x = (uint16_t)(tile->rect.x + tile->rect.w - 22U);
    unit_rect.y = (uint16_t)(tile->rect.y + 24U);
    unit_rect.w = 18U;
    unit_rect.h = 24U;
    GUI_DrawCenteredText(&unit_rect, tile->unit, GUI_FONT_LABEL, GUI_ACCENT_COLOR, GUI_PANEL_COLOR);
}

static void GUI_DrawTempTile(const GUI_ValueTile_t *tile, float value)
{
    char buf[16];
    GUI_Rect_t title_rect;
    GUI_Rect_t unit_rect;

    snprintf(buf, sizeof(buf), "%02.0f", value);

    GUI_DrawPanel(&tile->rect, GUI_PANEL_COLOR);

    title_rect.x = (uint16_t)(tile->rect.x + 4U);
    title_rect.y = (uint16_t)(tile->rect.y + 4U);
    title_rect.w = (uint16_t)(tile->rect.w - 36U);
    title_rect.h = 24U;
    LCD_DrawFontStringDMA(title_rect.x, title_rect.y, title_rect.w, title_rect.h, tile->title, GUI_FONT_LABEL, GUI_MUTED_COLOR, GUI_PANEL_COLOR);

    LCD_DrawFontStringDMA((uint16_t)(tile->rect.x + tile->rect.w - 34U), (uint16_t)(tile->rect.y + 4U), 20U, 24U, buf, GUI_FONT_LABEL, GUI_TEXT_COLOR, GUI_PANEL_COLOR);

    unit_rect.x = (uint16_t)(tile->rect.x + tile->rect.w - 14U);
    unit_rect.y = (uint16_t)(tile->rect.y + 4U);
    unit_rect.w = 12U;
    unit_rect.h = 24U;
    GUI_DrawCenteredText(&unit_rect, tile->unit, GUI_FONT_LABEL, GUI_ACCENT_COLOR, GUI_PANEL_COLOR);
}

static void GUI_DrawStatusTile(const GUI_Rect_t *rect, const char *label)
{
    GUI_DrawPanel(rect, GUI_PANEL_COLOR);
    GUI_DrawCenteredText(rect, label, GUI_FONT_LABEL, GUI_MUTED_COLOR, GUI_PANEL_COLOR);
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
