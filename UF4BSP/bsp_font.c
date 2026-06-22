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
#include "mdma.h"

#define LCD_FONT_CACHE_MAX_CELL_H     104U
static uint16_t g_font_dma_buffer[540U * 176U] __attribute__((section(".sdram"), aligned(32)));
static MDMA_LinkNodeTypeDef g_font_mdma_nodes[LCD_FONT_CACHE_MAX_CELL_H - 1U] __attribute__((section(".sdram"), aligned(32)));

#define LCD_FONT_CACHE_SIZE_VALUE   144U
#define LCD_FONT_CACHE_CELL_W       60U
#define LCD_FONT_CACHE_CELL_H       104U
#define LCD_FONT_CACHE_PHYS_W       LCD_FONT_CACHE_CELL_H
#define LCD_FONT_CACHE_PHYS_H       LCD_FONT_CACHE_CELL_W
#define LCD_FONT_CACHE_GLYPHS       13U

#define LCD_FONT_CACHE_SIZE_SET     34U
#define LCD_FONT_CACHE_SET_CELL_W   16U
#define LCD_FONT_CACHE_SET_CELL_H   34U
#define LCD_FONT_CACHE_SET_PHYS_W   LCD_FONT_CACHE_SET_CELL_H
#define LCD_FONT_CACHE_SET_PHYS_H   LCD_FONT_CACHE_SET_CELL_W

static uint16_t g_font_value_cache[LCD_FONT_CACHE_GLYPHS][LCD_FONT_CACHE_PHYS_W * LCD_FONT_CACHE_PHYS_H]
        __attribute__((section(".sdram"), aligned(32)));
static uint16_t g_font_set_cache[LCD_FONT_CACHE_GLYPHS][LCD_FONT_CACHE_SET_PHYS_W * LCD_FONT_CACHE_SET_PHYS_H]
        __attribute__((section(".sdram"), aligned(32)));
static uint8_t g_font_value_cache_ready;
static uint8_t g_font_set_cache_ready;

static int32_t LCD_FontCacheIndex(char ch)
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

static void LCD_FontCacheMaintainDCache(
        uint32_t addr,
        uint32_t bytes,
        uint8_t clean,
        uint8_t invalidate)
{
    uint32_t aligned_addr = addr & ~31UL;
    uint32_t end_addr = (addr + bytes + 31UL) & ~31UL;
    int32_t aligned_bytes = (int32_t)(end_addr - aligned_addr);

    if (bytes == 0U)
    {
        return;
    }

    if (clean != 0U && invalidate != 0U)
    {
        SCB_CleanInvalidateDCache_by_Addr((uint32_t *)aligned_addr, aligned_bytes);
    }
    else if (clean != 0U)
    {
        SCB_CleanDCache_by_Addr((uint32_t *)aligned_addr, aligned_bytes);
    }
    else if (invalidate != 0U)
    {
        SCB_InvalidateDCache_by_Addr((uint32_t *)aligned_addr, aligned_bytes);
    }
}

static void LCD_FontCacheRotateCpu(
        const uint16_t *src,
        uint16_t *dst,
        uint16_t cell_w,
        uint16_t cell_h)
{
    uint32_t lx;
    uint32_t ly;

    for (ly = 0U; ly < cell_h; ++ly)
    {
        for (lx = 0U; lx < cell_w; ++lx)
        {
            dst[(uint32_t)(cell_w - 1U - lx) * cell_h + ly] =
                src[ly * cell_w + lx];
        }
    }
}

