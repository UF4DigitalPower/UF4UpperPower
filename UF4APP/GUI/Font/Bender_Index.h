#ifndef BENDER_INDEX_H
#define BENDER_INDEX_H

#include "Bender_Config.h"
#include "Bender_10x10.h"
#include "Bender_14x14.h"
#include "Bender_16x16.h"
#include "Bender_20x20.h"
#include "Bender_24x24.h"
#include "Bender_28x28.h"
#include "Bender_34x34.h"
#include "Bender_48x48.h"
#include "Bender_72x72.h"
#include "Bender_96x96.h"
#include "Bender_128x128.h"

typedef struct {
	LCD_FontSize size;
	const uint8_t *bitmap;
} Bender_SizeBitmap;

typedef struct {
	char ch;
	const Bender_SizeBitmap *sizes;
	uint16_t size_count;
} Bender_Glyph;

static const Bender_SizeBitmap bender_glyph_0031[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0031},
	{LCD_FONT_BENDER_14, bender_14_char_0031},
	{LCD_FONT_BENDER_16, bender_16_char_0031},
	{LCD_FONT_BENDER_20, bender_20_char_0031},
	{LCD_FONT_BENDER_24, bender_24_char_0031},
	{LCD_FONT_BENDER_28, bender_28_char_0031},
	{LCD_FONT_BENDER_34, bender_34_char_0031},
	{LCD_FONT_BENDER_48, bender_48_char_0031},
	{LCD_FONT_BENDER_72, bender_72_char_0031},
	{LCD_FONT_BENDER_96, bender_96_char_0031},
	{LCD_FONT_BENDER_128, bender_128_char_0031},
};

static const Bender_SizeBitmap bender_glyph_0032[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0032},
	{LCD_FONT_BENDER_14, bender_14_char_0032},
	{LCD_FONT_BENDER_16, bender_16_char_0032},
	{LCD_FONT_BENDER_20, bender_20_char_0032},
	{LCD_FONT_BENDER_24, bender_24_char_0032},
	{LCD_FONT_BENDER_28, bender_28_char_0032},
	{LCD_FONT_BENDER_34, bender_34_char_0032},
	{LCD_FONT_BENDER_48, bender_48_char_0032},
	{LCD_FONT_BENDER_72, bender_72_char_0032},
	{LCD_FONT_BENDER_96, bender_96_char_0032},
	{LCD_FONT_BENDER_128, bender_128_char_0032},
};

static const Bender_SizeBitmap bender_glyph_0033[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0033},
	{LCD_FONT_BENDER_14, bender_14_char_0033},
	{LCD_FONT_BENDER_16, bender_16_char_0033},
	{LCD_FONT_BENDER_20, bender_20_char_0033},
	{LCD_FONT_BENDER_24, bender_24_char_0033},
	{LCD_FONT_BENDER_28, bender_28_char_0033},
	{LCD_FONT_BENDER_34, bender_34_char_0033},
	{LCD_FONT_BENDER_48, bender_48_char_0033},
	{LCD_FONT_BENDER_72, bender_72_char_0033},
	{LCD_FONT_BENDER_96, bender_96_char_0033},
	{LCD_FONT_BENDER_128, bender_128_char_0033},
};

static const Bender_SizeBitmap bender_glyph_0034[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0034},
	{LCD_FONT_BENDER_14, bender_14_char_0034},
	{LCD_FONT_BENDER_16, bender_16_char_0034},
	{LCD_FONT_BENDER_20, bender_20_char_0034},
	{LCD_FONT_BENDER_24, bender_24_char_0034},
	{LCD_FONT_BENDER_28, bender_28_char_0034},
	{LCD_FONT_BENDER_34, bender_34_char_0034},
	{LCD_FONT_BENDER_48, bender_48_char_0034},
	{LCD_FONT_BENDER_72, bender_72_char_0034},
	{LCD_FONT_BENDER_96, bender_96_char_0034},
	{LCD_FONT_BENDER_128, bender_128_char_0034},
};

static const Bender_SizeBitmap bender_glyph_0035[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0035},
	{LCD_FONT_BENDER_14, bender_14_char_0035},
	{LCD_FONT_BENDER_16, bender_16_char_0035},
	{LCD_FONT_BENDER_20, bender_20_char_0035},
	{LCD_FONT_BENDER_24, bender_24_char_0035},
	{LCD_FONT_BENDER_28, bender_28_char_0035},
	{LCD_FONT_BENDER_34, bender_34_char_0035},
	{LCD_FONT_BENDER_48, bender_48_char_0035},
	{LCD_FONT_BENDER_72, bender_72_char_0035},
	{LCD_FONT_BENDER_96, bender_96_char_0035},
	{LCD_FONT_BENDER_128, bender_128_char_0035},
};

static const Bender_SizeBitmap bender_glyph_0036[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0036},
	{LCD_FONT_BENDER_14, bender_14_char_0036},
	{LCD_FONT_BENDER_16, bender_16_char_0036},
	{LCD_FONT_BENDER_20, bender_20_char_0036},
	{LCD_FONT_BENDER_24, bender_24_char_0036},
	{LCD_FONT_BENDER_28, bender_28_char_0036},
	{LCD_FONT_BENDER_34, bender_34_char_0036},
	{LCD_FONT_BENDER_48, bender_48_char_0036},
	{LCD_FONT_BENDER_72, bender_72_char_0036},
	{LCD_FONT_BENDER_96, bender_96_char_0036},
	{LCD_FONT_BENDER_128, bender_128_char_0036},
};

static const Bender_SizeBitmap bender_glyph_0037[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0037},
	{LCD_FONT_BENDER_14, bender_14_char_0037},
	{LCD_FONT_BENDER_16, bender_16_char_0037},
	{LCD_FONT_BENDER_20, bender_20_char_0037},
	{LCD_FONT_BENDER_24, bender_24_char_0037},
	{LCD_FONT_BENDER_28, bender_28_char_0037},
	{LCD_FONT_BENDER_34, bender_34_char_0037},
	{LCD_FONT_BENDER_48, bender_48_char_0037},
	{LCD_FONT_BENDER_72, bender_72_char_0037},
	{LCD_FONT_BENDER_96, bender_96_char_0037},
	{LCD_FONT_BENDER_128, bender_128_char_0037},
};

static const Bender_SizeBitmap bender_glyph_0038[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0038},
	{LCD_FONT_BENDER_14, bender_14_char_0038},
	{LCD_FONT_BENDER_16, bender_16_char_0038},
	{LCD_FONT_BENDER_20, bender_20_char_0038},
	{LCD_FONT_BENDER_24, bender_24_char_0038},
	{LCD_FONT_BENDER_28, bender_28_char_0038},
	{LCD_FONT_BENDER_34, bender_34_char_0038},
	{LCD_FONT_BENDER_48, bender_48_char_0038},
	{LCD_FONT_BENDER_72, bender_72_char_0038},
	{LCD_FONT_BENDER_96, bender_96_char_0038},
	{LCD_FONT_BENDER_128, bender_128_char_0038},
};

static const Bender_SizeBitmap bender_glyph_0039[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0039},
	{LCD_FONT_BENDER_14, bender_14_char_0039},
	{LCD_FONT_BENDER_16, bender_16_char_0039},
	{LCD_FONT_BENDER_20, bender_20_char_0039},
	{LCD_FONT_BENDER_24, bender_24_char_0039},
	{LCD_FONT_BENDER_28, bender_28_char_0039},
	{LCD_FONT_BENDER_34, bender_34_char_0039},
	{LCD_FONT_BENDER_48, bender_48_char_0039},
	{LCD_FONT_BENDER_72, bender_72_char_0039},
	{LCD_FONT_BENDER_96, bender_96_char_0039},
	{LCD_FONT_BENDER_128, bender_128_char_0039},
};

static const Bender_SizeBitmap bender_glyph_0030[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0030},
	{LCD_FONT_BENDER_14, bender_14_char_0030},
	{LCD_FONT_BENDER_16, bender_16_char_0030},
	{LCD_FONT_BENDER_20, bender_20_char_0030},
	{LCD_FONT_BENDER_24, bender_24_char_0030},
	{LCD_FONT_BENDER_28, bender_28_char_0030},
	{LCD_FONT_BENDER_34, bender_34_char_0030},
	{LCD_FONT_BENDER_48, bender_48_char_0030},
	{LCD_FONT_BENDER_72, bender_72_char_0030},
	{LCD_FONT_BENDER_96, bender_96_char_0030},
	{LCD_FONT_BENDER_128, bender_128_char_0030},
};

static const Bender_SizeBitmap bender_glyph_0071[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0071},
	{LCD_FONT_BENDER_14, bender_14_char_0071},
	{LCD_FONT_BENDER_16, bender_16_char_0071},
	{LCD_FONT_BENDER_20, bender_20_char_0071},
	{LCD_FONT_BENDER_24, bender_24_char_0071},
	{LCD_FONT_BENDER_28, bender_28_char_0071},
	{LCD_FONT_BENDER_34, bender_34_char_0071},
	{LCD_FONT_BENDER_48, bender_48_char_0071},
	{LCD_FONT_BENDER_72, bender_72_char_0071},
	{LCD_FONT_BENDER_96, bender_96_char_0071},
	{LCD_FONT_BENDER_128, bender_128_char_0071},
};

