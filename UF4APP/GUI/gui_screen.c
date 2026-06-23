#include "gui_internal.h"
#include "logo_image.h"

#include <stdio.h>
#include <string.h>

#include "bsp_font.h"
#include "bsp_lcd.h"
#include "main.h"

static GUI_Data_t g_last_data;
static uint8_t g_has_last;

static uint8_t GUI_DataEqual(const GUI_Data_t *a, const GUI_Data_t *b);
static uint8_t GUI_ScopeDataEqual(const GUI_Data_t *a, const GUI_Data_t *b);
static uint8_t GUI_ScopeSourceValueEqual(const GUI_Data_t *a, const GUI_Data_t *b, uint8_t source);
static float GUI_ScopeDataSourceValue(const GUI_Data_t *data, uint8_t source);
static void GUI_DrawBootLogo(void);
static void GUI_BlitLogoChunked(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *pixels);

void GUI_Init(void)
{
    g_has_last = 0U;
    memset(&g_last_data, 0, sizeof(g_last_data));
    GUI_DrawBootLogo();
    LCD_Present();
    HAL_Delay(700U);
    GUI_Clear();
    GUI_DrawMainPage(&g_last_data, NULL);
    LCD_Present();
    GUI_Clear();
    GUI_DrawMainPage(&g_last_data, NULL);
}

void GUI_Clear(void)
{
    LCD_Clear(GUI_BG_COLOR);
}

void GUI_DrawStatic(void)
{
    if (g_last_data.page != 0U)
    {
        GUI_DrawWavePage(&g_last_data);
    }
    else
    {
        GUI_DrawMainPage(&g_last_data, NULL);
    }
}

void GUI_Update(const GUI_Data_t *data)
{
    uint8_t need_full_clear;
    uint8_t page_changed;
    uint8_t data_equal;
    const GUI_Data_t *last_data;

    if (data == NULL)
    {
        return;
    }

    data_equal = 0U;
    if (g_has_last != 0U)
    {
        data_equal = (data->page != 0U)
            ? GUI_ScopeDataEqual(&g_last_data, data)
            : GUI_DataEqual(&g_last_data, data);
    }
    if (data_equal != 0U)
    {
        return;
    }

    page_changed = (uint8_t)(g_has_last == 0U || data->page != g_last_data.page);
    need_full_clear = page_changed;
    last_data = (page_changed == 0U && g_has_last != 0U) ? &g_last_data : NULL;

    if (need_full_clear != 0U)
    {
        GUI_Clear();
    }

    if (data->page != 0U)
    {
        GUI_DrawWavePage(data);
    }
    else
    {
        GUI_DrawMainPage(data, last_data);
    }
    LCD_Present();

    if (page_changed != 0U)
    {
        GUI_Clear();

        if (data->page != 0U)
        {
            GUI_DrawWavePage(data);
        }
        else
        {
            GUI_DrawMainPage(data, NULL);
        }
    }

    g_last_data = *data;
    g_has_last = 1U;
}

void GUI_DrawPanel(const GUI_Rect_t *rect, uint16_t color)
{
    LCD_Rect_Fill(
        rect->x,
        rect->y,
        (uint16_t)(rect->x + rect->w - 1U),
        (uint16_t)(rect->y + rect->h - 1U),
        color);
    GUI_DrawPanelBorder(rect, GUI_BORDER_COLOR);
}

void GUI_DrawPanelBorder(const GUI_Rect_t *rect, uint16_t color)
{
    uint16_t x2 = (uint16_t)(rect->x + rect->w - 1U);
    uint16_t y2 = (uint16_t)(rect->y + rect->h - 1U);

    LCD_Rect_Fill(rect->x, rect->y, x2, rect->y, color);
    LCD_Rect_Fill(rect->x, y2, x2, y2, color);
    LCD_Rect_Fill(rect->x, rect->y, rect->x, y2, color);
    LCD_Rect_Fill(x2, rect->y, x2, y2, color);
}

