/**
  ******************************************************************************
  * @file    bap_font.c
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
#include "bap_font.h"
#include "bsp_lcd.h"

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
    static uint16_t font_dma_buffer[540U * 176U] __attribute__((section(".sdram"), aligned(32)));

    if (!str || w == 0U || h == 0U)
    {
        return;
    }
    if ((uint32_t)w * h > (uint32_t)(sizeof(font_dma_buffer) / sizeof(font_dma_buffer[0])))
    {
        return;
    }

    bg =
        LCD_EncodeColor(
            (uint16_t)(bg_color == LCD_FONT_BG_TRANSPARENT ? 0U : bg_color));

    for (i = 0U; i < (uint32_t)w * h; ++i)
    {
        font_dma_buffer[i] = bg;
    }

    LCD_RenderFontStringToBuffer(
        font_dma_buffer,
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
        font_dma_buffer);
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