static const Bender_SizeBitmap bender_glyph_0077[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0077},
	{LCD_FONT_BENDER_14, bender_14_char_0077},
	{LCD_FONT_BENDER_16, bender_16_char_0077},
	{LCD_FONT_BENDER_20, bender_20_char_0077},
	{LCD_FONT_BENDER_24, bender_24_char_0077},
	{LCD_FONT_BENDER_28, bender_28_char_0077},
	{LCD_FONT_BENDER_34, bender_34_char_0077},
	{LCD_FONT_BENDER_48, bender_48_char_0077},
	{LCD_FONT_BENDER_72, bender_72_char_0077},
	{LCD_FONT_BENDER_96, bender_96_char_0077},
	{LCD_FONT_BENDER_128, bender_128_char_0077},
};

static const Bender_SizeBitmap bender_glyph_0065[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0065},
	{LCD_FONT_BENDER_14, bender_14_char_0065},
	{LCD_FONT_BENDER_16, bender_16_char_0065},
	{LCD_FONT_BENDER_20, bender_20_char_0065},
	{LCD_FONT_BENDER_24, bender_24_char_0065},
	{LCD_FONT_BENDER_28, bender_28_char_0065},
	{LCD_FONT_BENDER_34, bender_34_char_0065},
	{LCD_FONT_BENDER_48, bender_48_char_0065},
	{LCD_FONT_BENDER_72, bender_72_char_0065},
	{LCD_FONT_BENDER_96, bender_96_char_0065},
	{LCD_FONT_BENDER_128, bender_128_char_0065},
};

static const Bender_SizeBitmap bender_glyph_0072[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0072},
	{LCD_FONT_BENDER_14, bender_14_char_0072},
	{LCD_FONT_BENDER_16, bender_16_char_0072},
	{LCD_FONT_BENDER_20, bender_20_char_0072},
	{LCD_FONT_BENDER_24, bender_24_char_0072},
	{LCD_FONT_BENDER_28, bender_28_char_0072},
	{LCD_FONT_BENDER_34, bender_34_char_0072},
	{LCD_FONT_BENDER_48, bender_48_char_0072},
	{LCD_FONT_BENDER_72, bender_72_char_0072},
	{LCD_FONT_BENDER_96, bender_96_char_0072},
	{LCD_FONT_BENDER_128, bender_128_char_0072},
};

static const Bender_SizeBitmap bender_glyph_0074[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0074},
	{LCD_FONT_BENDER_14, bender_14_char_0074},
	{LCD_FONT_BENDER_16, bender_16_char_0074},
	{LCD_FONT_BENDER_20, bender_20_char_0074},
	{LCD_FONT_BENDER_24, bender_24_char_0074},
	{LCD_FONT_BENDER_28, bender_28_char_0074},
	{LCD_FONT_BENDER_34, bender_34_char_0074},
	{LCD_FONT_BENDER_48, bender_48_char_0074},
	{LCD_FONT_BENDER_72, bender_72_char_0074},
	{LCD_FONT_BENDER_96, bender_96_char_0074},
	{LCD_FONT_BENDER_128, bender_128_char_0074},
};

static const Bender_SizeBitmap bender_glyph_0079[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0079},
	{LCD_FONT_BENDER_14, bender_14_char_0079},
	{LCD_FONT_BENDER_16, bender_16_char_0079},
	{LCD_FONT_BENDER_20, bender_20_char_0079},
	{LCD_FONT_BENDER_24, bender_24_char_0079},
	{LCD_FONT_BENDER_28, bender_28_char_0079},
	{LCD_FONT_BENDER_34, bender_34_char_0079},
	{LCD_FONT_BENDER_48, bender_48_char_0079},
	{LCD_FONT_BENDER_72, bender_72_char_0079},
	{LCD_FONT_BENDER_96, bender_96_char_0079},
	{LCD_FONT_BENDER_128, bender_128_char_0079},
};

static const Bender_SizeBitmap bender_glyph_0075[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0075},
	{LCD_FONT_BENDER_14, bender_14_char_0075},
	{LCD_FONT_BENDER_16, bender_16_char_0075},
	{LCD_FONT_BENDER_20, bender_20_char_0075},
	{LCD_FONT_BENDER_24, bender_24_char_0075},
	{LCD_FONT_BENDER_28, bender_28_char_0075},
	{LCD_FONT_BENDER_34, bender_34_char_0075},
	{LCD_FONT_BENDER_48, bender_48_char_0075},
	{LCD_FONT_BENDER_72, bender_72_char_0075},
	{LCD_FONT_BENDER_96, bender_96_char_0075},
	{LCD_FONT_BENDER_128, bender_128_char_0075},
};

static const Bender_SizeBitmap bender_glyph_0069[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0069},
	{LCD_FONT_BENDER_14, bender_14_char_0069},
	{LCD_FONT_BENDER_16, bender_16_char_0069},
	{LCD_FONT_BENDER_20, bender_20_char_0069},
	{LCD_FONT_BENDER_24, bender_24_char_0069},
	{LCD_FONT_BENDER_28, bender_28_char_0069},
	{LCD_FONT_BENDER_34, bender_34_char_0069},
	{LCD_FONT_BENDER_48, bender_48_char_0069},
	{LCD_FONT_BENDER_72, bender_72_char_0069},
	{LCD_FONT_BENDER_96, bender_96_char_0069},
	{LCD_FONT_BENDER_128, bender_128_char_0069},
};

static const Bender_SizeBitmap bender_glyph_006F[] = {
	{LCD_FONT_BENDER_10, bender_10_char_006F},
	{LCD_FONT_BENDER_14, bender_14_char_006F},
	{LCD_FONT_BENDER_16, bender_16_char_006F},
	{LCD_FONT_BENDER_20, bender_20_char_006F},
	{LCD_FONT_BENDER_24, bender_24_char_006F},
	{LCD_FONT_BENDER_28, bender_28_char_006F},
	{LCD_FONT_BENDER_34, bender_34_char_006F},
	{LCD_FONT_BENDER_48, bender_48_char_006F},
	{LCD_FONT_BENDER_72, bender_72_char_006F},
	{LCD_FONT_BENDER_96, bender_96_char_006F},
	{LCD_FONT_BENDER_128, bender_128_char_006F},
};

static const Bender_SizeBitmap bender_glyph_0070[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0070},
	{LCD_FONT_BENDER_14, bender_14_char_0070},
	{LCD_FONT_BENDER_16, bender_16_char_0070},
	{LCD_FONT_BENDER_20, bender_20_char_0070},
	{LCD_FONT_BENDER_24, bender_24_char_0070},
	{LCD_FONT_BENDER_28, bender_28_char_0070},
	{LCD_FONT_BENDER_34, bender_34_char_0070},
	{LCD_FONT_BENDER_48, bender_48_char_0070},
	{LCD_FONT_BENDER_72, bender_72_char_0070},
	{LCD_FONT_BENDER_96, bender_96_char_0070},
	{LCD_FONT_BENDER_128, bender_128_char_0070},
};

static const Bender_SizeBitmap bender_glyph_0061[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0061},
	{LCD_FONT_BENDER_14, bender_14_char_0061},
	{LCD_FONT_BENDER_16, bender_16_char_0061},
	{LCD_FONT_BENDER_20, bender_20_char_0061},
	{LCD_FONT_BENDER_24, bender_24_char_0061},
	{LCD_FONT_BENDER_28, bender_28_char_0061},
	{LCD_FONT_BENDER_34, bender_34_char_0061},
	{LCD_FONT_BENDER_48, bender_48_char_0061},
	{LCD_FONT_BENDER_72, bender_72_char_0061},
	{LCD_FONT_BENDER_96, bender_96_char_0061},
	{LCD_FONT_BENDER_128, bender_128_char_0061},
};

static const Bender_SizeBitmap bender_glyph_0073[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0073},
	{LCD_FONT_BENDER_14, bender_14_char_0073},
	{LCD_FONT_BENDER_16, bender_16_char_0073},
	{LCD_FONT_BENDER_20, bender_20_char_0073},
	{LCD_FONT_BENDER_24, bender_24_char_0073},
	{LCD_FONT_BENDER_28, bender_28_char_0073},
	{LCD_FONT_BENDER_34, bender_34_char_0073},
	{LCD_FONT_BENDER_48, bender_48_char_0073},
	{LCD_FONT_BENDER_72, bender_72_char_0073},
	{LCD_FONT_BENDER_96, bender_96_char_0073},
	{LCD_FONT_BENDER_128, bender_128_char_0073},
};