void GUI_DrawCenteredText(const GUI_Rect_t *rect, const char *text, uint16_t font, uint16_t color, uint16_t bg)
{
    uint16_t text_w;
    uint16_t text_min_row = 0U;
    uint16_t text_h = font;
    uint16_t x;
    uint16_t y;

    if (rect == NULL || text == NULL)
    {
        return;
    }

    text_w = LCD_MeasureFontString(text, font);
    x = rect->x;
    y = (uint16_t)(rect->y + (rect->h > font ? (rect->h - font) / 2U : 0U));

    if (text_w < rect->w)
    {
        x = (uint16_t)(rect->x + (rect->w - text_w) / 2U);
    }

    if (LCD_GetFontStringBBox(text, font, &text_min_row, &text_h) != 0U && rect->h > text_h)
    {
        y = (uint16_t)(rect->y + (rect->h - text_h) / 2U - text_min_row);
    }

    LCD_DrawFontStringDMATight(
        x,
        y,
        (uint16_t)(rect->x + rect->w - x),
        rect->h,
        text,
        font,
        color,
        bg);
}

void GUI_DrawFixedSlotText(const GUI_Rect_t *rect, const char *text, uint16_t font, uint16_t cell_w, uint16_t color, uint16_t bg)
{
    uint16_t text_len;
    uint16_t slot_w;
    uint16_t start_x;
    uint16_t y;
    uint16_t text_min_row = 0U;
    uint16_t text_h = font;
    uint32_t i;

    if (rect == NULL || text == NULL || cell_w == 0U)
    {
        return;
    }

    text_len = (uint16_t)strlen(text);
    slot_w = (uint16_t)(text_len * cell_w);
    start_x = rect->x;
    if (slot_w < rect->w)
    {
        start_x = (uint16_t)(rect->x + (rect->w - slot_w) / 2U);
    }

    y = (uint16_t)(rect->y + (rect->h > font ? (rect->h - font) / 2U : 0U));
    if (LCD_GetFontStringBBox(text, font, &text_min_row, &text_h) != 0U && rect->h > text_h)
    {
        y = (uint16_t)(rect->y + (rect->h - text_h) / 2U - text_min_row);
    }

    for (i = 0U; i < text_len; ++i)
    {
        char cell_text[2];
        uint16_t glyph_w;
        uint16_t glyph_x_offset = 0U;

        cell_text[0] = text[i];
        cell_text[1] = '\0';
        glyph_w = LCD_MeasureFontString(cell_text, font);
        if (glyph_w < cell_w)
        {
            glyph_x_offset = (uint16_t)((cell_w - glyph_w) / 2U);
        }

        LCD_DrawFontStringDMATight(
            (uint16_t)(start_x + i * cell_w + glyph_x_offset),
            y,
            (uint16_t)(cell_w - glyph_x_offset),
            rect->h,
            cell_text,
            font,
            color,
            bg);
    }
}

void GUI_FormatMainValue(char *buf, uint32_t size, float value)
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

void GUI_FormatFixed2(char *buf, uint32_t size, float value)
{
    snprintf(buf, size, "%05.2f", value);
}

void GUI_DrawTopTile(const GUI_ValueTile_t *tile, float value)
{
    char buf[16];
    GUI_Rect_t title_rect;
    GUI_Rect_t value_rect;
    GUI_Rect_t unit_rect;

    GUI_FormatMainValue(buf, sizeof(buf), value);
    GUI_DrawPanel(&tile->rect, GUI_PANEL_COLOR);

    title_rect = (GUI_Rect_t){tile->rect.x, (uint16_t)(tile->rect.y + 2U), tile->rect.w, 18U};
    value_rect = (GUI_Rect_t){(uint16_t)(tile->rect.x + 8U), (uint16_t)(tile->rect.y + 15U), (uint16_t)(tile->rect.w - 30U), 34U};
    unit_rect = (GUI_Rect_t){(uint16_t)(tile->rect.x + tile->rect.w - 22U), (uint16_t)(tile->rect.y + 24U), 18U, 24U};

    GUI_DrawCenteredText(&title_rect, tile->title, GUI_FONT_TOP_LABEL, GUI_MUTED_COLOR, GUI_PANEL_COLOR);
    LCD_Rect_Fill(value_rect.x, value_rect.y, (uint16_t)(value_rect.x + value_rect.w - 1U), (uint16_t)(value_rect.y + value_rect.h - 1U), GUI_PANEL_COLOR);
    GUI_DrawFixedSlotText(&value_rect, buf, GUI_FONT_TOP_VALUE, GUI_TOP_VALUE_CELL_W, GUI_TEXT_COLOR, GUI_PANEL_COLOR);
    GUI_DrawCenteredText(&unit_rect, tile->unit, GUI_FONT_LABEL, GUI_ACCENT_COLOR, GUI_PANEL_COLOR);
}

/**
  * @brief Refresh only the numeric area of a top status tile.
  */
