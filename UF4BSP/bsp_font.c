/**
  ******************************************************************************
  * @file    bsp_font.c
  * @author  UF4
  * @date    26-6-21 下午9:52
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
#include "bsp_font.h"
#include "bsp_lcd.h"

static uint16_t g_font_dma_buffer[540U * 176U] __attribute__((section(".sdram"), aligned(32)));

#define LCD_FONT_CACHE_SIZE_VALUE   144U
#define LCD_FONT_CACHE_CELL_W       60U
#define LCD_FONT_CACHE_CELL_H       104U
#define LCD_FONT_CACHE_PHYS_W       LCD_FONT_CACHE_CELL_H
#define LCD_FONT_CACHE_PHYS_H       LCD_FONT_CACHE_CELL_W
#define LCD_FONT_CACHE_GLYPHS       13U

static uint16_t g_font_value_cache[LCD_FONT_CACHE_GLYPHS][LCD_FONT_CACHE_PHYS_W * LCD_FONT_CACHE_PHYS_H]
        __attribute__((section(".sdram"), aligned(32)));
static uint8_t g_font_value_cache_ready;

static int32_t LCD_FontValueCacheIndex(char ch)
{
    if (ch >= '0' && ch <= '9')
    {
        return ch - '0';
    }
    if (ch == '.')
    {
        return 10;
    }
    if (ch == '-')
    {
        return 11;
    }
    if (ch == ' ')
    {
        return 12;
    }
    return -1;
}

static void LCD_FontValueCacheBuildGlyph(char ch, uint16_t *dst)
{
    uint32_t i;
    uint32_t lx;
    uint32_t ly;
    uint16_t bg = LCD_EncodeColor((uint16_t)0x0841);

    for (i = 0U; i < LCD_FONT_CACHE_CELL_W * LCD_FONT_CACHE_CELL_H; ++i)
    {
        g_font_dma_buffer[i] = bg;
    }

    if (ch != ' ')
    {
        char text[2];

        text[0] = ch;
        text[1] = '\0';
        LCD_RenderFontStringFixedToBuffer(
            g_font_dma_buffer,
            LCD_FONT_CACHE_CELL_W,
            LCD_FONT_CACHE_CELL_H,
            0U,
            0U,
            text,
            LCD_FONT_CACHE_SIZE_VALUE,
            LCD_FONT_CACHE_CELL_W,
            0xFFFF);
    }

    for (ly = 0U; ly < LCD_FONT_CACHE_CELL_H; ++ly)
    {
        for (lx = 0U; lx < LCD_FONT_CACHE_CELL_W; ++lx)
        {
            dst[(uint32_t)(LCD_FONT_CACHE_CELL_W - 1U - lx) * LCD_FONT_CACHE_PHYS_W + ly] =
                g_font_dma_buffer[ly * LCD_FONT_CACHE_CELL_W + lx];
        }
    }
}

static void LCD_FontValueCacheEnsure(void)
{
    static const char glyphs[LCD_FONT_CACHE_GLYPHS] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '-', ' '
    };
    uint32_t i;

    if (g_font_value_cache_ready != 0U)
    {
        return;
    }

    for (i = 0U; i < LCD_FONT_CACHE_GLYPHS; ++i)
    {
        LCD_FontValueCacheBuildGlyph(glyphs[i], g_font_value_cache[i]);
    }

    g_font_value_cache_ready = 1U;
}

static inline uint8_t LCD_FontBytesPerRow(
        uint16_t size)
{
    return (size + 7U) >> 3;
}

static inline uint8_t LCD_GlyphPixelIsSet(
        const uint8_t *bitmap,
        uint16_t size,
        uint16_t row,
        uint16_t col)
{
    uint8_t bytes_per_row =
            LCD_FontBytesPerRow(size);

    const uint8_t bits =
            bitmap[row * bytes_per_row +
                   (col >> 3)];

    return (bits &
           (0x80U >> (col & 7U))) != 0U;
}

uint16_t LCD_GetFontAdvance(
        uint8_t ch,
        uint16_t size)
{
    const LCD_FontGlyph *glyph;

    if (ch == ' ')
    {
        return size / 2;
    }

    glyph =
        Teko_SemiBold_FindGlyph(
            (char)ch,
            size);

    if (!glyph)
    {
        return size / 2;
    }

    return glyph->advance;
}

uint16_t LCD_MeasureFontString(
        const char *str,
        uint16_t size)
{
    uint16_t width = 0U;

    if (!str)
    {
        return 0U;
    }

    while (*str && *str != '\n')
    {
        width +=
            LCD_GetFontAdvance(
                (uint8_t)*str,
                size);

        str++;
    }

    return width;
}

void LCD_RenderFontStringToBuffer(
        uint16_t *buffer,
        uint16_t width,
        uint16_t height,
        uint16_t x,
        uint16_t y,
        const char *str,
        uint16_t size,
        uint32_t color)
{
    uint16_t cursor = x;
    uint16_t fg = LCD_EncodeColor((uint16_t)color);

    if (!buffer || !str)
    {
        return;
    }

    while (*str)
    {
        const LCD_FontGlyph *glyph;
        uint16_t row;
        uint16_t col;

        if (*str == '\n')
        {
            cursor = x;
            y += size;
            str++;
            continue;
        }

        glyph =
            Teko_SemiBold_FindGlyph(
                *str,
                size);

        if (glyph)
        {
            for (row = glyph->min_row;
                 row < glyph->min_row + glyph->height;
                 row++)
            {
                uint16_t dst_y =
                    y + row -
                    glyph->min_row;

                if (dst_y >= height)
                {
                    continue;
                }

                for (col = glyph->min_col;
                     col < glyph->min_col + glyph->width;
                     col++)
                {
                    uint16_t dst_x =
                        cursor + col -
                        glyph->min_col;

                    if (dst_x >= width)
                    {
                        continue;
                    }

                    if (LCD_GlyphPixelIsSet(
                            glyph->bitmap,
                            size,
                            row,
                            col))
                    {
                        buffer[(uint32_t)dst_y * width + dst_x] =
                            fg;
                    }
                }
            }
        }

        cursor +=
            LCD_GetFontAdvance(
                (uint8_t)*str,
                size);

        str++;
    }
}

void LCD_RenderFontStringFixedToBuffer(
        uint16_t *buffer,
        uint16_t width,
        uint16_t height,
        uint16_t x,
        uint16_t y,
        const char *str,
        uint16_t size,
        uint16_t cell_width,
        uint32_t color)
{
    uint16_t cell_x = x;
    uint16_t fg = LCD_EncodeColor((uint16_t)color);

    if (!buffer || !str || cell_width == 0U)
    {
        return;
    }

    while (*str)
    {
        const LCD_FontGlyph *glyph;
        uint16_t row;
        uint16_t col;
        uint16_t glyph_x;

        if (*str == '\n')
        {
            cell_x = x;
            y += size;
            str++;
            continue;
        }

        glyph =
            Teko_SemiBold_FindGlyph(
                *str,
                size);

        if (glyph)
        {
            glyph_x = cell_x;
            if (glyph->width < cell_width)
            {
                glyph_x = (uint16_t)(cell_x + (cell_width - glyph->width) / 2U);
            }

            for (row = glyph->min_row;
                 row < glyph->min_row + glyph->height;
                 row++)
            {
                uint16_t dst_y =
                    y + row -
                    glyph->min_row;

                if (dst_y >= height)
                {
                    continue;
                }

                for (col = glyph->min_col;
                     col < glyph->min_col + glyph->width;
                     col++)
                {
                    uint16_t dst_x =
                        glyph_x + col -
                        glyph->min_col;

                    if (dst_x >= width)
                    {
                        continue;
                    }

                    if (LCD_GlyphPixelIsSet(
                            glyph->bitmap,
                            size,
                            row,
                            col))
                    {
                        buffer[(uint32_t)dst_y * width + dst_x] =
                            fg;
                    }
                }
            }
        }

        cell_x = (uint16_t)(cell_x + cell_width);
        str++;
    }
}

void LCD_DrawFontStringDMA(
        uint16_t x,
        uint16_t y,
        uint16_t w,
        uint16_t h,
        const char *str,
        uint16_t size,
        uint32_t color,
        uint32_t bg_color)
{
    uint32_t i;
    uint16_t bg;

    if (!str || w == 0U || h == 0U)
    {
        return;
    }
    if ((uint32_t)w * h > (uint32_t)(sizeof(g_font_dma_buffer) / sizeof(g_font_dma_buffer[0])))
    {
        return;
    }

    bg =
        LCD_EncodeColor(
            (uint16_t)(bg_color == LCD_FONT_BG_TRANSPARENT ? 0U : bg_color));

    for (i = 0U; i < (uint32_t)w * h; ++i)
    {
        g_font_dma_buffer[i] = bg;
    }

    LCD_RenderFontStringToBuffer(
        g_font_dma_buffer,
        w,
        h,
        0U,
        0U,
        str,
        size,
        color);

    LCD_BlitRectRGB565(
        x,
        y,
        w,
        h,
        g_font_dma_buffer);
}

void LCD_DrawFontStringFixedDMA(
        uint16_t x,
        uint16_t y,
        uint16_t w,
        uint16_t h,
        const char *str,
        uint16_t size,
        uint16_t cell_width,
        uint32_t color,
        uint32_t bg_color)
{
    uint32_t i;
    uint16_t bg;

    if (!str || w == 0U || h == 0U || cell_width == 0U)
    {
        return;
    }
    if ((uint32_t)w * h > (uint32_t)(sizeof(g_font_dma_buffer) / sizeof(g_font_dma_buffer[0])))
    {
        return;
    }

    if (size == LCD_FONT_CACHE_SIZE_VALUE &&
        cell_width == LCD_FONT_CACHE_CELL_W &&
        h == LCD_FONT_CACHE_CELL_H &&
        bg_color == 0x0841 &&
        color == 0xFFFF)
    {
        uint16_t cell_x = x;
        const char *p = str;

        LCD_FontValueCacheEnsure();
        while (*p != '\0' && (uint32_t)(cell_x - x) < w)
        {
            int32_t cache_index = LCD_FontValueCacheIndex(*p);

            if (cache_index < 0)
            {
                break;
            }

            LCD_BlitRotatedRectRGB565(
                cell_x,
                y,
                LCD_FONT_CACHE_CELL_W,
                LCD_FONT_CACHE_CELL_H,
                g_font_value_cache[cache_index]);
            cell_x = (uint16_t)(cell_x + cell_width);
            p++;
        }

        if (*p == '\0')
        {
            return;
        }
    }

    bg =
        LCD_EncodeColor(
            (uint16_t)(bg_color == LCD_FONT_BG_TRANSPARENT ? 0U : bg_color));

    for (i = 0U; i < (uint32_t)w * h; ++i)
    {
        g_font_dma_buffer[i] = bg;
    }

    LCD_RenderFontStringFixedToBuffer(
        g_font_dma_buffer,
        w,
        h,
        0U,
        0U,
        str,
        size,
        cell_width,
        color);

    LCD_BlitRectRGB565(
        x,
        y,
        w,
        h,
        g_font_dma_buffer);
}

void LCD_DrawFontChar(
        uint16_t x,
        uint16_t y,
        char ch,
        uint16_t size,
        uint32_t color,
        uint32_t bg_color)
{
    const LCD_FontGlyph *glyph;

    uint16_t row;
    uint16_t col;

    if (ch == ' ')
    {
        return;
    }

    glyph =
        Teko_SemiBold_FindGlyph(
            ch,
            size);

    if (!glyph)
    {
        return;
    }

    for (row = glyph->min_row;
         row < glyph->min_row + glyph->height;
         row++)
    {
        uint8_t run_active = 0;
        uint16_t run_start = 0;

        for (col = glyph->min_col;
             col < glyph->min_col + glyph->width;
             col++)
        {
            uint8_t pixel =
                LCD_GlyphPixelIsSet(
                    glyph->bitmap,
                    size,
                    row,
                    col);

            if (pixel)
            {
                if (!run_active)
                {
                    run_active = 1;
                    run_start = col;
                }
            }
            else
            {
                if (run_active)
                {
                    LCD_Rect_Fill(
                        x + run_start -
                            glyph->min_col,

                        y + row -
                            glyph->min_row,

                        x + col - 1 -
                            glyph->min_col,

                        y + row -
                            glyph->min_row,

                        color);

                    run_active = 0;
                }

                if (bg_color !=
                    LCD_FONT_BG_TRANSPARENT)
                {
                    LCD_DrawPixelColor(
                        x + col -
                        glyph->min_col,

                        y + row -
                        glyph->min_row,

                        bg_color);
                }
            }
        }

        if (run_active)
        {
            LCD_Rect_Fill(
                x + run_start -
                    glyph->min_col,

                y + row -
                    glyph->min_row,

                x +
                glyph->width -
                1,

                y + row -
                    glyph->min_row,

                color);
        }
    }
}

void LCD_DrawFontString(
        uint16_t x,
        uint16_t y,
        const char *str,
        uint16_t size,
        uint32_t color,
        uint32_t bg_color)
{
    uint16_t cursor = x;

    if (!str)
    {
        return;
    }

    while (*str)
    {
        if (*str == '\n')
        {
            cursor = x;

            y += size;

            str++;

            continue;
        }

        LCD_DrawFontChar(
            cursor,
            y,
            *str,
            size,
            color,
            bg_color);

        cursor +=
            LCD_GetFontAdvance(
                *str,
                size);

        str++;
    }
}