static const Bender_SizeBitmap bender_glyph_0064[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0064},
	{LCD_FONT_BENDER_14, bender_14_char_0064},
	{LCD_FONT_BENDER_16, bender_16_char_0064},
	{LCD_FONT_BENDER_20, bender_20_char_0064},
	{LCD_FONT_BENDER_24, bender_24_char_0064},
	{LCD_FONT_BENDER_28, bender_28_char_0064},
	{LCD_FONT_BENDER_34, bender_34_char_0064},
	{LCD_FONT_BENDER_48, bender_48_char_0064},
	{LCD_FONT_BENDER_72, bender_72_char_0064},
	{LCD_FONT_BENDER_96, bender_96_char_0064},
	{LCD_FONT_BENDER_128, bender_128_char_0064},
};

static const Bender_SizeBitmap bender_glyph_0066[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0066},
	{LCD_FONT_BENDER_14, bender_14_char_0066},
	{LCD_FONT_BENDER_16, bender_16_char_0066},
	{LCD_FONT_BENDER_20, bender_20_char_0066},
	{LCD_FONT_BENDER_24, bender_24_char_0066},
	{LCD_FONT_BENDER_28, bender_28_char_0066},
	{LCD_FONT_BENDER_34, bender_34_char_0066},
	{LCD_FONT_BENDER_48, bender_48_char_0066},
	{LCD_FONT_BENDER_72, bender_72_char_0066},
	{LCD_FONT_BENDER_96, bender_96_char_0066},
	{LCD_FONT_BENDER_128, bender_128_char_0066},
};

static const Bender_SizeBitmap bender_glyph_0067[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0067},
	{LCD_FONT_BENDER_14, bender_14_char_0067},
	{LCD_FONT_BENDER_16, bender_16_char_0067},
	{LCD_FONT_BENDER_20, bender_20_char_0067},
	{LCD_FONT_BENDER_24, bender_24_char_0067},
	{LCD_FONT_BENDER_28, bender_28_char_0067},
	{LCD_FONT_BENDER_34, bender_34_char_0067},
	{LCD_FONT_BENDER_48, bender_48_char_0067},
	{LCD_FONT_BENDER_72, bender_72_char_0067},
	{LCD_FONT_BENDER_96, bender_96_char_0067},
	{LCD_FONT_BENDER_128, bender_128_char_0067},
};

static const Bender_SizeBitmap bender_glyph_0068[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0068},
	{LCD_FONT_BENDER_14, bender_14_char_0068},
	{LCD_FONT_BENDER_16, bender_16_char_0068},
	{LCD_FONT_BENDER_20, bender_20_char_0068},
	{LCD_FONT_BENDER_24, bender_24_char_0068},
	{LCD_FONT_BENDER_28, bender_28_char_0068},
	{LCD_FONT_BENDER_34, bender_34_char_0068},
	{LCD_FONT_BENDER_48, bender_48_char_0068},
	{LCD_FONT_BENDER_72, bender_72_char_0068},
	{LCD_FONT_BENDER_96, bender_96_char_0068},
	{LCD_FONT_BENDER_128, bender_128_char_0068},
};

static const Bender_SizeBitmap bender_glyph_006A[] = {
	{LCD_FONT_BENDER_10, bender_10_char_006A},
	{LCD_FONT_BENDER_14, bender_14_char_006A},
	{LCD_FONT_BENDER_16, bender_16_char_006A},
	{LCD_FONT_BENDER_20, bender_20_char_006A},
	{LCD_FONT_BENDER_24, bender_24_char_006A},
	{LCD_FONT_BENDER_28, bender_28_char_006A},
	{LCD_FONT_BENDER_34, bender_34_char_006A},
	{LCD_FONT_BENDER_48, bender_48_char_006A},
	{LCD_FONT_BENDER_72, bender_72_char_006A},
	{LCD_FONT_BENDER_96, bender_96_char_006A},
	{LCD_FONT_BENDER_128, bender_128_char_006A},
};

static const Bender_SizeBitmap bender_glyph_006B[] = {
	{LCD_FONT_BENDER_10, bender_10_char_006B},
	{LCD_FONT_BENDER_14, bender_14_char_006B},
	{LCD_FONT_BENDER_16, bender_16_char_006B},
	{LCD_FONT_BENDER_20, bender_20_char_006B},
	{LCD_FONT_BENDER_24, bender_24_char_006B},
	{LCD_FONT_BENDER_28, bender_28_char_006B},
	{LCD_FONT_BENDER_34, bender_34_char_006B},
	{LCD_FONT_BENDER_48, bender_48_char_006B},
	{LCD_FONT_BENDER_72, bender_72_char_006B},
	{LCD_FONT_BENDER_96, bender_96_char_006B},
	{LCD_FONT_BENDER_128, bender_128_char_006B},
};

static const Bender_SizeBitmap bender_glyph_006C[] = {
	{LCD_FONT_BENDER_10, bender_10_char_006C},
	{LCD_FONT_BENDER_14, bender_14_char_006C},
	{LCD_FONT_BENDER_16, bender_16_char_006C},
	{LCD_FONT_BENDER_20, bender_20_char_006C},
	{LCD_FONT_BENDER_24, bender_24_char_006C},
	{LCD_FONT_BENDER_28, bender_28_char_006C},
	{LCD_FONT_BENDER_34, bender_34_char_006C},
	{LCD_FONT_BENDER_48, bender_48_char_006C},
	{LCD_FONT_BENDER_72, bender_72_char_006C},
	{LCD_FONT_BENDER_96, bender_96_char_006C},
	{LCD_FONT_BENDER_128, bender_128_char_006C},
};

static const Bender_SizeBitmap bender_glyph_007A[] = {
	{LCD_FONT_BENDER_10, bender_10_char_007A},
	{LCD_FONT_BENDER_14, bender_14_char_007A},
	{LCD_FONT_BENDER_16, bender_16_char_007A},
	{LCD_FONT_BENDER_20, bender_20_char_007A},
	{LCD_FONT_BENDER_24, bender_24_char_007A},
	{LCD_FONT_BENDER_28, bender_28_char_007A},
	{LCD_FONT_BENDER_34, bender_34_char_007A},
	{LCD_FONT_BENDER_48, bender_48_char_007A},
	{LCD_FONT_BENDER_72, bender_72_char_007A},
	{LCD_FONT_BENDER_96, bender_96_char_007A},
	{LCD_FONT_BENDER_128, bender_128_char_007A},
};

static const Bender_SizeBitmap bender_glyph_0078[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0078},
	{LCD_FONT_BENDER_14, bender_14_char_0078},
	{LCD_FONT_BENDER_16, bender_16_char_0078},
	{LCD_FONT_BENDER_20, bender_20_char_0078},
	{LCD_FONT_BENDER_24, bender_24_char_0078},
	{LCD_FONT_BENDER_28, bender_28_char_0078},
	{LCD_FONT_BENDER_34, bender_34_char_0078},
	{LCD_FONT_BENDER_48, bender_48_char_0078},
	{LCD_FONT_BENDER_72, bender_72_char_0078},
	{LCD_FONT_BENDER_96, bender_96_char_0078},
	{LCD_FONT_BENDER_128, bender_128_char_0078},
};

static const Bender_SizeBitmap bender_glyph_0063[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0063},
	{LCD_FONT_BENDER_14, bender_14_char_0063},
	{LCD_FONT_BENDER_16, bender_16_char_0063},
	{LCD_FONT_BENDER_20, bender_20_char_0063},
	{LCD_FONT_BENDER_24, bender_24_char_0063},
	{LCD_FONT_BENDER_28, bender_28_char_0063},
	{LCD_FONT_BENDER_34, bender_34_char_0063},
	{LCD_FONT_BENDER_48, bender_48_char_0063},
	{LCD_FONT_BENDER_72, bender_72_char_0063},
	{LCD_FONT_BENDER_96, bender_96_char_0063},
	{LCD_FONT_BENDER_128, bender_128_char_0063},
};

static const Bender_SizeBitmap bender_glyph_0076[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0076},
	{LCD_FONT_BENDER_14, bender_14_char_0076},
	{LCD_FONT_BENDER_16, bender_16_char_0076},
	{LCD_FONT_BENDER_20, bender_20_char_0076},
	{LCD_FONT_BENDER_24, bender_24_char_0076},
	{LCD_FONT_BENDER_28, bender_28_char_0076},
	{LCD_FONT_BENDER_34, bender_34_char_0076},
	{LCD_FONT_BENDER_48, bender_48_char_0076},
	{LCD_FONT_BENDER_72, bender_72_char_0076},
	{LCD_FONT_BENDER_96, bender_96_char_0076},
	{LCD_FONT_BENDER_128, bender_128_char_0076},
};

static const Bender_SizeBitmap bender_glyph_0062[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0062},
	{LCD_FONT_BENDER_14, bender_14_char_0062},
	{LCD_FONT_BENDER_16, bender_16_char_0062},
	{LCD_FONT_BENDER_20, bender_20_char_0062},
	{LCD_FONT_BENDER_24, bender_24_char_0062},
	{LCD_FONT_BENDER_28, bender_28_char_0062},
	{LCD_FONT_BENDER_34, bender_34_char_0062},
	{LCD_FONT_BENDER_48, bender_48_char_0062},
	{LCD_FONT_BENDER_72, bender_72_char_0062},
	{LCD_FONT_BENDER_96, bender_96_char_0062},
	{LCD_FONT_BENDER_128, bender_128_char_0062},
};