static uint8_t LCD_FontCacheRotateMdma(
        const uint16_t *src,
        uint16_t *dst,
        uint16_t cell_w,
        uint16_t cell_h)
{
    uint32_t row;
    uint32_t node_ctcr;
    uint32_t node_cbndtr;
    uint32_t node_cbrur;
    uint32_t pixel_bytes = sizeof(uint16_t);
    uint32_t src_bytes = (uint32_t)cell_w * cell_h * pixel_bytes;
    uint32_t dst_bytes = src_bytes;

    if (cell_w == 0U || cell_h == 0U ||
        cell_h > LCD_FONT_CACHE_MAX_CELL_H)
    {
        return 0U;
    }

    __HAL_RCC_MDMA_CLK_ENABLE();

    hmdma_mdma_channel0_sw_0.Instance = MDMA_Channel0;
    (void)HAL_MDMA_Abort(&hmdma_mdma_channel0_sw_0);

    hmdma_mdma_channel0_sw_0.Init.Request = MDMA_REQUEST_SW;
    hmdma_mdma_channel0_sw_0.Init.TransferTriggerMode = MDMA_FULL_TRANSFER;
    hmdma_mdma_channel0_sw_0.Init.Priority = MDMA_PRIORITY_HIGH;
    hmdma_mdma_channel0_sw_0.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
    hmdma_mdma_channel0_sw_0.Init.SourceInc = MDMA_SRC_INC_HALFWORD;
    hmdma_mdma_channel0_sw_0.Init.DestinationInc = MDMA_DEST_INC_DISABLE;
    hmdma_mdma_channel0_sw_0.Init.SourceDataSize = MDMA_SRC_DATASIZE_HALFWORD;
    hmdma_mdma_channel0_sw_0.Init.DestDataSize = MDMA_DEST_DATASIZE_HALFWORD;
    hmdma_mdma_channel0_sw_0.Init.DataAlignment = MDMA_DATAALIGN_RIGHT;
    hmdma_mdma_channel0_sw_0.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
    hmdma_mdma_channel0_sw_0.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
    hmdma_mdma_channel0_sw_0.Init.BufferTransferLength = pixel_bytes;
    hmdma_mdma_channel0_sw_0.Init.SourceBlockAddressOffset = 0;
    hmdma_mdma_channel0_sw_0.Init.DestBlockAddressOffset =
        -((int32_t)cell_h * (int32_t)pixel_bytes);

    if (HAL_MDMA_Init(&hmdma_mdma_channel0_sw_0) != HAL_OK)
    {
        return 0U;
    }

    hmdma_mdma_channel0_sw_0.FirstLinkedListNodeAddress = NULL;
    hmdma_mdma_channel0_sw_0.LastLinkedListNodeAddress = NULL;
    hmdma_mdma_channel0_sw_0.LinkedListNodeCounter = 0U;
    hmdma_mdma_channel0_sw_0.Instance->CLAR = 0U;

    node_ctcr =
        hmdma_mdma_channel0_sw_0.Init.SourceInc |
        hmdma_mdma_channel0_sw_0.Init.DestinationInc |
        hmdma_mdma_channel0_sw_0.Init.SourceDataSize |
        hmdma_mdma_channel0_sw_0.Init.DestDataSize |
        hmdma_mdma_channel0_sw_0.Init.DataAlignment |
        hmdma_mdma_channel0_sw_0.Init.SourceBurst |
        hmdma_mdma_channel0_sw_0.Init.DestBurst |
        ((hmdma_mdma_channel0_sw_0.Init.BufferTransferLength - 1U) << MDMA_CTCR_TLEN_Pos) |
        hmdma_mdma_channel0_sw_0.Init.TransferTriggerMode |
        MDMA_CTCR_SWRM |
        MDMA_CTCR_BWM;
    node_cbndtr =
        (((uint32_t)cell_w - 1U) << MDMA_CBNDTR_BRC_Pos) |
        MDMA_CBNDTR_BRDUM |
        pixel_bytes;
    node_cbrur =
        ((uint32_t)cell_h * pixel_bytes) << MDMA_CBRUR_DUV_Pos;

    for (row = 1U; row < cell_h; ++row)
    {
        MDMA_LinkNodeTypeDef *node = &g_font_mdma_nodes[row - 1U];

        node->CTCR = node_ctcr;
        node->CBNDTR = node_cbndtr;
        node->CSAR = (uint32_t)&src[row * cell_w];
        node->CDAR = (uint32_t)&dst[(uint32_t)(cell_w - 1U) * cell_h + row];
        node->CBRUR = node_cbrur;
        node->CLAR = (row + 1U < cell_h) ? (uint32_t)&g_font_mdma_nodes[row] : 0U;
        node->CTBR = 0U;
        node->Reserved = 0U;
        node->CMAR = 0U;
        node->CMDR = 0U;
    }

    if (cell_h > 1U)
    {
        hmdma_mdma_channel0_sw_0.Instance->CLAR = (uint32_t)&g_font_mdma_nodes[0];
        hmdma_mdma_channel0_sw_0.FirstLinkedListNodeAddress = &g_font_mdma_nodes[0];
        hmdma_mdma_channel0_sw_0.LastLinkedListNodeAddress = &g_font_mdma_nodes[cell_h - 2U];
        hmdma_mdma_channel0_sw_0.LinkedListNodeCounter = cell_h - 1U;

        LCD_FontCacheMaintainDCache(
            (uint32_t)g_font_mdma_nodes,
            (uint32_t)(cell_h - 1U) * sizeof(g_font_mdma_nodes[0]),
            1U,
            0U);
    }

    LCD_FontCacheMaintainDCache((uint32_t)src, src_bytes, 1U, 0U);
    LCD_FontCacheMaintainDCache((uint32_t)dst, dst_bytes, 1U, 1U);

    if (HAL_MDMA_Start(
            &hmdma_mdma_channel0_sw_0,
            (uint32_t)src,
            (uint32_t)&dst[(uint32_t)(cell_w - 1U) * cell_h],
            pixel_bytes,
            cell_w) != HAL_OK)
    {
        (void)HAL_MDMA_Abort(&hmdma_mdma_channel0_sw_0);
        return 0U;
    }

    if (HAL_MDMA_PollForTransfer(
            &hmdma_mdma_channel0_sw_0,
            HAL_MDMA_FULL_TRANSFER,
            10U) != HAL_OK)
    {
        (void)HAL_MDMA_Abort(&hmdma_mdma_channel0_sw_0);
        return 0U;
    }

    LCD_FontCacheMaintainDCache((uint32_t)dst, dst_bytes, 0U, 1U);
    return 1U;
}