void GUI_DrawTopTileValue(const GUI_ValueTile_t *tile, float value)
{
    char buf[16];
    GUI_Rect_t value_rect;

    if (tile == NULL)
    {
        return;
    }

    GUI_FormatMainValue(buf, sizeof(buf), value);
    value_rect = (GUI_Rect_t){(uint16_t)(tile->rect.x + 8U), (uint16_t)(tile->rect.y + 15U), (uint16_t)(tile->rect.w - 30U), 34U};

    LCD_Rect_Fill(value_rect.x,
                  value_rect.y,
                  (uint16_t)(value_rect.x + value_rect.w - 1U),
                  (uint16_t)(value_rect.y + value_rect.h - 1U),
                  GUI_PANEL_COLOR);
    GUI_DrawFixedSlotText(&value_rect, buf, GUI_FONT_TOP_VALUE, GUI_TOP_VALUE_CELL_W, GUI_TEXT_COLOR, GUI_PANEL_COLOR);
}

void GUI_DrawMainTile(const GUI_ValueTile_t *tile, float value)
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

    title_rect = (GUI_Rect_t){(uint16_t)(tile->rect.x + tile->rect.w - 40U), (uint16_t)(tile->rect.y + 42U), 34U, 24U};
    unit_rect = (GUI_Rect_t){(uint16_t)(tile->rect.x + tile->rect.w - 38U), (uint16_t)(tile->rect.y + 74U), 32U, 40U};
    GUI_DrawCenteredText(&title_rect, tile->title, GUI_FONT_LABEL, GUI_MUTED_COLOR, GUI_PANEL_DARK);
    GUI_DrawCenteredText(&unit_rect, tile->unit, GUI_FONT_LABEL, GUI_ACCENT_COLOR, GUI_PANEL_DARK);
}

/**
  * @brief Refresh only the large numeric area of a main value tile.
  */
void GUI_DrawMainTileValue(const GUI_ValueTile_t *tile, float value)
{
    char buf[16];

    if (tile == NULL)
    {
        return;
    }

    GUI_FormatMainValue(buf, sizeof(buf), value);
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
}

void GUI_DrawSetTile(const GUI_ValueTile_t *tile, float value, uint8_t digit)
{
    char buf[16];
    GUI_Rect_t title_rect;
    GUI_Rect_t value_rect;
    GUI_Rect_t unit_rect;
    uint16_t mark_x;
    uint8_t char_index;

    GUI_FormatFixed2(buf, sizeof(buf), value);
    GUI_DrawPanel(&tile->rect, GUI_PANEL_COLOR);

    title_rect = (GUI_Rect_t){tile->rect.x, (uint16_t)(tile->rect.y + 2U), tile->rect.w, 18U};
    value_rect = (GUI_Rect_t){(uint16_t)(tile->rect.x + 10U), (uint16_t)(tile->rect.y + tile->rect.h - 39U), GUI_SET_TEXT_W, 34U};
    unit_rect = (GUI_Rect_t){(uint16_t)(tile->rect.x + tile->rect.w - 24U), (uint16_t)(value_rect.y + 5U), 22U, 24U};

    GUI_DrawCenteredText(&title_rect, tile->title, GUI_FONT_TILE_TITLE, GUI_MUTED_COLOR, GUI_PANEL_COLOR);
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
    GUI_DrawCenteredText(&unit_rect, tile->unit, GUI_FONT_TILE_VALUE, GUI_ACCENT_COLOR, GUI_PANEL_COLOR);

    if (digit < 4U)
    {
        char_index = (digit < 2U) ? digit : (uint8_t)(digit + 1U);
        mark_x = (uint16_t)(value_rect.x + char_index * GUI_SET_CELL_W);
        LCD_Rect_Fill(
            mark_x,
            (uint16_t)(tile->rect.y + tile->rect.h - 4U),
            (uint16_t)(mark_x + GUI_SET_CELL_W - 2U),
            (uint16_t)(tile->rect.y + tile->rect.h - 3U),
            GUI_ACCENT_COLOR);
    }
}