static const Bender_SizeBitmap bender_glyph_006E[] = {
	{LCD_FONT_BENDER_10, bender_10_char_006E},
	{LCD_FONT_BENDER_14, bender_14_char_006E},
	{LCD_FONT_BENDER_16, bender_16_char_006E},
	{LCD_FONT_BENDER_20, bender_20_char_006E},
	{LCD_FONT_BENDER_24, bender_24_char_006E},
	{LCD_FONT_BENDER_28, bender_28_char_006E},
	{LCD_FONT_BENDER_34, bender_34_char_006E},
	{LCD_FONT_BENDER_48, bender_48_char_006E},
	{LCD_FONT_BENDER_72, bender_72_char_006E},
	{LCD_FONT_BENDER_96, bender_96_char_006E},
	{LCD_FONT_BENDER_128, bender_128_char_006E},
};

static const Bender_SizeBitmap bender_glyph_006D[] = {
	{LCD_FONT_BENDER_10, bender_10_char_006D},
	{LCD_FONT_BENDER_14, bender_14_char_006D},
	{LCD_FONT_BENDER_16, bender_16_char_006D},
	{LCD_FONT_BENDER_20, bender_20_char_006D},
	{LCD_FONT_BENDER_24, bender_24_char_006D},
	{LCD_FONT_BENDER_28, bender_28_char_006D},
	{LCD_FONT_BENDER_34, bender_34_char_006D},
	{LCD_FONT_BENDER_48, bender_48_char_006D},
	{LCD_FONT_BENDER_72, bender_72_char_006D},
	{LCD_FONT_BENDER_96, bender_96_char_006D},
	{LCD_FONT_BENDER_128, bender_128_char_006D},
};

static const Bender_SizeBitmap bender_glyph_0051[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0051},
	{LCD_FONT_BENDER_14, bender_14_char_0051},
	{LCD_FONT_BENDER_16, bender_16_char_0051},
	{LCD_FONT_BENDER_20, bender_20_char_0051},
	{LCD_FONT_BENDER_24, bender_24_char_0051},
	{LCD_FONT_BENDER_28, bender_28_char_0051},
	{LCD_FONT_BENDER_34, bender_34_char_0051},
	{LCD_FONT_BENDER_48, bender_48_char_0051},
	{LCD_FONT_BENDER_72, bender_72_char_0051},
	{LCD_FONT_BENDER_96, bender_96_char_0051},
	{LCD_FONT_BENDER_128, bender_128_char_0051},
};

static const Bender_SizeBitmap bender_glyph_0057[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0057},
	{LCD_FONT_BENDER_14, bender_14_char_0057},
	{LCD_FONT_BENDER_16, bender_16_char_0057},
	{LCD_FONT_BENDER_20, bender_20_char_0057},
	{LCD_FONT_BENDER_24, bender_24_char_0057},
	{LCD_FONT_BENDER_28, bender_28_char_0057},
	{LCD_FONT_BENDER_34, bender_34_char_0057},
	{LCD_FONT_BENDER_48, bender_48_char_0057},
	{LCD_FONT_BENDER_72, bender_72_char_0057},
	{LCD_FONT_BENDER_96, bender_96_char_0057},
	{LCD_FONT_BENDER_128, bender_128_char_0057},
};

static const Bender_SizeBitmap bender_glyph_0045[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0045},
	{LCD_FONT_BENDER_14, bender_14_char_0045},
	{LCD_FONT_BENDER_16, bender_16_char_0045},
	{LCD_FONT_BENDER_20, bender_20_char_0045},
	{LCD_FONT_BENDER_24, bender_24_char_0045},
	{LCD_FONT_BENDER_28, bender_28_char_0045},
	{LCD_FONT_BENDER_34, bender_34_char_0045},
	{LCD_FONT_BENDER_48, bender_48_char_0045},
	{LCD_FONT_BENDER_72, bender_72_char_0045},
	{LCD_FONT_BENDER_96, bender_96_char_0045},
	{LCD_FONT_BENDER_128, bender_128_char_0045},
};

static const Bender_SizeBitmap bender_glyph_0052[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0052},
	{LCD_FONT_BENDER_14, bender_14_char_0052},
	{LCD_FONT_BENDER_16, bender_16_char_0052},
	{LCD_FONT_BENDER_20, bender_20_char_0052},
	{LCD_FONT_BENDER_24, bender_24_char_0052},
	{LCD_FONT_BENDER_28, bender_28_char_0052},
	{LCD_FONT_BENDER_34, bender_34_char_0052},
	{LCD_FONT_BENDER_48, bender_48_char_0052},
	{LCD_FONT_BENDER_72, bender_72_char_0052},
	{LCD_FONT_BENDER_96, bender_96_char_0052},
	{LCD_FONT_BENDER_128, bender_128_char_0052},
};

static const Bender_SizeBitmap bender_glyph_0054[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0054},
	{LCD_FONT_BENDER_14, bender_14_char_0054},
	{LCD_FONT_BENDER_16, bender_16_char_0054},
	{LCD_FONT_BENDER_20, bender_20_char_0054},
	{LCD_FONT_BENDER_24, bender_24_char_0054},
	{LCD_FONT_BENDER_28, bender_28_char_0054},
	{LCD_FONT_BENDER_34, bender_34_char_0054},
	{LCD_FONT_BENDER_48, bender_48_char_0054},
	{LCD_FONT_BENDER_72, bender_72_char_0054},
	{LCD_FONT_BENDER_96, bender_96_char_0054},
	{LCD_FONT_BENDER_128, bender_128_char_0054},
};

static const Bender_SizeBitmap bender_glyph_0059[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0059},
	{LCD_FONT_BENDER_14, bender_14_char_0059},
	{LCD_FONT_BENDER_16, bender_16_char_0059},
	{LCD_FONT_BENDER_20, bender_20_char_0059},
	{LCD_FONT_BENDER_24, bender_24_char_0059},
	{LCD_FONT_BENDER_28, bender_28_char_0059},
	{LCD_FONT_BENDER_34, bender_34_char_0059},
	{LCD_FONT_BENDER_48, bender_48_char_0059},
	{LCD_FONT_BENDER_72, bender_72_char_0059},
	{LCD_FONT_BENDER_96, bender_96_char_0059},
	{LCD_FONT_BENDER_128, bender_128_char_0059},
};

static const Bender_SizeBitmap bender_glyph_0055[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0055},
	{LCD_FONT_BENDER_14, bender_14_char_0055},
	{LCD_FONT_BENDER_16, bender_16_char_0055},
	{LCD_FONT_BENDER_20, bender_20_char_0055},
	{LCD_FONT_BENDER_24, bender_24_char_0055},
	{LCD_FONT_BENDER_28, bender_28_char_0055},
	{LCD_FONT_BENDER_34, bender_34_char_0055},
	{LCD_FONT_BENDER_48, bender_48_char_0055},
	{LCD_FONT_BENDER_72, bender_72_char_0055},
	{LCD_FONT_BENDER_96, bender_96_char_0055},
	{LCD_FONT_BENDER_128, bender_128_char_0055},
};

static const Bender_SizeBitmap bender_glyph_0049[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0049},
	{LCD_FONT_BENDER_14, bender_14_char_0049},
	{LCD_FONT_BENDER_16, bender_16_char_0049},
	{LCD_FONT_BENDER_20, bender_20_char_0049},
	{LCD_FONT_BENDER_24, bender_24_char_0049},
	{LCD_FONT_BENDER_28, bender_28_char_0049},
	{LCD_FONT_BENDER_34, bender_34_char_0049},
	{LCD_FONT_BENDER_48, bender_48_char_0049},
	{LCD_FONT_BENDER_72, bender_72_char_0049},
	{LCD_FONT_BENDER_96, bender_96_char_0049},
	{LCD_FONT_BENDER_128, bender_128_char_0049},
};

static const Bender_SizeBitmap bender_glyph_004F[] = {
	{LCD_FONT_BENDER_10, bender_10_char_004F},
	{LCD_FONT_BENDER_14, bender_14_char_004F},
	{LCD_FONT_BENDER_16, bender_16_char_004F},
	{LCD_FONT_BENDER_20, bender_20_char_004F},
	{LCD_FONT_BENDER_24, bender_24_char_004F},
	{LCD_FONT_BENDER_28, bender_28_char_004F},
	{LCD_FONT_BENDER_34, bender_34_char_004F},
	{LCD_FONT_BENDER_48, bender_48_char_004F},
	{LCD_FONT_BENDER_72, bender_72_char_004F},
	{LCD_FONT_BENDER_96, bender_96_char_004F},
	{LCD_FONT_BENDER_128, bender_128_char_004F},
};