static void LCD_FontCacheBuildGlyph(
        char ch,
        uint16_t *dst,
        uint16_t size,
        uint16_t cell_w,
        uint16_t cell_h,
        uint32_t color,
        uint32_t bg_color)
{
    uint32_t i;
    uint16_t bg = LCD_EncodeColor((uint16_t)bg_color);

    for (i = 0U; i < (uint32_t)cell_w * cell_h; ++i)
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
            cell_w,
            cell_h,
            0U,
            0U,
            text,
            size,
            cell_w,
            color);
    }

    if (LCD_FontCacheRotateMdma(
            g_font_dma_buffer,
            dst,
            cell_w,
            cell_h) == 0U)
    {
        LCD_FontCacheRotateCpu(
            g_font_dma_buffer,
            dst,
            cell_w,
            cell_h);
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
        LCD_FontCacheBuildGlyph(
            glyphs[i],
            g_font_value_cache[i],
            LCD_FONT_CACHE_SIZE_VALUE,
            LCD_FONT_CACHE_CELL_W,
            LCD_FONT_CACHE_CELL_H,
            0xFFFF,
            0x0841);
    }

    SCB_CleanDCache_by_Addr(
        (uint32_t *)g_font_value_cache,
        (int32_t)sizeof(g_font_value_cache));
    g_font_value_cache_ready = 1U;
}

static void LCD_FontSetCacheEnsure(void)
{
    static const char glyphs[LCD_FONT_CACHE_GLYPHS] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '-', ' '
    };
    uint32_t i;

    if (g_font_set_cache_ready != 0U)
    {
        return;
    }

    for (i = 0U; i < LCD_FONT_CACHE_GLYPHS; ++i)
    {
        LCD_FontCacheBuildGlyph(
            glyphs[i],
            g_font_set_cache[i],
            LCD_FONT_CACHE_SIZE_SET,
            LCD_FONT_CACHE_SET_CELL_W,
            LCD_FONT_CACHE_SET_CELL_H,
            0xFFFF,
            0x1082);
    }

    SCB_CleanDCache_by_Addr(
        (uint32_t *)g_font_set_cache,
        (int32_t)sizeof(g_font_set_cache));
    g_font_set_cache_ready = 1U;
}