void GUI_DrawProtectTile(const GUI_LabelTile_t *tile, float value, const char *unit, uint8_t enabled, uint8_t selected)
{
    char buf[16];
    char value_buf[20];
    GUI_Rect_t title_rect;
    GUI_Rect_t value_rect;

    if (enabled != 0U)
    {
        GUI_FormatFixed2(buf, sizeof(buf), value);
        snprintf(value_buf, sizeof(value_buf), "%s%s", buf, unit);
    }
    else
    {
        snprintf(value_buf, sizeof(value_buf), "OFF");
    }

    GUI_DrawPanel(&tile->rect, GUI_PANEL_COLOR);
    title_rect = (GUI_Rect_t){(uint16_t)(tile->rect.x + 4U), (uint16_t)(tile->rect.y + 11U), 52U, 20U};
    value_rect = (GUI_Rect_t){(uint16_t)(tile->rect.x + 58U), (uint16_t)(tile->rect.y + 9U), (uint16_t)(tile->rect.w - 62U), 24U};
    GUI_DrawCenteredText(&title_rect, tile->title, GUI_FONT_TILE_TITLE, GUI_MUTED_COLOR, GUI_PANEL_COLOR);
    GUI_DrawFixedSlotText(&value_rect, value_buf, GUI_FONT_TILE_VALUE, GUI_TILE_VALUE_CELL_W, GUI_TEXT_COLOR, GUI_PANEL_COLOR);
    if (selected != 0U)
    {
        GUI_DrawPanelBorder(&tile->rect, GUI_ACCENT_COLOR);
    }
}

void GUI_DrawStateTile(const GUI_LabelTile_t *tile, const char *value, uint16_t fill_color, uint8_t selected)
{
    GUI_Rect_t title_rect;
    GUI_Rect_t value_rect;

    GUI_DrawPanel(&tile->rect, fill_color);
    title_rect = (GUI_Rect_t){(uint16_t)(tile->rect.x + 4U), (uint16_t)(tile->rect.y + 11U), 52U, 20U};
    value_rect = (GUI_Rect_t){(uint16_t)(tile->rect.x + 58U), (uint16_t)(tile->rect.y + 9U), (uint16_t)(tile->rect.w - 62U), 24U};
    GUI_DrawCenteredText(&title_rect, tile->title, GUI_FONT_TILE_TITLE, GUI_MUTED_COLOR, fill_color);
    GUI_DrawCenteredText(&value_rect, value, GUI_FONT_TILE_VALUE, GUI_TEXT_COLOR, fill_color);
    if (selected != 0U)
    {
        GUI_DrawPanelBorder(&tile->rect, GUI_ACCENT_COLOR);
    }
}

void GUI_DrawTempTile(const GUI_ValueTile_t *tile, float value)
{
    char buf[16];
    GUI_Rect_t title_rect;
    GUI_Rect_t value_rect;

    GUI_FormatFixed2(buf, sizeof(buf), value);
    GUI_DrawPanel(&tile->rect, GUI_PANEL_COLOR);
    title_rect = (GUI_Rect_t){(uint16_t)(tile->rect.x + 4U), (uint16_t)(tile->rect.y + 11U), 52U, 20U};
    value_rect = (GUI_Rect_t){(uint16_t)(tile->rect.x + 56U), (uint16_t)(tile->rect.y + 9U), (uint16_t)(tile->rect.w - 60U), 24U};
    GUI_DrawCenteredText(&title_rect, tile->title, GUI_FONT_TILE_TITLE, GUI_MUTED_COLOR, GUI_PANEL_COLOR);
    GUI_DrawFixedSlotText(&value_rect, buf, GUI_FONT_TILE_VALUE, GUI_TILE_VALUE_CELL_W, GUI_TEXT_COLOR, GUI_PANEL_COLOR);
}

const char *GUI_ModeText(uint8_t mode)
{
    return (mode != 0U) ? "CV" : "CC";
}

const char *GUI_StateText(uint8_t state)
{
    switch (state)
    {
        case 0x01U: return "INIT";
        case 0x02U: return "WAIT";
        case 0x04U: return "RISE";
        case 0x08U: return "RUN";
        case 0x0FU: return "ERR";
        default: return "NA";
    }
}

const char *GUI_FaultText(uint8_t fault)
{
    uint16_t fault_bits = fault;

    if (fault_bits == 0x0000U)
    {
        return "NA";
    }

    if ((fault_bits & 0x0008U) != 0U)
    {
        return "OVP";
    }
    if ((fault_bits & 0x0010U) != 0U)
    {
        return "OCP";
    }
    if ((fault_bits & 0x0040U) != 0U)
    {
        return "OTP";
    }

    return "ERR";
}

const char *GUI_TopoText(const GUI_Data_t *data)
{
    if (data == NULL)
    {
        return "NA";
    }

    if ((data->valid_flags & GUI_VALID_STATE_MACHINE_STATE) == 0U)
    {
        return "NA";
    }

    switch (data->state_machine_state)
    {
        case 1U: return "BUCK";
        case 2U: return "BOOST";
        case 3U: return "MIX";
        case 0U: return "NA";
        default: return "NA";
    }
}