static const Bender_SizeBitmap bender_glyph_0050[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0050},
	{LCD_FONT_BENDER_14, bender_14_char_0050},
	{LCD_FONT_BENDER_16, bender_16_char_0050},
	{LCD_FONT_BENDER_20, bender_20_char_0050},
	{LCD_FONT_BENDER_24, bender_24_char_0050},
	{LCD_FONT_BENDER_28, bender_28_char_0050},
	{LCD_FONT_BENDER_34, bender_34_char_0050},
	{LCD_FONT_BENDER_48, bender_48_char_0050},
	{LCD_FONT_BENDER_72, bender_72_char_0050},
	{LCD_FONT_BENDER_96, bender_96_char_0050},
	{LCD_FONT_BENDER_128, bender_128_char_0050},
};

static const Bender_SizeBitmap bender_glyph_004C[] = {
	{LCD_FONT_BENDER_10, bender_10_char_004C},
	{LCD_FONT_BENDER_14, bender_14_char_004C},
	{LCD_FONT_BENDER_16, bender_16_char_004C},
	{LCD_FONT_BENDER_20, bender_20_char_004C},
	{LCD_FONT_BENDER_24, bender_24_char_004C},
	{LCD_FONT_BENDER_28, bender_28_char_004C},
	{LCD_FONT_BENDER_34, bender_34_char_004C},
	{LCD_FONT_BENDER_48, bender_48_char_004C},
	{LCD_FONT_BENDER_72, bender_72_char_004C},
	{LCD_FONT_BENDER_96, bender_96_char_004C},
	{LCD_FONT_BENDER_128, bender_128_char_004C},
};

static const Bender_SizeBitmap bender_glyph_004B[] = {
	{LCD_FONT_BENDER_10, bender_10_char_004B},
	{LCD_FONT_BENDER_14, bender_14_char_004B},
	{LCD_FONT_BENDER_16, bender_16_char_004B},
	{LCD_FONT_BENDER_20, bender_20_char_004B},
	{LCD_FONT_BENDER_24, bender_24_char_004B},
	{LCD_FONT_BENDER_28, bender_28_char_004B},
	{LCD_FONT_BENDER_34, bender_34_char_004B},
	{LCD_FONT_BENDER_48, bender_48_char_004B},
	{LCD_FONT_BENDER_72, bender_72_char_004B},
	{LCD_FONT_BENDER_96, bender_96_char_004B},
	{LCD_FONT_BENDER_128, bender_128_char_004B},
};

static const Bender_SizeBitmap bender_glyph_004A[] = {
	{LCD_FONT_BENDER_10, bender_10_char_004A},
	{LCD_FONT_BENDER_14, bender_14_char_004A},
	{LCD_FONT_BENDER_16, bender_16_char_004A},
	{LCD_FONT_BENDER_20, bender_20_char_004A},
	{LCD_FONT_BENDER_24, bender_24_char_004A},
	{LCD_FONT_BENDER_28, bender_28_char_004A},
	{LCD_FONT_BENDER_34, bender_34_char_004A},
	{LCD_FONT_BENDER_48, bender_48_char_004A},
	{LCD_FONT_BENDER_72, bender_72_char_004A},
	{LCD_FONT_BENDER_96, bender_96_char_004A},
	{LCD_FONT_BENDER_128, bender_128_char_004A},
};

static const Bender_SizeBitmap bender_glyph_0048[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0048},
	{LCD_FONT_BENDER_14, bender_14_char_0048},
	{LCD_FONT_BENDER_16, bender_16_char_0048},
	{LCD_FONT_BENDER_20, bender_20_char_0048},
	{LCD_FONT_BENDER_24, bender_24_char_0048},
	{LCD_FONT_BENDER_28, bender_28_char_0048},
	{LCD_FONT_BENDER_34, bender_34_char_0048},
	{LCD_FONT_BENDER_48, bender_48_char_0048},
	{LCD_FONT_BENDER_72, bender_72_char_0048},
	{LCD_FONT_BENDER_96, bender_96_char_0048},
	{LCD_FONT_BENDER_128, bender_128_char_0048},
};

static const Bender_SizeBitmap bender_glyph_0047[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0047},
	{LCD_FONT_BENDER_14, bender_14_char_0047},
	{LCD_FONT_BENDER_16, bender_16_char_0047},
	{LCD_FONT_BENDER_20, bender_20_char_0047},
	{LCD_FONT_BENDER_24, bender_24_char_0047},
	{LCD_FONT_BENDER_28, bender_28_char_0047},
	{LCD_FONT_BENDER_34, bender_34_char_0047},
	{LCD_FONT_BENDER_48, bender_48_char_0047},
	{LCD_FONT_BENDER_72, bender_72_char_0047},
	{LCD_FONT_BENDER_96, bender_96_char_0047},
	{LCD_FONT_BENDER_128, bender_128_char_0047},
};

static const Bender_SizeBitmap bender_glyph_0046[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0046},
	{LCD_FONT_BENDER_14, bender_14_char_0046},
	{LCD_FONT_BENDER_16, bender_16_char_0046},
	{LCD_FONT_BENDER_20, bender_20_char_0046},
	{LCD_FONT_BENDER_24, bender_24_char_0046},
	{LCD_FONT_BENDER_28, bender_28_char_0046},
	{LCD_FONT_BENDER_34, bender_34_char_0046},
	{LCD_FONT_BENDER_48, bender_48_char_0046},
	{LCD_FONT_BENDER_72, bender_72_char_0046},
	{LCD_FONT_BENDER_96, bender_96_char_0046},
	{LCD_FONT_BENDER_128, bender_128_char_0046},
};

static const Bender_SizeBitmap bender_glyph_0044[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0044},
	{LCD_FONT_BENDER_14, bender_14_char_0044},
	{LCD_FONT_BENDER_16, bender_16_char_0044},
	{LCD_FONT_BENDER_20, bender_20_char_0044},
	{LCD_FONT_BENDER_24, bender_24_char_0044},
	{LCD_FONT_BENDER_28, bender_28_char_0044},
	{LCD_FONT_BENDER_34, bender_34_char_0044},
	{LCD_FONT_BENDER_48, bender_48_char_0044},
	{LCD_FONT_BENDER_72, bender_72_char_0044},
	{LCD_FONT_BENDER_96, bender_96_char_0044},
	{LCD_FONT_BENDER_128, bender_128_char_0044},
};

static const Bender_SizeBitmap bender_glyph_0053[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0053},
	{LCD_FONT_BENDER_14, bender_14_char_0053},
	{LCD_FONT_BENDER_16, bender_16_char_0053},
	{LCD_FONT_BENDER_20, bender_20_char_0053},
	{LCD_FONT_BENDER_24, bender_24_char_0053},
	{LCD_FONT_BENDER_28, bender_28_char_0053},
	{LCD_FONT_BENDER_34, bender_34_char_0053},
	{LCD_FONT_BENDER_48, bender_48_char_0053},
	{LCD_FONT_BENDER_72, bender_72_char_0053},
	{LCD_FONT_BENDER_96, bender_96_char_0053},
	{LCD_FONT_BENDER_128, bender_128_char_0053},
};

static const Bender_SizeBitmap bender_glyph_0041[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0041},
	{LCD_FONT_BENDER_14, bender_14_char_0041},
	{LCD_FONT_BENDER_16, bender_16_char_0041},
	{LCD_FONT_BENDER_20, bender_20_char_0041},
	{LCD_FONT_BENDER_24, bender_24_char_0041},
	{LCD_FONT_BENDER_28, bender_28_char_0041},
	{LCD_FONT_BENDER_34, bender_34_char_0041},
	{LCD_FONT_BENDER_48, bender_48_char_0041},
	{LCD_FONT_BENDER_72, bender_72_char_0041},
	{LCD_FONT_BENDER_96, bender_96_char_0041},
	{LCD_FONT_BENDER_128, bender_128_char_0041},
};

static const Bender_SizeBitmap bender_glyph_005A[] = {
	{LCD_FONT_BENDER_10, bender_10_char_005A},
	{LCD_FONT_BENDER_14, bender_14_char_005A},
	{LCD_FONT_BENDER_16, bender_16_char_005A},
	{LCD_FONT_BENDER_20, bender_20_char_005A},
	{LCD_FONT_BENDER_24, bender_24_char_005A},
	{LCD_FONT_BENDER_28, bender_28_char_005A},
	{LCD_FONT_BENDER_34, bender_34_char_005A},
	{LCD_FONT_BENDER_48, bender_48_char_005A},
	{LCD_FONT_BENDER_72, bender_72_char_005A},
	{LCD_FONT_BENDER_96, bender_96_char_005A},
	{LCD_FONT_BENDER_128, bender_128_char_005A},
};