static uint8_t LCD_DrawFontCachedFixed(
        uint16_t x,
        uint16_t y,
        uint16_t w,
        const char *str,
        uint16_t cell_w,
        uint16_t cell_h,
        uint16_t phys_w,
        const uint16_t cache[][phys_w * cell_w])
{
    uint16_t cell_x = x;
    const char *p = str;

    while (*p != '\0' && (uint32_t)(cell_x - x) < w)
    {
        int32_t cache_index = LCD_FontCacheIndex(*p);

        if (cache_index < 0)
        {
            return 0U;
        }

        LCD_BlitRotatedRectRGB565Clean(
            cell_x,
            y,
            cell_w,
            cell_h,
            cache[cache_index]);
        cell_x = (uint16_t)(cell_x + cell_w);
        p++;
    }

    return (uint8_t)(*p == '\0');
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

uint8_t LCD_GetFontStringBBox(
        const char *str,
        uint16_t size,
        uint16_t *min_row,
        uint16_t *height)
{
    uint16_t top = size;
    uint16_t bottom = 0U;
    uint8_t found = 0U;

    if (!str || !min_row || !height)
    {
        return 0U;
    }

    while (*str && *str != '\n')
    {
        const LCD_FontGlyph *glyph;

        if (*str == ' ')
        {
            str++;
            continue;
        }

        glyph =
            Teko_SemiBold_FindGlyph(
                *str,
                size);

        if (glyph)
        {
            uint16_t glyph_bottom =
                (uint16_t)(glyph->min_row + glyph->height);

            if (glyph->min_row < top)
            {
                top = glyph->min_row;
            }
            if (glyph_bottom > bottom)
            {
                bottom = glyph_bottom;
            }
            found = 1U;
        }

        str++;
    }

    if (found == 0U || bottom <= top)
    {
        *min_row = 0U;
        *height = size;
        return 0U;
    }

    *min_row = top;
    *height = (uint16_t)(bottom - top);
    return 1U;
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

void LCD_DrawFontStringDMATight(
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
    uint16_t min_row = 0U;
    uint16_t tight_h = h;

    if (!str || w == 0U || h == 0U)
    {
        return;
    }

    if (LCD_GetFontStringBBox(str, size, &min_row, &tight_h) == 0U)
    {
        LCD_DrawFontStringDMA(x, y, w, h, str, size, color, bg_color);
        return;
    }
    if (min_row >= h)
    {
        return;
    }
    if ((uint32_t)min_row + tight_h > h)
    {
        tight_h = (uint16_t)(h - min_row);
    }
    if ((uint32_t)w * tight_h > (uint32_t)(sizeof(g_font_dma_buffer) / sizeof(g_font_dma_buffer[0])))
    {
        return;
    }

    bg =
        LCD_EncodeColor(
            (uint16_t)(bg_color == LCD_FONT_BG_TRANSPARENT ? 0U : bg_color));

    for (i = 0U; i < (uint32_t)w * tight_h; ++i)
    {
        g_font_dma_buffer[i] = bg;
    }

    {
        uint16_t cursor = 0U;
        uint16_t fg = LCD_EncodeColor((uint16_t)color);
        const char *p = str;

        while (*p)
        {
            const LCD_FontGlyph *glyph;
            uint16_t row;
            uint16_t col;

            if (*p == '\n')
            {
                break;
            }

            glyph =
                Teko_SemiBold_FindGlyph(
                    *p,
                    size);

            if (glyph)
            {
                for (row = glyph->min_row;
                     row < glyph->min_row + glyph->height;
                     row++)
                {
                    uint16_t dst_y =
                        (uint16_t)(row - min_row);

                    if (row < min_row || dst_y >= tight_h)
                    {
                        continue;
                    }

                    for (col = glyph->min_col;
                         col < glyph->min_col + glyph->width;
                         col++)
                    {
                        uint16_t dst_x =
                            (uint16_t)(cursor + col - glyph->min_col);

                        if (dst_x >= w)
                        {
                            continue;
                        }

                        if (LCD_GlyphPixelIsSet(
                                glyph->bitmap,
                                size,
                                row,
                                col))
                        {
                            g_font_dma_buffer[(uint32_t)dst_y * w + dst_x] = fg;
                        }
                    }
                }
            }

            cursor +=
                LCD_GetFontAdvance(
                    (uint8_t)*p,
                    size);

            p++;
        }
    }

    LCD_BlitRectRGB565(
        x,
        (uint16_t)(y + min_row),
        w,
        tight_h,
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
        LCD_FontValueCacheEnsure();
        if (LCD_DrawFontCachedFixed(
                x,
                y,
                w,
                str,
                LCD_FONT_CACHE_CELL_W,
                LCD_FONT_CACHE_CELL_H,
                LCD_FONT_CACHE_PHYS_W,
                g_font_value_cache) != 0U)
        {
            return;
        }
    }

    if (size == LCD_FONT_CACHE_SIZE_SET &&
        cell_width == LCD_FONT_CACHE_SET_CELL_W &&
        h == LCD_FONT_CACHE_SET_CELL_H &&
        bg_color == 0x1082 &&
        color == 0xFFFF)
    {
        LCD_FontSetCacheEnsure();
        if (LCD_DrawFontCachedFixed(
                x,
                y,
                w,
                str,
                LCD_FONT_CACHE_SET_CELL_W,
                LCD_FONT_CACHE_SET_CELL_H,
                LCD_FONT_CACHE_SET_PHYS_W,
                g_font_set_cache) != 0U)
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
