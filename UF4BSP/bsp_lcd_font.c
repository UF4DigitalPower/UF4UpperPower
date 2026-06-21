/**
 * @file    : bsp_lcd_font.c
 * @brief   : Bitmap font drawing
 */

#include "bsp_lcd.h"
#include "bsp_lcd_font.h"
#include "Teko_SemiBold_Index.h"


typedef struct {
	const uint8_t *bitmap;
	uint16_t min_col;
	uint16_t width;
} LCD_GlyphInfo;

static uint8_t LCD_FontBytesPerRow(LCD_FontSize size) {
	return (uint8_t) ((size + 7U) / 8U);
}

static uint8_t LCD_GlyphPixelIsSet(const uint8_t *bitmap, LCD_FontSize size, uint8_t bytes_per_row,
		uint16_t row, uint16_t col) {
	uint8_t bits;

	if ((bitmap == NULL) || (row >= size) || (col >= size)) {
		return 0U;
	}

	bits = bitmap[(uint32_t) row * bytes_per_row + (col / 8U)];
	return (uint8_t) ((bits & (uint8_t) (0x80U >> (col % 8U))) != 0U);
}

static LCD_GlyphInfo LCD_GetGlyphInfo(uint8_t ch, LCD_FontSize size) {
	LCD_GlyphInfo glyph = {0};
	uint8_t bytes_per_row;
	uint16_t row;
	int16_t min_col = (int16_t) size;
	int16_t max_col = -1;

	glyph.bitmap = Teko_SemiBold_FindBitmap((char) ch, size);
	if (glyph.bitmap == NULL) {
		return glyph;
	}

	bytes_per_row = LCD_FontBytesPerRow(size);
	for (row = 0U; row < size; ++row) {
		uint16_t col;
		for (col = 0U; col < size; ++col) {
			if (LCD_GlyphPixelIsSet(glyph.bitmap, size, bytes_per_row, row, col) != 0U) {
				if ((int16_t) col < min_col) {
					min_col = (int16_t) col;
				}
				if ((int16_t) col > max_col) {
					max_col = (int16_t) col;
				}
			}
		}
	}

	if (max_col >= min_col) {
		glyph.min_col = (uint16_t) min_col;
		glyph.width = (uint16_t) (max_col - min_col + 1);
	}

	return glyph;
}

static uint16_t LCD_GetFontAdvance(uint8_t ch, LCD_FontSize size) {
	LCD_GlyphInfo glyph;

	if (ch == ' ') {
		return size / 2U;
	}

	glyph = LCD_GetGlyphInfo(ch, size);
	if (glyph.width == 0U) {
		return size / 2U;
	}

	return (uint16_t) (glyph.width + (glyph.width / 6U));
}

static void LCD_FillSpace(uint16_t x, uint16_t y, LCD_FontSize size, uint32_t bg_color) {
	uint16_t ex;
	uint16_t ey;

	if ((bg_color == LCD_FONT_BG_TRANSPARENT) || (x >= LCD_DEV.width) || (y >= LCD_DEV.height)) {
		return;
	}

	ex = (uint16_t) (x + size - 1U);
	ey = (uint16_t) (y + size - 1U);
	if (ex >= LCD_DEV.width) {
		ex = (uint16_t) (LCD_DEV.width - 1U);
	}
	if (ey >= LCD_DEV.height) {
		ey = (uint16_t) (LCD_DEV.height - 1U);
	}
	LCD_Rect_Fill(x, y, ex, ey, bg_color);
}

void LCD_DrawFontChar(uint16_t x, uint16_t y, char ch, LCD_FontSize size, uint32_t color, uint32_t bg_color) {
	LCD_GlyphInfo glyph;
	uint8_t bytes_per_row;
	uint16_t row;

	if (size == 0U) {
		return;
	}

	if (ch == ' ') {
		LCD_FillSpace(x, y, size, bg_color);
		return;
	}

	glyph = LCD_GetGlyphInfo((uint8_t) ch, size);
	if (glyph.bitmap == NULL) {
		return;
	}

	bytes_per_row = LCD_FontBytesPerRow(size);
	for (row = 0U; row < size; ++row) {
		uint16_t col;
		for (col = glyph.min_col; col < size; ++col) {
			uint16_t px = (uint16_t) (x + col - glyph.min_col);
			uint16_t py = (uint16_t) (y + row);
			if (LCD_GlyphPixelIsSet(glyph.bitmap, size, bytes_per_row, row, col) != 0U) {
				LCD_DrawPixelColor(px, py, color);
			} else if (bg_color != LCD_FONT_BG_TRANSPARENT) {
				LCD_DrawPixelColor(px, py, bg_color);
			}
		}
	}
}

void LCD_DrawFontString(uint16_t x, uint16_t y, const char *str, LCD_FontSize size, uint32_t color, uint32_t bg_color) {
	uint16_t cursor = x;

	if ((str == NULL) || (size == 0U)) {
		return;
	}

	while (*str != '\0') {
		if (*str == '\n') {
			cursor = x;
			y = (uint16_t) (y + size + 2U);
			++str;
			continue;
		}

		LCD_DrawFontChar(cursor, y, *str, size, color, bg_color);
		cursor = (uint16_t) (cursor + LCD_GetFontAdvance((uint8_t) *str, size));
		++str;
	}
}