static const Bender_SizeBitmap bender_glyph_0058[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0058},
	{LCD_FONT_BENDER_14, bender_14_char_0058},
	{LCD_FONT_BENDER_16, bender_16_char_0058},
	{LCD_FONT_BENDER_20, bender_20_char_0058},
	{LCD_FONT_BENDER_24, bender_24_char_0058},
	{LCD_FONT_BENDER_28, bender_28_char_0058},
	{LCD_FONT_BENDER_34, bender_34_char_0058},
	{LCD_FONT_BENDER_48, bender_48_char_0058},
	{LCD_FONT_BENDER_72, bender_72_char_0058},
	{LCD_FONT_BENDER_96, bender_96_char_0058},
	{LCD_FONT_BENDER_128, bender_128_char_0058},
};

static const Bender_SizeBitmap bender_glyph_0043[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0043},
	{LCD_FONT_BENDER_14, bender_14_char_0043},
	{LCD_FONT_BENDER_16, bender_16_char_0043},
	{LCD_FONT_BENDER_20, bender_20_char_0043},
	{LCD_FONT_BENDER_24, bender_24_char_0043},
	{LCD_FONT_BENDER_28, bender_28_char_0043},
	{LCD_FONT_BENDER_34, bender_34_char_0043},
	{LCD_FONT_BENDER_48, bender_48_char_0043},
	{LCD_FONT_BENDER_72, bender_72_char_0043},
	{LCD_FONT_BENDER_96, bender_96_char_0043},
	{LCD_FONT_BENDER_128, bender_128_char_0043},
};

static const Bender_SizeBitmap bender_glyph_0056[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0056},
	{LCD_FONT_BENDER_14, bender_14_char_0056},
	{LCD_FONT_BENDER_16, bender_16_char_0056},
	{LCD_FONT_BENDER_20, bender_20_char_0056},
	{LCD_FONT_BENDER_24, bender_24_char_0056},
	{LCD_FONT_BENDER_28, bender_28_char_0056},
	{LCD_FONT_BENDER_34, bender_34_char_0056},
	{LCD_FONT_BENDER_48, bender_48_char_0056},
	{LCD_FONT_BENDER_72, bender_72_char_0056},
	{LCD_FONT_BENDER_96, bender_96_char_0056},
	{LCD_FONT_BENDER_128, bender_128_char_0056},
};

static const Bender_SizeBitmap bender_glyph_0042[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0042},
	{LCD_FONT_BENDER_14, bender_14_char_0042},
	{LCD_FONT_BENDER_16, bender_16_char_0042},
	{LCD_FONT_BENDER_20, bender_20_char_0042},
	{LCD_FONT_BENDER_24, bender_24_char_0042},
	{LCD_FONT_BENDER_28, bender_28_char_0042},
	{LCD_FONT_BENDER_34, bender_34_char_0042},
	{LCD_FONT_BENDER_48, bender_48_char_0042},
	{LCD_FONT_BENDER_72, bender_72_char_0042},
	{LCD_FONT_BENDER_96, bender_96_char_0042},
	{LCD_FONT_BENDER_128, bender_128_char_0042},
};

static const Bender_SizeBitmap bender_glyph_004E[] = {
	{LCD_FONT_BENDER_10, bender_10_char_004E},
	{LCD_FONT_BENDER_14, bender_14_char_004E},
	{LCD_FONT_BENDER_16, bender_16_char_004E},
	{LCD_FONT_BENDER_20, bender_20_char_004E},
	{LCD_FONT_BENDER_24, bender_24_char_004E},
	{LCD_FONT_BENDER_28, bender_28_char_004E},
	{LCD_FONT_BENDER_34, bender_34_char_004E},
	{LCD_FONT_BENDER_48, bender_48_char_004E},
	{LCD_FONT_BENDER_72, bender_72_char_004E},
	{LCD_FONT_BENDER_96, bender_96_char_004E},
	{LCD_FONT_BENDER_128, bender_128_char_004E},
};

static const Bender_SizeBitmap bender_glyph_004D[] = {
	{LCD_FONT_BENDER_10, bender_10_char_004D},
	{LCD_FONT_BENDER_14, bender_14_char_004D},
	{LCD_FONT_BENDER_16, bender_16_char_004D},
	{LCD_FONT_BENDER_20, bender_20_char_004D},
	{LCD_FONT_BENDER_24, bender_24_char_004D},
	{LCD_FONT_BENDER_28, bender_28_char_004D},
	{LCD_FONT_BENDER_34, bender_34_char_004D},
	{LCD_FONT_BENDER_48, bender_48_char_004D},
	{LCD_FONT_BENDER_72, bender_72_char_004D},
	{LCD_FONT_BENDER_96, bender_96_char_004D},
	{LCD_FONT_BENDER_128, bender_128_char_004D},
};

static const Bender_SizeBitmap bender_glyph_002E[] = {
	{LCD_FONT_BENDER_10, bender_10_char_002E},
	{LCD_FONT_BENDER_14, bender_14_char_002E},
	{LCD_FONT_BENDER_16, bender_16_char_002E},
	{LCD_FONT_BENDER_20, bender_20_char_002E},
	{LCD_FONT_BENDER_24, bender_24_char_002E},
	{LCD_FONT_BENDER_28, bender_28_char_002E},
	{LCD_FONT_BENDER_34, bender_34_char_002E},
	{LCD_FONT_BENDER_48, bender_48_char_002E},
	{LCD_FONT_BENDER_72, bender_72_char_002E},
	{LCD_FONT_BENDER_96, bender_96_char_002E},
	{LCD_FONT_BENDER_128, bender_128_char_002E},
};

static const Bender_SizeBitmap bender_glyph_003A[] = {
	{LCD_FONT_BENDER_10, bender_10_char_003A},
	{LCD_FONT_BENDER_14, bender_14_char_003A},
	{LCD_FONT_BENDER_16, bender_16_char_003A},
	{LCD_FONT_BENDER_20, bender_20_char_003A},
	{LCD_FONT_BENDER_24, bender_24_char_003A},
	{LCD_FONT_BENDER_28, bender_28_char_003A},
	{LCD_FONT_BENDER_34, bender_34_char_003A},
	{LCD_FONT_BENDER_48, bender_48_char_003A},
	{LCD_FONT_BENDER_72, bender_72_char_003A},
	{LCD_FONT_BENDER_96, bender_96_char_003A},
	{LCD_FONT_BENDER_128, bender_128_char_003A},
};

static const Bender_SizeBitmap bender_glyph_002D[] = {
	{LCD_FONT_BENDER_10, bender_10_char_002D},
	{LCD_FONT_BENDER_14, bender_14_char_002D},
	{LCD_FONT_BENDER_16, bender_16_char_002D},
	{LCD_FONT_BENDER_20, bender_20_char_002D},
	{LCD_FONT_BENDER_24, bender_24_char_002D},
	{LCD_FONT_BENDER_28, bender_28_char_002D},
	{LCD_FONT_BENDER_34, bender_34_char_002D},
	{LCD_FONT_BENDER_48, bender_48_char_002D},
	{LCD_FONT_BENDER_72, bender_72_char_002D},
	{LCD_FONT_BENDER_96, bender_96_char_002D},
	{LCD_FONT_BENDER_128, bender_128_char_002D},
};

static const Bender_SizeBitmap bender_glyph_003D[] = {
	{LCD_FONT_BENDER_10, bender_10_char_003D},
	{LCD_FONT_BENDER_14, bender_14_char_003D},
	{LCD_FONT_BENDER_16, bender_16_char_003D},
	{LCD_FONT_BENDER_20, bender_20_char_003D},
	{LCD_FONT_BENDER_24, bender_24_char_003D},
	{LCD_FONT_BENDER_28, bender_28_char_003D},
	{LCD_FONT_BENDER_34, bender_34_char_003D},
	{LCD_FONT_BENDER_48, bender_48_char_003D},
	{LCD_FONT_BENDER_72, bender_72_char_003D},
	{LCD_FONT_BENDER_96, bender_96_char_003D},
	{LCD_FONT_BENDER_128, bender_128_char_003D},
};

static const Bender_SizeBitmap bender_glyph_002B[] = {
	{LCD_FONT_BENDER_10, bender_10_char_002B},
	{LCD_FONT_BENDER_14, bender_14_char_002B},
	{LCD_FONT_BENDER_16, bender_16_char_002B},
	{LCD_FONT_BENDER_20, bender_20_char_002B},
	{LCD_FONT_BENDER_24, bender_24_char_002B},
	{LCD_FONT_BENDER_28, bender_28_char_002B},
	{LCD_FONT_BENDER_34, bender_34_char_002B},
	{LCD_FONT_BENDER_48, bender_48_char_002B},
	{LCD_FONT_BENDER_72, bender_72_char_002B},
	{LCD_FONT_BENDER_96, bender_96_char_002B},
	{LCD_FONT_BENDER_128, bender_128_char_002B},
};