static uint8_t GUI_DataEqual(const GUI_Data_t *a, const GUI_Data_t *b)
{
    return (uint8_t)(memcmp(a, b, sizeof(GUI_Data_t)) == 0);
}

/**
  * @brief Compare only fields that are rendered by the scope page.
  */
static uint8_t GUI_ScopeDataEqual(const GUI_Data_t *a, const GUI_Data_t *b)
{
    uint8_t scope_valid_mask = GUI_VALID_FAULT_STATE | GUI_VALID_STATE_MACHINE_STATE;

    if (a->page != b->page ||
        a->scope_field != b->scope_field ||
        a->scope_timebase != b->scope_timebase ||
        a->scope_ch1_enabled != b->scope_ch1_enabled ||
        a->scope_ch2_enabled != b->scope_ch2_enabled ||
        a->scope_hold != b->scope_hold ||
        a->scope_trigger != b->scope_trigger ||
        a->scope_ch1_source != b->scope_ch1_source ||
        a->scope_ch2_source != b->scope_ch2_source ||
        a->scope_ch1_scale != b->scope_ch1_scale ||
        a->scope_ch2_scale != b->scope_ch2_scale ||
        a->vset_digit != b->vset_digit ||
        a->iset_digit != b->iset_digit ||
        a->fault_state != b->fault_state ||
        a->state_machine_state != b->state_machine_state ||
        (a->valid_flags & scope_valid_mask) != (b->valid_flags & scope_valid_mask))
    {
        return 0U;
    }

    if (a->vset != b->vset || a->iset != b->iset)
    {
        return 0U;
    }

    if (a->scope_hold == 0U)
    {
        if (a->scope_ch1_enabled != 0U &&
            GUI_ScopeSourceValueEqual(a, b, a->scope_ch1_source) == 0U)
        {
            return 0U;
        }

        if (a->scope_ch2_enabled != 0U &&
            GUI_ScopeSourceValueEqual(a, b, a->scope_ch2_source) == 0U)
        {
            return 0U;
        }
    }

    return 1U;
}

/**
  * @brief Compare the telemetry value currently bound to a scope channel.
  */
static uint8_t GUI_ScopeSourceValueEqual(const GUI_Data_t *a, const GUI_Data_t *b, uint8_t source)
{
    return (uint8_t)(GUI_ScopeDataSourceValue(a, source) == GUI_ScopeDataSourceValue(b, source));
}

static float GUI_ScopeDataSourceValue(const GUI_Data_t *data, uint8_t source)
{
    switch (source)
    {
        case 1U: return data->iout;
        case 2U: return data->pout;
        case 3U: return data->vin;
        case 4U: return data->iin;
        case 5U: return data->pin;
        case 6U: return data->vset;
        case 7U: return data->iset;
        case 8U: return data->efficiency;
        case 9U: return data->fan;
        case 0U:
        default:
            return data->vout;
    }
}

static void GUI_DrawBootLogo(void)
{
    uint16_t x;
    uint16_t y;

    GUI_Clear();

    x = (uint16_t)((LCD_LOGICAL_LANDSCAPE_WIDTH - G_LOGO_IMAGE_DATA_W) / 2U);
    y = (uint16_t)((LCD_LOGICAL_LANDSCAPE_HEIGHT - G_LOGO_IMAGE_DATA_H) / 2U);

    GUI_BlitLogoChunked(
        x,
        y,
        G_LOGO_IMAGE_DATA_W,
        G_LOGO_IMAGE_DATA_H,
        g_logo_image_data);
}

static void GUI_BlitLogoChunked(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *pixels)
{
    uint16_t chunk_y = 0U;
    const uint16_t chunk_h_max = 120U;

    while (chunk_y < h)
    {
        uint16_t chunk_h = (uint16_t)(h - chunk_y);
        const uint16_t *chunk_pixels;

        if (chunk_h > chunk_h_max)
        {
            chunk_h = chunk_h_max;
        }

        chunk_pixels = pixels + (uint32_t)chunk_y * w;
        LCD_BlitRectRGB565(
            x,
            (uint16_t)(y + chunk_y),
            w,
            chunk_h,
            chunk_pixels);

        chunk_y = (uint16_t)(chunk_y + chunk_h);
    }
}