static const Bender_SizeBitmap bender_glyph_005F[] = {
	{LCD_FONT_BENDER_10, bender_10_char_005F},
	{LCD_FONT_BENDER_14, bender_14_char_005F},
	{LCD_FONT_BENDER_16, bender_16_char_005F},
	{LCD_FONT_BENDER_20, bender_20_char_005F},
	{LCD_FONT_BENDER_24, bender_24_char_005F},
	{LCD_FONT_BENDER_28, bender_28_char_005F},
	{LCD_FONT_BENDER_34, bender_34_char_005F},
	{LCD_FONT_BENDER_48, bender_48_char_005F},
	{LCD_FONT_BENDER_72, bender_72_char_005F},
	{LCD_FONT_BENDER_96, bender_96_char_005F},
	{LCD_FONT_BENDER_128, bender_128_char_005F},
};

static const Bender_SizeBitmap bender_glyph_0025[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0025},
	{LCD_FONT_BENDER_14, bender_14_char_0025},
	{LCD_FONT_BENDER_16, bender_16_char_0025},
	{LCD_FONT_BENDER_20, bender_20_char_0025},
	{LCD_FONT_BENDER_24, bender_24_char_0025},
	{LCD_FONT_BENDER_28, bender_28_char_0025},
	{LCD_FONT_BENDER_34, bender_34_char_0025},
	{LCD_FONT_BENDER_48, bender_48_char_0025},
	{LCD_FONT_BENDER_72, bender_72_char_0025},
	{LCD_FONT_BENDER_96, bender_96_char_0025},
	{LCD_FONT_BENDER_128, bender_128_char_0025},
};

static const Bender_SizeBitmap bender_glyph_002A[] = {
	{LCD_FONT_BENDER_10, bender_10_char_002A},
	{LCD_FONT_BENDER_14, bender_14_char_002A},
	{LCD_FONT_BENDER_16, bender_16_char_002A},
	{LCD_FONT_BENDER_20, bender_20_char_002A},
	{LCD_FONT_BENDER_24, bender_24_char_002A},
	{LCD_FONT_BENDER_28, bender_28_char_002A},
	{LCD_FONT_BENDER_34, bender_34_char_002A},
	{LCD_FONT_BENDER_48, bender_48_char_002A},
	{LCD_FONT_BENDER_72, bender_72_char_002A},
	{LCD_FONT_BENDER_96, bender_96_char_002A},
	{LCD_FONT_BENDER_128, bender_128_char_002A},
};

static const Bender_SizeBitmap bender_glyph_0023[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0023},
	{LCD_FONT_BENDER_14, bender_14_char_0023},
	{LCD_FONT_BENDER_16, bender_16_char_0023},
	{LCD_FONT_BENDER_20, bender_20_char_0023},
	{LCD_FONT_BENDER_24, bender_24_char_0023},
	{LCD_FONT_BENDER_28, bender_28_char_0023},
	{LCD_FONT_BENDER_34, bender_34_char_0023},
	{LCD_FONT_BENDER_48, bender_48_char_0023},
	{LCD_FONT_BENDER_72, bender_72_char_0023},
	{LCD_FONT_BENDER_96, bender_96_char_0023},
	{LCD_FONT_BENDER_128, bender_128_char_0023},
};

static const Bender_SizeBitmap bender_glyph_0040[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0040},
	{LCD_FONT_BENDER_14, bender_14_char_0040},
	{LCD_FONT_BENDER_16, bender_16_char_0040},
	{LCD_FONT_BENDER_20, bender_20_char_0040},
	{LCD_FONT_BENDER_24, bender_24_char_0040},
	{LCD_FONT_BENDER_28, bender_28_char_0040},
	{LCD_FONT_BENDER_34, bender_34_char_0040},
	{LCD_FONT_BENDER_48, bender_48_char_0040},
	{LCD_FONT_BENDER_72, bender_72_char_0040},
	{LCD_FONT_BENDER_96, bender_96_char_0040},
	{LCD_FONT_BENDER_128, bender_128_char_0040},
};

static const Bender_SizeBitmap bender_glyph_0021[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0021},
	{LCD_FONT_BENDER_14, bender_14_char_0021},
	{LCD_FONT_BENDER_16, bender_16_char_0021},
	{LCD_FONT_BENDER_20, bender_20_char_0021},
	{LCD_FONT_BENDER_24, bender_24_char_0021},
	{LCD_FONT_BENDER_28, bender_28_char_0021},
	{LCD_FONT_BENDER_34, bender_34_char_0021},
	{LCD_FONT_BENDER_48, bender_48_char_0021},
	{LCD_FONT_BENDER_72, bender_72_char_0021},
	{LCD_FONT_BENDER_96, bender_96_char_0021},
	{LCD_FONT_BENDER_128, bender_128_char_0021},
};

static const Bender_SizeBitmap bender_glyph_005E[] = {
	{LCD_FONT_BENDER_10, bender_10_char_005E},
	{LCD_FONT_BENDER_14, bender_14_char_005E},
	{LCD_FONT_BENDER_16, bender_16_char_005E},
	{LCD_FONT_BENDER_20, bender_20_char_005E},
	{LCD_FONT_BENDER_24, bender_24_char_005E},
	{LCD_FONT_BENDER_28, bender_28_char_005E},
	{LCD_FONT_BENDER_34, bender_34_char_005E},
	{LCD_FONT_BENDER_48, bender_48_char_005E},
	{LCD_FONT_BENDER_72, bender_72_char_005E},
	{LCD_FONT_BENDER_96, bender_96_char_005E},
	{LCD_FONT_BENDER_128, bender_128_char_005E},
};

static const Bender_SizeBitmap bender_glyph_0026[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0026},
	{LCD_FONT_BENDER_14, bender_14_char_0026},
	{LCD_FONT_BENDER_16, bender_16_char_0026},
	{LCD_FONT_BENDER_20, bender_20_char_0026},
	{LCD_FONT_BENDER_24, bender_24_char_0026},
	{LCD_FONT_BENDER_28, bender_28_char_0026},
	{LCD_FONT_BENDER_34, bender_34_char_0026},
	{LCD_FONT_BENDER_48, bender_48_char_0026},
	{LCD_FONT_BENDER_72, bender_72_char_0026},
	{LCD_FONT_BENDER_96, bender_96_char_0026},
	{LCD_FONT_BENDER_128, bender_128_char_0026},
};

static const Bender_SizeBitmap bender_glyph_0028[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0028},
	{LCD_FONT_BENDER_14, bender_14_char_0028},
	{LCD_FONT_BENDER_16, bender_16_char_0028},
	{LCD_FONT_BENDER_20, bender_20_char_0028},
	{LCD_FONT_BENDER_24, bender_24_char_0028},
	{LCD_FONT_BENDER_28, bender_28_char_0028},
	{LCD_FONT_BENDER_34, bender_34_char_0028},
	{LCD_FONT_BENDER_48, bender_48_char_0028},
	{LCD_FONT_BENDER_72, bender_72_char_0028},
	{LCD_FONT_BENDER_96, bender_96_char_0028},
	{LCD_FONT_BENDER_128, bender_128_char_0028},
};

static const Bender_SizeBitmap bender_glyph_0029[] = {
	{LCD_FONT_BENDER_10, bender_10_char_0029},
	{LCD_FONT_BENDER_14, bender_14_char_0029},
	{LCD_FONT_BENDER_16, bender_16_char_0029},
	{LCD_FONT_BENDER_20, bender_20_char_0029},
	{LCD_FONT_BENDER_24, bender_24_char_0029},
	{LCD_FONT_BENDER_28, bender_28_char_0029},
	{LCD_FONT_BENDER_34, bender_34_char_0029},
	{LCD_FONT_BENDER_48, bender_48_char_0029},
	{LCD_FONT_BENDER_72, bender_72_char_0029},
	{LCD_FONT_BENDER_96, bender_96_char_0029},
	{LCD_FONT_BENDER_128, bender_128_char_0029},
};

static const Bender_Glyph bender_glyphs[] = {
	{'1', bender_glyph_0031, sizeof(bender_glyph_0031) / sizeof(bender_glyph_0031[0])},
	{'2', bender_glyph_0032, sizeof(bender_glyph_0032) / sizeof(bender_glyph_0032[0])},
	{'3', bender_glyph_0033, sizeof(bender_glyph_0033) / sizeof(bender_glyph_0033[0])},
	{'4', bender_glyph_0034, sizeof(bender_glyph_0034) / sizeof(bender_glyph_0034[0])},
	{'5', bender_glyph_0035, sizeof(bender_glyph_0035) / sizeof(bender_glyph_0035[0])},
	{'6', bender_glyph_0036, sizeof(bender_glyph_0036) / sizeof(bender_glyph_0036[0])},
	{'7', bender_glyph_0037, sizeof(bender_glyph_0037) / sizeof(bender_glyph_0037[0])},
	{'8', bender_glyph_0038, sizeof(bender_glyph_0038) / sizeof(bender_glyph_0038[0])},
	{'9', bender_glyph_0039, sizeof(bender_glyph_0039) / sizeof(bender_glyph_0039[0])},
	{'0', bender_glyph_0030, sizeof(bender_glyph_0030) / sizeof(bender_glyph_0030[0])},
	{'q', bender_glyph_0071, sizeof(bender_glyph_0071) / sizeof(bender_glyph_0071[0])},
	{'w', bender_glyph_0077, sizeof(bender_glyph_0077) / sizeof(bender_glyph_0077[0])},
	{'e', bender_glyph_0065, sizeof(bender_glyph_0065) / sizeof(bender_glyph_0065[0])},
	{'r', bender_glyph_0072, sizeof(bender_glyph_0072) / sizeof(bender_glyph_0072[0])},
	{'t', bender_glyph_0074, sizeof(bender_glyph_0074) / sizeof(bender_glyph_0074[0])},
	{'y', bender_glyph_0079, sizeof(bender_glyph_0079) / sizeof(bender_glyph_0079[0])},
	{'u', bender_glyph_0075, sizeof(bender_glyph_0075) / sizeof(bender_glyph_0075[0])},
	{'i', bender_glyph_0069, sizeof(bender_glyph_0069) / sizeof(bender_glyph_0069[0])},
	{'o', bender_glyph_006F, sizeof(bender_glyph_006F) / sizeof(bender_glyph_006F[0])},
	{'p', bender_glyph_0070, sizeof(bender_glyph_0070) / sizeof(bender_glyph_0070[0])},
	{'a', bender_glyph_0061, sizeof(bender_glyph_0061) / sizeof(bender_glyph_0061[0])},
	{'s', bender_glyph_0073, sizeof(bender_glyph_0073) / sizeof(bender_glyph_0073[0])},
	{'d', bender_glyph_0064, sizeof(bender_glyph_0064) / sizeof(bender_glyph_0064[0])},
	{'f', bender_glyph_0066, sizeof(bender_glyph_0066) / sizeof(bender_glyph_0066[0])},
	{'g', bender_glyph_0067, sizeof(bender_glyph_0067) / sizeof(bender_glyph_0067[0])},
	{'h', bender_glyph_0068, sizeof(bender_glyph_0068) / sizeof(bender_glyph_0068[0])},
	{'j', bender_glyph_006A, sizeof(bender_glyph_006A) / sizeof(bender_glyph_006A[0])},
	{'k', bender_glyph_006B, sizeof(bender_glyph_006B) / sizeof(bender_glyph_006B[0])},
	{'l', bender_glyph_006C, sizeof(bender_glyph_006C) / sizeof(bender_glyph_006C[0])},
	{'z', bender_glyph_007A, sizeof(bender_glyph_007A) / sizeof(bender_glyph_007A[0])},
	{'x', bender_glyph_0078, sizeof(bender_glyph_0078) / sizeof(bender_glyph_0078[0])},
	{'c', bender_glyph_0063, sizeof(bender_glyph_0063) / sizeof(bender_glyph_0063[0])},
	{'v', bender_glyph_0076, sizeof(bender_glyph_0076) / sizeof(bender_glyph_0076[0])},
	{'b', bender_glyph_0062, sizeof(bender_glyph_0062) / sizeof(bender_glyph_0062[0])},
	{'n', bender_glyph_006E, sizeof(bender_glyph_006E) / sizeof(bender_glyph_006E[0])},
	{'m', bender_glyph_006D, sizeof(bender_glyph_006D) / sizeof(bender_glyph_006D[0])},
	{'Q', bender_glyph_0051, sizeof(bender_glyph_0051) / sizeof(bender_glyph_0051[0])},
	{'W', bender_glyph_0057, sizeof(bender_glyph_0057) / sizeof(bender_glyph_0057[0])},
	{'E', bender_glyph_0045, sizeof(bender_glyph_0045) / sizeof(bender_glyph_0045[0])},
	{'R', bender_glyph_0052, sizeof(bender_glyph_0052) / sizeof(bender_glyph_0052[0])},
	{'T', bender_glyph_0054, sizeof(bender_glyph_0054) / sizeof(bender_glyph_0054[0])},
	{'Y', bender_glyph_0059, sizeof(bender_glyph_0059) / sizeof(bender_glyph_0059[0])},
	{'U', bender_glyph_0055, sizeof(bender_glyph_0055) / sizeof(bender_glyph_0055[0])},
	{'I', bender_glyph_0049, sizeof(bender_glyph_0049) / sizeof(bender_glyph_0049[0])},
	{'O', bender_glyph_004F, sizeof(bender_glyph_004F) / sizeof(bender_glyph_004F[0])},
	{'P', bender_glyph_0050, sizeof(bender_glyph_0050) / sizeof(bender_glyph_0050[0])},
	{'L', bender_glyph_004C, sizeof(bender_glyph_004C) / sizeof(bender_glyph_004C[0])},
	{'K', bender_glyph_004B, sizeof(bender_glyph_004B) / sizeof(bender_glyph_004B[0])},
	{'J', bender_glyph_004A, sizeof(bender_glyph_004A) / sizeof(bender_glyph_004A[0])},
	{'H', bender_glyph_0048, sizeof(bender_glyph_0048) / sizeof(bender_glyph_0048[0])},
	{'G', bender_glyph_0047, sizeof(bender_glyph_0047) / sizeof(bender_glyph_0047[0])},
	{'F', bender_glyph_0046, sizeof(bender_glyph_0046) / sizeof(bender_glyph_0046[0])},
	{'D', bender_glyph_0044, sizeof(bender_glyph_0044) / sizeof(bender_glyph_0044[0])},
	{'S', bender_glyph_0053, sizeof(bender_glyph_0053) / sizeof(bender_glyph_0053[0])},
	{'A', bender_glyph_0041, sizeof(bender_glyph_0041) / sizeof(bender_glyph_0041[0])},
	{'Z', bender_glyph_005A, sizeof(bender_glyph_005A) / sizeof(bender_glyph_005A[0])},
	{'X', bender_glyph_0058, sizeof(bender_glyph_0058) / sizeof(bender_glyph_0058[0])},
	{'C', bender_glyph_0043, sizeof(bender_glyph_0043) / sizeof(bender_glyph_0043[0])},
	{'V', bender_glyph_0056, sizeof(bender_glyph_0056) / sizeof(bender_glyph_0056[0])},
	{'B', bender_glyph_0042, sizeof(bender_glyph_0042) / sizeof(bender_glyph_0042[0])},
	{'N', bender_glyph_004E, sizeof(bender_glyph_004E) / sizeof(bender_glyph_004E[0])},
	{'M', bender_glyph_004D, sizeof(bender_glyph_004D) / sizeof(bender_glyph_004D[0])},
	{'.', bender_glyph_002E, sizeof(bender_glyph_002E) / sizeof(bender_glyph_002E[0])},
	{':', bender_glyph_003A, sizeof(bender_glyph_003A) / sizeof(bender_glyph_003A[0])},
	{'-', bender_glyph_002D, sizeof(bender_glyph_002D) / sizeof(bender_glyph_002D[0])},
	{'=', bender_glyph_003D, sizeof(bender_glyph_003D) / sizeof(bender_glyph_003D[0])},
	{'+', bender_glyph_002B, sizeof(bender_glyph_002B) / sizeof(bender_glyph_002B[0])},
	{'_', bender_glyph_005F, sizeof(bender_glyph_005F) / sizeof(bender_glyph_005F[0])},
	{'%', bender_glyph_0025, sizeof(bender_glyph_0025) / sizeof(bender_glyph_0025[0])},
	{'*', bender_glyph_002A, sizeof(bender_glyph_002A) / sizeof(bender_glyph_002A[0])},
	{'#', bender_glyph_0023, sizeof(bender_glyph_0023) / sizeof(bender_glyph_0023[0])},
	{'@', bender_glyph_0040, sizeof(bender_glyph_0040) / sizeof(bender_glyph_0040[0])},
	{'!', bender_glyph_0021, sizeof(bender_glyph_0021) / sizeof(bender_glyph_0021[0])},
	{'^', bender_glyph_005E, sizeof(bender_glyph_005E) / sizeof(bender_glyph_005E[0])},
	{'&', bender_glyph_0026, sizeof(bender_glyph_0026) / sizeof(bender_glyph_0026[0])},
	{'(', bender_glyph_0028, sizeof(bender_glyph_0028) / sizeof(bender_glyph_0028[0])},
	{')', bender_glyph_0029, sizeof(bender_glyph_0029) / sizeof(bender_glyph_0029[0])},
};

static inline const uint8_t *Bender_FindBitmap(char ch, LCD_FontSize size) {
	uint32_t glyph_index;
	for (glyph_index = 0U; glyph_index < (sizeof(bender_glyphs) / sizeof(bender_glyphs[0])); ++glyph_index) {
		uint32_t size_index;
		if (bender_glyphs[glyph_index].ch != ch) {
			continue;
		}
		for (size_index = 0U; size_index < bender_glyphs[glyph_index].size_count; ++size_index) {
			if (bender_glyphs[glyph_index].sizes[size_index].size == size) {
				return bender_glyphs[glyph_index].sizes[size_index].bitmap;
			}
		}
		return 0;
	}
	return 0;
}

#endif /* BENDER_INDEX_H */
