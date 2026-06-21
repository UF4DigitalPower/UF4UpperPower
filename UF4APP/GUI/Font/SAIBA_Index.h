#ifndef SAIBA_INDEX_H
#define SAIBA_INDEX_H

#include "SAIBA_Config.h"
#include "SAIBA_10x10.h"
#include "SAIBA_14x14.h"
#include "SAIBA_16x16.h"
#include "SAIBA_20x20.h"
#include "SAIBA_24x24.h"
#include "SAIBA_28x28.h"
#include "SAIBA_34x34.h"
#include "SAIBA_48x48.h"
#include "SAIBA_72x72.h"
#include "SAIBA_96x96.h"
#include "SAIBA_128x128.h"

typedef struct {
	LCD_FontSize size;
	const uint8_t *bitmap;
} SAIBA_SizeBitmap;

typedef struct {
	char ch;
	const SAIBA_SizeBitmap *sizes;
	uint16_t size_count;
} SAIBA_Glyph;

static const SAIBA_SizeBitmap saiba_glyph_0031[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0031},
	{LCD_FONT_SAIBA_14, saiba_14_char_0031},
	{LCD_FONT_SAIBA_16, saiba_16_char_0031},
	{LCD_FONT_SAIBA_20, saiba_20_char_0031},
	{LCD_FONT_SAIBA_24, saiba_24_char_0031},
	{LCD_FONT_SAIBA_28, saiba_28_char_0031},
	{LCD_FONT_SAIBA_34, saiba_34_char_0031},
	{LCD_FONT_SAIBA_48, saiba_48_char_0031},
	{LCD_FONT_SAIBA_72, saiba_72_char_0031},
	{LCD_FONT_SAIBA_96, saiba_96_char_0031},
	{LCD_FONT_SAIBA_128, saiba_128_char_0031},
};

static const SAIBA_SizeBitmap saiba_glyph_0032[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0032},
	{LCD_FONT_SAIBA_14, saiba_14_char_0032},
	{LCD_FONT_SAIBA_16, saiba_16_char_0032},
	{LCD_FONT_SAIBA_20, saiba_20_char_0032},
	{LCD_FONT_SAIBA_24, saiba_24_char_0032},
	{LCD_FONT_SAIBA_28, saiba_28_char_0032},
	{LCD_FONT_SAIBA_34, saiba_34_char_0032},
	{LCD_FONT_SAIBA_48, saiba_48_char_0032},
	{LCD_FONT_SAIBA_72, saiba_72_char_0032},
	{LCD_FONT_SAIBA_96, saiba_96_char_0032},
	{LCD_FONT_SAIBA_128, saiba_128_char_0032},
};

static const SAIBA_SizeBitmap saiba_glyph_0033[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0033},
	{LCD_FONT_SAIBA_14, saiba_14_char_0033},
	{LCD_FONT_SAIBA_16, saiba_16_char_0033},
	{LCD_FONT_SAIBA_20, saiba_20_char_0033},
	{LCD_FONT_SAIBA_24, saiba_24_char_0033},
	{LCD_FONT_SAIBA_28, saiba_28_char_0033},
	{LCD_FONT_SAIBA_34, saiba_34_char_0033},
	{LCD_FONT_SAIBA_48, saiba_48_char_0033},
	{LCD_FONT_SAIBA_72, saiba_72_char_0033},
	{LCD_FONT_SAIBA_96, saiba_96_char_0033},
	{LCD_FONT_SAIBA_128, saiba_128_char_0033},
};

static const SAIBA_SizeBitmap saiba_glyph_0034[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0034},
	{LCD_FONT_SAIBA_14, saiba_14_char_0034},
	{LCD_FONT_SAIBA_16, saiba_16_char_0034},
	{LCD_FONT_SAIBA_20, saiba_20_char_0034},
	{LCD_FONT_SAIBA_24, saiba_24_char_0034},
	{LCD_FONT_SAIBA_28, saiba_28_char_0034},
	{LCD_FONT_SAIBA_34, saiba_34_char_0034},
	{LCD_FONT_SAIBA_48, saiba_48_char_0034},
	{LCD_FONT_SAIBA_72, saiba_72_char_0034},
	{LCD_FONT_SAIBA_96, saiba_96_char_0034},
	{LCD_FONT_SAIBA_128, saiba_128_char_0034},
};

static const SAIBA_SizeBitmap saiba_glyph_0035[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0035},
	{LCD_FONT_SAIBA_14, saiba_14_char_0035},
	{LCD_FONT_SAIBA_16, saiba_16_char_0035},
	{LCD_FONT_SAIBA_20, saiba_20_char_0035},
	{LCD_FONT_SAIBA_24, saiba_24_char_0035},
	{LCD_FONT_SAIBA_28, saiba_28_char_0035},
	{LCD_FONT_SAIBA_34, saiba_34_char_0035},
	{LCD_FONT_SAIBA_48, saiba_48_char_0035},
	{LCD_FONT_SAIBA_72, saiba_72_char_0035},
	{LCD_FONT_SAIBA_96, saiba_96_char_0035},
	{LCD_FONT_SAIBA_128, saiba_128_char_0035},
};

static const SAIBA_SizeBitmap saiba_glyph_0036[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0036},
	{LCD_FONT_SAIBA_14, saiba_14_char_0036},
	{LCD_FONT_SAIBA_16, saiba_16_char_0036},
	{LCD_FONT_SAIBA_20, saiba_20_char_0036},
	{LCD_FONT_SAIBA_24, saiba_24_char_0036},
	{LCD_FONT_SAIBA_28, saiba_28_char_0036},
	{LCD_FONT_SAIBA_34, saiba_34_char_0036},
	{LCD_FONT_SAIBA_48, saiba_48_char_0036},
	{LCD_FONT_SAIBA_72, saiba_72_char_0036},
	{LCD_FONT_SAIBA_96, saiba_96_char_0036},
	{LCD_FONT_SAIBA_128, saiba_128_char_0036},
};

static const SAIBA_SizeBitmap saiba_glyph_0037[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0037},
	{LCD_FONT_SAIBA_14, saiba_14_char_0037},
	{LCD_FONT_SAIBA_16, saiba_16_char_0037},
	{LCD_FONT_SAIBA_20, saiba_20_char_0037},
	{LCD_FONT_SAIBA_24, saiba_24_char_0037},
	{LCD_FONT_SAIBA_28, saiba_28_char_0037},
	{LCD_FONT_SAIBA_34, saiba_34_char_0037},
	{LCD_FONT_SAIBA_48, saiba_48_char_0037},
	{LCD_FONT_SAIBA_72, saiba_72_char_0037},
	{LCD_FONT_SAIBA_96, saiba_96_char_0037},
	{LCD_FONT_SAIBA_128, saiba_128_char_0037},
};

static const SAIBA_SizeBitmap saiba_glyph_0038[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0038},
	{LCD_FONT_SAIBA_14, saiba_14_char_0038},
	{LCD_FONT_SAIBA_16, saiba_16_char_0038},
	{LCD_FONT_SAIBA_20, saiba_20_char_0038},
	{LCD_FONT_SAIBA_24, saiba_24_char_0038},
	{LCD_FONT_SAIBA_28, saiba_28_char_0038},
	{LCD_FONT_SAIBA_34, saiba_34_char_0038},
	{LCD_FONT_SAIBA_48, saiba_48_char_0038},
	{LCD_FONT_SAIBA_72, saiba_72_char_0038},
	{LCD_FONT_SAIBA_96, saiba_96_char_0038},
	{LCD_FONT_SAIBA_128, saiba_128_char_0038},
};

static const SAIBA_SizeBitmap saiba_glyph_0039[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0039},
	{LCD_FONT_SAIBA_14, saiba_14_char_0039},
	{LCD_FONT_SAIBA_16, saiba_16_char_0039},
	{LCD_FONT_SAIBA_20, saiba_20_char_0039},
	{LCD_FONT_SAIBA_24, saiba_24_char_0039},
	{LCD_FONT_SAIBA_28, saiba_28_char_0039},
	{LCD_FONT_SAIBA_34, saiba_34_char_0039},
	{LCD_FONT_SAIBA_48, saiba_48_char_0039},
	{LCD_FONT_SAIBA_72, saiba_72_char_0039},
	{LCD_FONT_SAIBA_96, saiba_96_char_0039},
	{LCD_FONT_SAIBA_128, saiba_128_char_0039},
};

static const SAIBA_SizeBitmap saiba_glyph_0030[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0030},
	{LCD_FONT_SAIBA_14, saiba_14_char_0030},
	{LCD_FONT_SAIBA_16, saiba_16_char_0030},
	{LCD_FONT_SAIBA_20, saiba_20_char_0030},
	{LCD_FONT_SAIBA_24, saiba_24_char_0030},
	{LCD_FONT_SAIBA_28, saiba_28_char_0030},
	{LCD_FONT_SAIBA_34, saiba_34_char_0030},
	{LCD_FONT_SAIBA_48, saiba_48_char_0030},
	{LCD_FONT_SAIBA_72, saiba_72_char_0030},
	{LCD_FONT_SAIBA_96, saiba_96_char_0030},
	{LCD_FONT_SAIBA_128, saiba_128_char_0030},
};

static const SAIBA_SizeBitmap saiba_glyph_0071[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0071},
	{LCD_FONT_SAIBA_14, saiba_14_char_0071},
	{LCD_FONT_SAIBA_16, saiba_16_char_0071},
	{LCD_FONT_SAIBA_20, saiba_20_char_0071},
	{LCD_FONT_SAIBA_24, saiba_24_char_0071},
	{LCD_FONT_SAIBA_28, saiba_28_char_0071},
	{LCD_FONT_SAIBA_34, saiba_34_char_0071},
	{LCD_FONT_SAIBA_48, saiba_48_char_0071},
	{LCD_FONT_SAIBA_72, saiba_72_char_0071},
	{LCD_FONT_SAIBA_96, saiba_96_char_0071},
	{LCD_FONT_SAIBA_128, saiba_128_char_0071},
};

static const SAIBA_SizeBitmap saiba_glyph_0077[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0077},
	{LCD_FONT_SAIBA_14, saiba_14_char_0077},
	{LCD_FONT_SAIBA_16, saiba_16_char_0077},
	{LCD_FONT_SAIBA_20, saiba_20_char_0077},
	{LCD_FONT_SAIBA_24, saiba_24_char_0077},
	{LCD_FONT_SAIBA_28, saiba_28_char_0077},
	{LCD_FONT_SAIBA_34, saiba_34_char_0077},
	{LCD_FONT_SAIBA_48, saiba_48_char_0077},
	{LCD_FONT_SAIBA_72, saiba_72_char_0077},
	{LCD_FONT_SAIBA_96, saiba_96_char_0077},
	{LCD_FONT_SAIBA_128, saiba_128_char_0077},
};

static const SAIBA_SizeBitmap saiba_glyph_0065[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0065},
	{LCD_FONT_SAIBA_14, saiba_14_char_0065},
	{LCD_FONT_SAIBA_16, saiba_16_char_0065},
	{LCD_FONT_SAIBA_20, saiba_20_char_0065},
	{LCD_FONT_SAIBA_24, saiba_24_char_0065},
	{LCD_FONT_SAIBA_28, saiba_28_char_0065},
	{LCD_FONT_SAIBA_34, saiba_34_char_0065},
	{LCD_FONT_SAIBA_48, saiba_48_char_0065},
	{LCD_FONT_SAIBA_72, saiba_72_char_0065},
	{LCD_FONT_SAIBA_96, saiba_96_char_0065},
	{LCD_FONT_SAIBA_128, saiba_128_char_0065},
};

static const SAIBA_SizeBitmap saiba_glyph_0072[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0072},
	{LCD_FONT_SAIBA_14, saiba_14_char_0072},
	{LCD_FONT_SAIBA_16, saiba_16_char_0072},
	{LCD_FONT_SAIBA_20, saiba_20_char_0072},
	{LCD_FONT_SAIBA_24, saiba_24_char_0072},
	{LCD_FONT_SAIBA_28, saiba_28_char_0072},
	{LCD_FONT_SAIBA_34, saiba_34_char_0072},
	{LCD_FONT_SAIBA_48, saiba_48_char_0072},
	{LCD_FONT_SAIBA_72, saiba_72_char_0072},
	{LCD_FONT_SAIBA_96, saiba_96_char_0072},
	{LCD_FONT_SAIBA_128, saiba_128_char_0072},
};

static const SAIBA_SizeBitmap saiba_glyph_0074[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0074},
	{LCD_FONT_SAIBA_14, saiba_14_char_0074},
	{LCD_FONT_SAIBA_16, saiba_16_char_0074},
	{LCD_FONT_SAIBA_20, saiba_20_char_0074},
	{LCD_FONT_SAIBA_24, saiba_24_char_0074},
	{LCD_FONT_SAIBA_28, saiba_28_char_0074},
	{LCD_FONT_SAIBA_34, saiba_34_char_0074},
	{LCD_FONT_SAIBA_48, saiba_48_char_0074},
	{LCD_FONT_SAIBA_72, saiba_72_char_0074},
	{LCD_FONT_SAIBA_96, saiba_96_char_0074},
	{LCD_FONT_SAIBA_128, saiba_128_char_0074},
};

static const SAIBA_SizeBitmap saiba_glyph_0079[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0079},
	{LCD_FONT_SAIBA_14, saiba_14_char_0079},
	{LCD_FONT_SAIBA_16, saiba_16_char_0079},
	{LCD_FONT_SAIBA_20, saiba_20_char_0079},
	{LCD_FONT_SAIBA_24, saiba_24_char_0079},
	{LCD_FONT_SAIBA_28, saiba_28_char_0079},
	{LCD_FONT_SAIBA_34, saiba_34_char_0079},
	{LCD_FONT_SAIBA_48, saiba_48_char_0079},
	{LCD_FONT_SAIBA_72, saiba_72_char_0079},
	{LCD_FONT_SAIBA_96, saiba_96_char_0079},
	{LCD_FONT_SAIBA_128, saiba_128_char_0079},
};

static const SAIBA_SizeBitmap saiba_glyph_0075[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0075},
	{LCD_FONT_SAIBA_14, saiba_14_char_0075},
	{LCD_FONT_SAIBA_16, saiba_16_char_0075},
	{LCD_FONT_SAIBA_20, saiba_20_char_0075},
	{LCD_FONT_SAIBA_24, saiba_24_char_0075},
	{LCD_FONT_SAIBA_28, saiba_28_char_0075},
	{LCD_FONT_SAIBA_34, saiba_34_char_0075},
	{LCD_FONT_SAIBA_48, saiba_48_char_0075},
	{LCD_FONT_SAIBA_72, saiba_72_char_0075},
	{LCD_FONT_SAIBA_96, saiba_96_char_0075},
	{LCD_FONT_SAIBA_128, saiba_128_char_0075},
};

static const SAIBA_SizeBitmap saiba_glyph_0069[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0069},
	{LCD_FONT_SAIBA_14, saiba_14_char_0069},
	{LCD_FONT_SAIBA_16, saiba_16_char_0069},
	{LCD_FONT_SAIBA_20, saiba_20_char_0069},
	{LCD_FONT_SAIBA_24, saiba_24_char_0069},
	{LCD_FONT_SAIBA_28, saiba_28_char_0069},
	{LCD_FONT_SAIBA_34, saiba_34_char_0069},
	{LCD_FONT_SAIBA_48, saiba_48_char_0069},
	{LCD_FONT_SAIBA_72, saiba_72_char_0069},
	{LCD_FONT_SAIBA_96, saiba_96_char_0069},
	{LCD_FONT_SAIBA_128, saiba_128_char_0069},
};

static const SAIBA_SizeBitmap saiba_glyph_006F[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_006F},
	{LCD_FONT_SAIBA_14, saiba_14_char_006F},
	{LCD_FONT_SAIBA_16, saiba_16_char_006F},
	{LCD_FONT_SAIBA_20, saiba_20_char_006F},
	{LCD_FONT_SAIBA_24, saiba_24_char_006F},
	{LCD_FONT_SAIBA_28, saiba_28_char_006F},
	{LCD_FONT_SAIBA_34, saiba_34_char_006F},
	{LCD_FONT_SAIBA_48, saiba_48_char_006F},
	{LCD_FONT_SAIBA_72, saiba_72_char_006F},
	{LCD_FONT_SAIBA_96, saiba_96_char_006F},
	{LCD_FONT_SAIBA_128, saiba_128_char_006F},
};

static const SAIBA_SizeBitmap saiba_glyph_0070[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0070},
	{LCD_FONT_SAIBA_14, saiba_14_char_0070},
	{LCD_FONT_SAIBA_16, saiba_16_char_0070},
	{LCD_FONT_SAIBA_20, saiba_20_char_0070},
	{LCD_FONT_SAIBA_24, saiba_24_char_0070},
	{LCD_FONT_SAIBA_28, saiba_28_char_0070},
	{LCD_FONT_SAIBA_34, saiba_34_char_0070},
	{LCD_FONT_SAIBA_48, saiba_48_char_0070},
	{LCD_FONT_SAIBA_72, saiba_72_char_0070},
	{LCD_FONT_SAIBA_96, saiba_96_char_0070},
	{LCD_FONT_SAIBA_128, saiba_128_char_0070},
};

static const SAIBA_SizeBitmap saiba_glyph_0061[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0061},
	{LCD_FONT_SAIBA_14, saiba_14_char_0061},
	{LCD_FONT_SAIBA_16, saiba_16_char_0061},
	{LCD_FONT_SAIBA_20, saiba_20_char_0061},
	{LCD_FONT_SAIBA_24, saiba_24_char_0061},
	{LCD_FONT_SAIBA_28, saiba_28_char_0061},
	{LCD_FONT_SAIBA_34, saiba_34_char_0061},
	{LCD_FONT_SAIBA_48, saiba_48_char_0061},
	{LCD_FONT_SAIBA_72, saiba_72_char_0061},
	{LCD_FONT_SAIBA_96, saiba_96_char_0061},
	{LCD_FONT_SAIBA_128, saiba_128_char_0061},
};

static const SAIBA_SizeBitmap saiba_glyph_0073[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0073},
	{LCD_FONT_SAIBA_14, saiba_14_char_0073},
	{LCD_FONT_SAIBA_16, saiba_16_char_0073},
	{LCD_FONT_SAIBA_20, saiba_20_char_0073},
	{LCD_FONT_SAIBA_24, saiba_24_char_0073},
	{LCD_FONT_SAIBA_28, saiba_28_char_0073},
	{LCD_FONT_SAIBA_34, saiba_34_char_0073},
	{LCD_FONT_SAIBA_48, saiba_48_char_0073},
	{LCD_FONT_SAIBA_72, saiba_72_char_0073},
	{LCD_FONT_SAIBA_96, saiba_96_char_0073},
	{LCD_FONT_SAIBA_128, saiba_128_char_0073},
};

static const SAIBA_SizeBitmap saiba_glyph_0064[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0064},
	{LCD_FONT_SAIBA_14, saiba_14_char_0064},
	{LCD_FONT_SAIBA_16, saiba_16_char_0064},
	{LCD_FONT_SAIBA_20, saiba_20_char_0064},
	{LCD_FONT_SAIBA_24, saiba_24_char_0064},
	{LCD_FONT_SAIBA_28, saiba_28_char_0064},
	{LCD_FONT_SAIBA_34, saiba_34_char_0064},
	{LCD_FONT_SAIBA_48, saiba_48_char_0064},
	{LCD_FONT_SAIBA_72, saiba_72_char_0064},
	{LCD_FONT_SAIBA_96, saiba_96_char_0064},
	{LCD_FONT_SAIBA_128, saiba_128_char_0064},
};

static const SAIBA_SizeBitmap saiba_glyph_0066[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0066},
	{LCD_FONT_SAIBA_14, saiba_14_char_0066},
	{LCD_FONT_SAIBA_16, saiba_16_char_0066},
	{LCD_FONT_SAIBA_20, saiba_20_char_0066},
	{LCD_FONT_SAIBA_24, saiba_24_char_0066},
	{LCD_FONT_SAIBA_28, saiba_28_char_0066},
	{LCD_FONT_SAIBA_34, saiba_34_char_0066},
	{LCD_FONT_SAIBA_48, saiba_48_char_0066},
	{LCD_FONT_SAIBA_72, saiba_72_char_0066},
	{LCD_FONT_SAIBA_96, saiba_96_char_0066},
	{LCD_FONT_SAIBA_128, saiba_128_char_0066},
};

static const SAIBA_SizeBitmap saiba_glyph_0067[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0067},
	{LCD_FONT_SAIBA_14, saiba_14_char_0067},
	{LCD_FONT_SAIBA_16, saiba_16_char_0067},
	{LCD_FONT_SAIBA_20, saiba_20_char_0067},
	{LCD_FONT_SAIBA_24, saiba_24_char_0067},
	{LCD_FONT_SAIBA_28, saiba_28_char_0067},
	{LCD_FONT_SAIBA_34, saiba_34_char_0067},
	{LCD_FONT_SAIBA_48, saiba_48_char_0067},
	{LCD_FONT_SAIBA_72, saiba_72_char_0067},
	{LCD_FONT_SAIBA_96, saiba_96_char_0067},
	{LCD_FONT_SAIBA_128, saiba_128_char_0067},
};

static const SAIBA_SizeBitmap saiba_glyph_0068[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0068},
	{LCD_FONT_SAIBA_14, saiba_14_char_0068},
	{LCD_FONT_SAIBA_16, saiba_16_char_0068},
	{LCD_FONT_SAIBA_20, saiba_20_char_0068},
	{LCD_FONT_SAIBA_24, saiba_24_char_0068},
	{LCD_FONT_SAIBA_28, saiba_28_char_0068},
	{LCD_FONT_SAIBA_34, saiba_34_char_0068},
	{LCD_FONT_SAIBA_48, saiba_48_char_0068},
	{LCD_FONT_SAIBA_72, saiba_72_char_0068},
	{LCD_FONT_SAIBA_96, saiba_96_char_0068},
	{LCD_FONT_SAIBA_128, saiba_128_char_0068},
};

static const SAIBA_SizeBitmap saiba_glyph_006A[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_006A},
	{LCD_FONT_SAIBA_14, saiba_14_char_006A},
	{LCD_FONT_SAIBA_16, saiba_16_char_006A},
	{LCD_FONT_SAIBA_20, saiba_20_char_006A},
	{LCD_FONT_SAIBA_24, saiba_24_char_006A},
	{LCD_FONT_SAIBA_28, saiba_28_char_006A},
	{LCD_FONT_SAIBA_34, saiba_34_char_006A},
	{LCD_FONT_SAIBA_48, saiba_48_char_006A},
	{LCD_FONT_SAIBA_72, saiba_72_char_006A},
	{LCD_FONT_SAIBA_96, saiba_96_char_006A},
	{LCD_FONT_SAIBA_128, saiba_128_char_006A},
};

static const SAIBA_SizeBitmap saiba_glyph_006B[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_006B},
	{LCD_FONT_SAIBA_14, saiba_14_char_006B},
	{LCD_FONT_SAIBA_16, saiba_16_char_006B},
	{LCD_FONT_SAIBA_20, saiba_20_char_006B},
	{LCD_FONT_SAIBA_24, saiba_24_char_006B},
	{LCD_FONT_SAIBA_28, saiba_28_char_006B},
	{LCD_FONT_SAIBA_34, saiba_34_char_006B},
	{LCD_FONT_SAIBA_48, saiba_48_char_006B},
	{LCD_FONT_SAIBA_72, saiba_72_char_006B},
	{LCD_FONT_SAIBA_96, saiba_96_char_006B},
	{LCD_FONT_SAIBA_128, saiba_128_char_006B},
};

static const SAIBA_SizeBitmap saiba_glyph_006C[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_006C},
	{LCD_FONT_SAIBA_14, saiba_14_char_006C},
	{LCD_FONT_SAIBA_16, saiba_16_char_006C},
	{LCD_FONT_SAIBA_20, saiba_20_char_006C},
	{LCD_FONT_SAIBA_24, saiba_24_char_006C},
	{LCD_FONT_SAIBA_28, saiba_28_char_006C},
	{LCD_FONT_SAIBA_34, saiba_34_char_006C},
	{LCD_FONT_SAIBA_48, saiba_48_char_006C},
	{LCD_FONT_SAIBA_72, saiba_72_char_006C},
	{LCD_FONT_SAIBA_96, saiba_96_char_006C},
	{LCD_FONT_SAIBA_128, saiba_128_char_006C},
};

static const SAIBA_SizeBitmap saiba_glyph_007A[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_007A},
	{LCD_FONT_SAIBA_14, saiba_14_char_007A},
	{LCD_FONT_SAIBA_16, saiba_16_char_007A},
	{LCD_FONT_SAIBA_20, saiba_20_char_007A},
	{LCD_FONT_SAIBA_24, saiba_24_char_007A},
	{LCD_FONT_SAIBA_28, saiba_28_char_007A},
	{LCD_FONT_SAIBA_34, saiba_34_char_007A},
	{LCD_FONT_SAIBA_48, saiba_48_char_007A},
	{LCD_FONT_SAIBA_72, saiba_72_char_007A},
	{LCD_FONT_SAIBA_96, saiba_96_char_007A},
	{LCD_FONT_SAIBA_128, saiba_128_char_007A},
};

static const SAIBA_SizeBitmap saiba_glyph_0078[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0078},
	{LCD_FONT_SAIBA_14, saiba_14_char_0078},
	{LCD_FONT_SAIBA_16, saiba_16_char_0078},
	{LCD_FONT_SAIBA_20, saiba_20_char_0078},
	{LCD_FONT_SAIBA_24, saiba_24_char_0078},
	{LCD_FONT_SAIBA_28, saiba_28_char_0078},
	{LCD_FONT_SAIBA_34, saiba_34_char_0078},
	{LCD_FONT_SAIBA_48, saiba_48_char_0078},
	{LCD_FONT_SAIBA_72, saiba_72_char_0078},
	{LCD_FONT_SAIBA_96, saiba_96_char_0078},
	{LCD_FONT_SAIBA_128, saiba_128_char_0078},
};

static const SAIBA_SizeBitmap saiba_glyph_0063[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0063},
	{LCD_FONT_SAIBA_14, saiba_14_char_0063},
	{LCD_FONT_SAIBA_16, saiba_16_char_0063},
	{LCD_FONT_SAIBA_20, saiba_20_char_0063},
	{LCD_FONT_SAIBA_24, saiba_24_char_0063},
	{LCD_FONT_SAIBA_28, saiba_28_char_0063},
	{LCD_FONT_SAIBA_34, saiba_34_char_0063},
	{LCD_FONT_SAIBA_48, saiba_48_char_0063},
	{LCD_FONT_SAIBA_72, saiba_72_char_0063},
	{LCD_FONT_SAIBA_96, saiba_96_char_0063},
	{LCD_FONT_SAIBA_128, saiba_128_char_0063},
};

static const SAIBA_SizeBitmap saiba_glyph_0076[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0076},
	{LCD_FONT_SAIBA_14, saiba_14_char_0076},
	{LCD_FONT_SAIBA_16, saiba_16_char_0076},
	{LCD_FONT_SAIBA_20, saiba_20_char_0076},
	{LCD_FONT_SAIBA_24, saiba_24_char_0076},
	{LCD_FONT_SAIBA_28, saiba_28_char_0076},
	{LCD_FONT_SAIBA_34, saiba_34_char_0076},
	{LCD_FONT_SAIBA_48, saiba_48_char_0076},
	{LCD_FONT_SAIBA_72, saiba_72_char_0076},
	{LCD_FONT_SAIBA_96, saiba_96_char_0076},
	{LCD_FONT_SAIBA_128, saiba_128_char_0076},
};

static const SAIBA_SizeBitmap saiba_glyph_0062[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0062},
	{LCD_FONT_SAIBA_14, saiba_14_char_0062},
	{LCD_FONT_SAIBA_16, saiba_16_char_0062},
	{LCD_FONT_SAIBA_20, saiba_20_char_0062},
	{LCD_FONT_SAIBA_24, saiba_24_char_0062},
	{LCD_FONT_SAIBA_28, saiba_28_char_0062},
	{LCD_FONT_SAIBA_34, saiba_34_char_0062},
	{LCD_FONT_SAIBA_48, saiba_48_char_0062},
	{LCD_FONT_SAIBA_72, saiba_72_char_0062},
	{LCD_FONT_SAIBA_96, saiba_96_char_0062},
	{LCD_FONT_SAIBA_128, saiba_128_char_0062},
};

static const SAIBA_SizeBitmap saiba_glyph_006E[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_006E},
	{LCD_FONT_SAIBA_14, saiba_14_char_006E},
	{LCD_FONT_SAIBA_16, saiba_16_char_006E},
	{LCD_FONT_SAIBA_20, saiba_20_char_006E},
	{LCD_FONT_SAIBA_24, saiba_24_char_006E},
	{LCD_FONT_SAIBA_28, saiba_28_char_006E},
	{LCD_FONT_SAIBA_34, saiba_34_char_006E},
	{LCD_FONT_SAIBA_48, saiba_48_char_006E},
	{LCD_FONT_SAIBA_72, saiba_72_char_006E},
	{LCD_FONT_SAIBA_96, saiba_96_char_006E},
	{LCD_FONT_SAIBA_128, saiba_128_char_006E},
};

static const SAIBA_SizeBitmap saiba_glyph_006D[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_006D},
	{LCD_FONT_SAIBA_14, saiba_14_char_006D},
	{LCD_FONT_SAIBA_16, saiba_16_char_006D},
	{LCD_FONT_SAIBA_20, saiba_20_char_006D},
	{LCD_FONT_SAIBA_24, saiba_24_char_006D},
	{LCD_FONT_SAIBA_28, saiba_28_char_006D},
	{LCD_FONT_SAIBA_34, saiba_34_char_006D},
	{LCD_FONT_SAIBA_48, saiba_48_char_006D},
	{LCD_FONT_SAIBA_72, saiba_72_char_006D},
	{LCD_FONT_SAIBA_96, saiba_96_char_006D},
	{LCD_FONT_SAIBA_128, saiba_128_char_006D},
};

static const SAIBA_SizeBitmap saiba_glyph_0051[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0051},
	{LCD_FONT_SAIBA_14, saiba_14_char_0051},
	{LCD_FONT_SAIBA_16, saiba_16_char_0051},
	{LCD_FONT_SAIBA_20, saiba_20_char_0051},
	{LCD_FONT_SAIBA_24, saiba_24_char_0051},
	{LCD_FONT_SAIBA_28, saiba_28_char_0051},
	{LCD_FONT_SAIBA_34, saiba_34_char_0051},
	{LCD_FONT_SAIBA_48, saiba_48_char_0051},
	{LCD_FONT_SAIBA_72, saiba_72_char_0051},
	{LCD_FONT_SAIBA_96, saiba_96_char_0051},
	{LCD_FONT_SAIBA_128, saiba_128_char_0051},
};

static const SAIBA_SizeBitmap saiba_glyph_0057[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0057},
	{LCD_FONT_SAIBA_14, saiba_14_char_0057},
	{LCD_FONT_SAIBA_16, saiba_16_char_0057},
	{LCD_FONT_SAIBA_20, saiba_20_char_0057},
	{LCD_FONT_SAIBA_24, saiba_24_char_0057},
	{LCD_FONT_SAIBA_28, saiba_28_char_0057},
	{LCD_FONT_SAIBA_34, saiba_34_char_0057},
	{LCD_FONT_SAIBA_48, saiba_48_char_0057},
	{LCD_FONT_SAIBA_72, saiba_72_char_0057},
	{LCD_FONT_SAIBA_96, saiba_96_char_0057},
	{LCD_FONT_SAIBA_128, saiba_128_char_0057},
};

static const SAIBA_SizeBitmap saiba_glyph_0045[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0045},
	{LCD_FONT_SAIBA_14, saiba_14_char_0045},
	{LCD_FONT_SAIBA_16, saiba_16_char_0045},
	{LCD_FONT_SAIBA_20, saiba_20_char_0045},
	{LCD_FONT_SAIBA_24, saiba_24_char_0045},
	{LCD_FONT_SAIBA_28, saiba_28_char_0045},
	{LCD_FONT_SAIBA_34, saiba_34_char_0045},
	{LCD_FONT_SAIBA_48, saiba_48_char_0045},
	{LCD_FONT_SAIBA_72, saiba_72_char_0045},
	{LCD_FONT_SAIBA_96, saiba_96_char_0045},
	{LCD_FONT_SAIBA_128, saiba_128_char_0045},
};

static const SAIBA_SizeBitmap saiba_glyph_0052[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0052},
	{LCD_FONT_SAIBA_14, saiba_14_char_0052},
	{LCD_FONT_SAIBA_16, saiba_16_char_0052},
	{LCD_FONT_SAIBA_20, saiba_20_char_0052},
	{LCD_FONT_SAIBA_24, saiba_24_char_0052},
	{LCD_FONT_SAIBA_28, saiba_28_char_0052},
	{LCD_FONT_SAIBA_34, saiba_34_char_0052},
	{LCD_FONT_SAIBA_48, saiba_48_char_0052},
	{LCD_FONT_SAIBA_72, saiba_72_char_0052},
	{LCD_FONT_SAIBA_96, saiba_96_char_0052},
	{LCD_FONT_SAIBA_128, saiba_128_char_0052},
};

static const SAIBA_SizeBitmap saiba_glyph_0054[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0054},
	{LCD_FONT_SAIBA_14, saiba_14_char_0054},
	{LCD_FONT_SAIBA_16, saiba_16_char_0054},
	{LCD_FONT_SAIBA_20, saiba_20_char_0054},
	{LCD_FONT_SAIBA_24, saiba_24_char_0054},
	{LCD_FONT_SAIBA_28, saiba_28_char_0054},
	{LCD_FONT_SAIBA_34, saiba_34_char_0054},
	{LCD_FONT_SAIBA_48, saiba_48_char_0054},
	{LCD_FONT_SAIBA_72, saiba_72_char_0054},
	{LCD_FONT_SAIBA_96, saiba_96_char_0054},
	{LCD_FONT_SAIBA_128, saiba_128_char_0054},
};

static const SAIBA_SizeBitmap saiba_glyph_0059[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0059},
	{LCD_FONT_SAIBA_14, saiba_14_char_0059},
	{LCD_FONT_SAIBA_16, saiba_16_char_0059},
	{LCD_FONT_SAIBA_20, saiba_20_char_0059},
	{LCD_FONT_SAIBA_24, saiba_24_char_0059},
	{LCD_FONT_SAIBA_28, saiba_28_char_0059},
	{LCD_FONT_SAIBA_34, saiba_34_char_0059},
	{LCD_FONT_SAIBA_48, saiba_48_char_0059},
	{LCD_FONT_SAIBA_72, saiba_72_char_0059},
	{LCD_FONT_SAIBA_96, saiba_96_char_0059},
	{LCD_FONT_SAIBA_128, saiba_128_char_0059},
};

static const SAIBA_SizeBitmap saiba_glyph_0055[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0055},
	{LCD_FONT_SAIBA_14, saiba_14_char_0055},
	{LCD_FONT_SAIBA_16, saiba_16_char_0055},
	{LCD_FONT_SAIBA_20, saiba_20_char_0055},
	{LCD_FONT_SAIBA_24, saiba_24_char_0055},
	{LCD_FONT_SAIBA_28, saiba_28_char_0055},
	{LCD_FONT_SAIBA_34, saiba_34_char_0055},
	{LCD_FONT_SAIBA_48, saiba_48_char_0055},
	{LCD_FONT_SAIBA_72, saiba_72_char_0055},
	{LCD_FONT_SAIBA_96, saiba_96_char_0055},
	{LCD_FONT_SAIBA_128, saiba_128_char_0055},
};

static const SAIBA_SizeBitmap saiba_glyph_0049[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0049},
	{LCD_FONT_SAIBA_14, saiba_14_char_0049},
	{LCD_FONT_SAIBA_16, saiba_16_char_0049},
	{LCD_FONT_SAIBA_20, saiba_20_char_0049},
	{LCD_FONT_SAIBA_24, saiba_24_char_0049},
	{LCD_FONT_SAIBA_28, saiba_28_char_0049},
	{LCD_FONT_SAIBA_34, saiba_34_char_0049},
	{LCD_FONT_SAIBA_48, saiba_48_char_0049},
	{LCD_FONT_SAIBA_72, saiba_72_char_0049},
	{LCD_FONT_SAIBA_96, saiba_96_char_0049},
	{LCD_FONT_SAIBA_128, saiba_128_char_0049},
};

static const SAIBA_SizeBitmap saiba_glyph_004F[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_004F},
	{LCD_FONT_SAIBA_14, saiba_14_char_004F},
	{LCD_FONT_SAIBA_16, saiba_16_char_004F},
	{LCD_FONT_SAIBA_20, saiba_20_char_004F},
	{LCD_FONT_SAIBA_24, saiba_24_char_004F},
	{LCD_FONT_SAIBA_28, saiba_28_char_004F},
	{LCD_FONT_SAIBA_34, saiba_34_char_004F},
	{LCD_FONT_SAIBA_48, saiba_48_char_004F},
	{LCD_FONT_SAIBA_72, saiba_72_char_004F},
	{LCD_FONT_SAIBA_96, saiba_96_char_004F},
	{LCD_FONT_SAIBA_128, saiba_128_char_004F},
};

static const SAIBA_SizeBitmap saiba_glyph_0050[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0050},
	{LCD_FONT_SAIBA_14, saiba_14_char_0050},
	{LCD_FONT_SAIBA_16, saiba_16_char_0050},
	{LCD_FONT_SAIBA_20, saiba_20_char_0050},
	{LCD_FONT_SAIBA_24, saiba_24_char_0050},
	{LCD_FONT_SAIBA_28, saiba_28_char_0050},
	{LCD_FONT_SAIBA_34, saiba_34_char_0050},
	{LCD_FONT_SAIBA_48, saiba_48_char_0050},
	{LCD_FONT_SAIBA_72, saiba_72_char_0050},
	{LCD_FONT_SAIBA_96, saiba_96_char_0050},
	{LCD_FONT_SAIBA_128, saiba_128_char_0050},
};

static const SAIBA_SizeBitmap saiba_glyph_004C[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_004C},
	{LCD_FONT_SAIBA_14, saiba_14_char_004C},
	{LCD_FONT_SAIBA_16, saiba_16_char_004C},
	{LCD_FONT_SAIBA_20, saiba_20_char_004C},
	{LCD_FONT_SAIBA_24, saiba_24_char_004C},
	{LCD_FONT_SAIBA_28, saiba_28_char_004C},
	{LCD_FONT_SAIBA_34, saiba_34_char_004C},
	{LCD_FONT_SAIBA_48, saiba_48_char_004C},
	{LCD_FONT_SAIBA_72, saiba_72_char_004C},
	{LCD_FONT_SAIBA_96, saiba_96_char_004C},
	{LCD_FONT_SAIBA_128, saiba_128_char_004C},
};

static const SAIBA_SizeBitmap saiba_glyph_004B[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_004B},
	{LCD_FONT_SAIBA_14, saiba_14_char_004B},
	{LCD_FONT_SAIBA_16, saiba_16_char_004B},
	{LCD_FONT_SAIBA_20, saiba_20_char_004B},
	{LCD_FONT_SAIBA_24, saiba_24_char_004B},
	{LCD_FONT_SAIBA_28, saiba_28_char_004B},
	{LCD_FONT_SAIBA_34, saiba_34_char_004B},
	{LCD_FONT_SAIBA_48, saiba_48_char_004B},
	{LCD_FONT_SAIBA_72, saiba_72_char_004B},
	{LCD_FONT_SAIBA_96, saiba_96_char_004B},
	{LCD_FONT_SAIBA_128, saiba_128_char_004B},
};

static const SAIBA_SizeBitmap saiba_glyph_004A[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_004A},
	{LCD_FONT_SAIBA_14, saiba_14_char_004A},
	{LCD_FONT_SAIBA_16, saiba_16_char_004A},
	{LCD_FONT_SAIBA_20, saiba_20_char_004A},
	{LCD_FONT_SAIBA_24, saiba_24_char_004A},
	{LCD_FONT_SAIBA_28, saiba_28_char_004A},
	{LCD_FONT_SAIBA_34, saiba_34_char_004A},
	{LCD_FONT_SAIBA_48, saiba_48_char_004A},
	{LCD_FONT_SAIBA_72, saiba_72_char_004A},
	{LCD_FONT_SAIBA_96, saiba_96_char_004A},
	{LCD_FONT_SAIBA_128, saiba_128_char_004A},
};

static const SAIBA_SizeBitmap saiba_glyph_0048[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0048},
	{LCD_FONT_SAIBA_14, saiba_14_char_0048},
	{LCD_FONT_SAIBA_16, saiba_16_char_0048},
	{LCD_FONT_SAIBA_20, saiba_20_char_0048},
	{LCD_FONT_SAIBA_24, saiba_24_char_0048},
	{LCD_FONT_SAIBA_28, saiba_28_char_0048},
	{LCD_FONT_SAIBA_34, saiba_34_char_0048},
	{LCD_FONT_SAIBA_48, saiba_48_char_0048},
	{LCD_FONT_SAIBA_72, saiba_72_char_0048},
	{LCD_FONT_SAIBA_96, saiba_96_char_0048},
	{LCD_FONT_SAIBA_128, saiba_128_char_0048},
};

static const SAIBA_SizeBitmap saiba_glyph_0047[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0047},
	{LCD_FONT_SAIBA_14, saiba_14_char_0047},
	{LCD_FONT_SAIBA_16, saiba_16_char_0047},
	{LCD_FONT_SAIBA_20, saiba_20_char_0047},
	{LCD_FONT_SAIBA_24, saiba_24_char_0047},
	{LCD_FONT_SAIBA_28, saiba_28_char_0047},
	{LCD_FONT_SAIBA_34, saiba_34_char_0047},
	{LCD_FONT_SAIBA_48, saiba_48_char_0047},
	{LCD_FONT_SAIBA_72, saiba_72_char_0047},
	{LCD_FONT_SAIBA_96, saiba_96_char_0047},
	{LCD_FONT_SAIBA_128, saiba_128_char_0047},
};

static const SAIBA_SizeBitmap saiba_glyph_0046[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0046},
	{LCD_FONT_SAIBA_14, saiba_14_char_0046},
	{LCD_FONT_SAIBA_16, saiba_16_char_0046},
	{LCD_FONT_SAIBA_20, saiba_20_char_0046},
	{LCD_FONT_SAIBA_24, saiba_24_char_0046},
	{LCD_FONT_SAIBA_28, saiba_28_char_0046},
	{LCD_FONT_SAIBA_34, saiba_34_char_0046},
	{LCD_FONT_SAIBA_48, saiba_48_char_0046},
	{LCD_FONT_SAIBA_72, saiba_72_char_0046},
	{LCD_FONT_SAIBA_96, saiba_96_char_0046},
	{LCD_FONT_SAIBA_128, saiba_128_char_0046},
};

static const SAIBA_SizeBitmap saiba_glyph_0044[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0044},
	{LCD_FONT_SAIBA_14, saiba_14_char_0044},
	{LCD_FONT_SAIBA_16, saiba_16_char_0044},
	{LCD_FONT_SAIBA_20, saiba_20_char_0044},
	{LCD_FONT_SAIBA_24, saiba_24_char_0044},
	{LCD_FONT_SAIBA_28, saiba_28_char_0044},
	{LCD_FONT_SAIBA_34, saiba_34_char_0044},
	{LCD_FONT_SAIBA_48, saiba_48_char_0044},
	{LCD_FONT_SAIBA_72, saiba_72_char_0044},
	{LCD_FONT_SAIBA_96, saiba_96_char_0044},
	{LCD_FONT_SAIBA_128, saiba_128_char_0044},
};

static const SAIBA_SizeBitmap saiba_glyph_0053[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0053},
	{LCD_FONT_SAIBA_14, saiba_14_char_0053},
	{LCD_FONT_SAIBA_16, saiba_16_char_0053},
	{LCD_FONT_SAIBA_20, saiba_20_char_0053},
	{LCD_FONT_SAIBA_24, saiba_24_char_0053},
	{LCD_FONT_SAIBA_28, saiba_28_char_0053},
	{LCD_FONT_SAIBA_34, saiba_34_char_0053},
	{LCD_FONT_SAIBA_48, saiba_48_char_0053},
	{LCD_FONT_SAIBA_72, saiba_72_char_0053},
	{LCD_FONT_SAIBA_96, saiba_96_char_0053},
	{LCD_FONT_SAIBA_128, saiba_128_char_0053},
};

static const SAIBA_SizeBitmap saiba_glyph_0041[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0041},
	{LCD_FONT_SAIBA_14, saiba_14_char_0041},
	{LCD_FONT_SAIBA_16, saiba_16_char_0041},
	{LCD_FONT_SAIBA_20, saiba_20_char_0041},
	{LCD_FONT_SAIBA_24, saiba_24_char_0041},
	{LCD_FONT_SAIBA_28, saiba_28_char_0041},
	{LCD_FONT_SAIBA_34, saiba_34_char_0041},
	{LCD_FONT_SAIBA_48, saiba_48_char_0041},
	{LCD_FONT_SAIBA_72, saiba_72_char_0041},
	{LCD_FONT_SAIBA_96, saiba_96_char_0041},
	{LCD_FONT_SAIBA_128, saiba_128_char_0041},
};

static const SAIBA_SizeBitmap saiba_glyph_005A[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_005A},
	{LCD_FONT_SAIBA_14, saiba_14_char_005A},
	{LCD_FONT_SAIBA_16, saiba_16_char_005A},
	{LCD_FONT_SAIBA_20, saiba_20_char_005A},
	{LCD_FONT_SAIBA_24, saiba_24_char_005A},
	{LCD_FONT_SAIBA_28, saiba_28_char_005A},
	{LCD_FONT_SAIBA_34, saiba_34_char_005A},
	{LCD_FONT_SAIBA_48, saiba_48_char_005A},
	{LCD_FONT_SAIBA_72, saiba_72_char_005A},
	{LCD_FONT_SAIBA_96, saiba_96_char_005A},
	{LCD_FONT_SAIBA_128, saiba_128_char_005A},
};

static const SAIBA_SizeBitmap saiba_glyph_0058[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0058},
	{LCD_FONT_SAIBA_14, saiba_14_char_0058},
	{LCD_FONT_SAIBA_16, saiba_16_char_0058},
	{LCD_FONT_SAIBA_20, saiba_20_char_0058},
	{LCD_FONT_SAIBA_24, saiba_24_char_0058},
	{LCD_FONT_SAIBA_28, saiba_28_char_0058},
	{LCD_FONT_SAIBA_34, saiba_34_char_0058},
	{LCD_FONT_SAIBA_48, saiba_48_char_0058},
	{LCD_FONT_SAIBA_72, saiba_72_char_0058},
	{LCD_FONT_SAIBA_96, saiba_96_char_0058},
	{LCD_FONT_SAIBA_128, saiba_128_char_0058},
};

static const SAIBA_SizeBitmap saiba_glyph_0043[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0043},
	{LCD_FONT_SAIBA_14, saiba_14_char_0043},
	{LCD_FONT_SAIBA_16, saiba_16_char_0043},
	{LCD_FONT_SAIBA_20, saiba_20_char_0043},
	{LCD_FONT_SAIBA_24, saiba_24_char_0043},
	{LCD_FONT_SAIBA_28, saiba_28_char_0043},
	{LCD_FONT_SAIBA_34, saiba_34_char_0043},
	{LCD_FONT_SAIBA_48, saiba_48_char_0043},
	{LCD_FONT_SAIBA_72, saiba_72_char_0043},
	{LCD_FONT_SAIBA_96, saiba_96_char_0043},
	{LCD_FONT_SAIBA_128, saiba_128_char_0043},
};

static const SAIBA_SizeBitmap saiba_glyph_0056[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0056},
	{LCD_FONT_SAIBA_14, saiba_14_char_0056},
	{LCD_FONT_SAIBA_16, saiba_16_char_0056},
	{LCD_FONT_SAIBA_20, saiba_20_char_0056},
	{LCD_FONT_SAIBA_24, saiba_24_char_0056},
	{LCD_FONT_SAIBA_28, saiba_28_char_0056},
	{LCD_FONT_SAIBA_34, saiba_34_char_0056},
	{LCD_FONT_SAIBA_48, saiba_48_char_0056},
	{LCD_FONT_SAIBA_72, saiba_72_char_0056},
	{LCD_FONT_SAIBA_96, saiba_96_char_0056},
	{LCD_FONT_SAIBA_128, saiba_128_char_0056},
};

static const SAIBA_SizeBitmap saiba_glyph_0042[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0042},
	{LCD_FONT_SAIBA_14, saiba_14_char_0042},
	{LCD_FONT_SAIBA_16, saiba_16_char_0042},
	{LCD_FONT_SAIBA_20, saiba_20_char_0042},
	{LCD_FONT_SAIBA_24, saiba_24_char_0042},
	{LCD_FONT_SAIBA_28, saiba_28_char_0042},
	{LCD_FONT_SAIBA_34, saiba_34_char_0042},
	{LCD_FONT_SAIBA_48, saiba_48_char_0042},
	{LCD_FONT_SAIBA_72, saiba_72_char_0042},
	{LCD_FONT_SAIBA_96, saiba_96_char_0042},
	{LCD_FONT_SAIBA_128, saiba_128_char_0042},
};

static const SAIBA_SizeBitmap saiba_glyph_004E[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_004E},
	{LCD_FONT_SAIBA_14, saiba_14_char_004E},
	{LCD_FONT_SAIBA_16, saiba_16_char_004E},
	{LCD_FONT_SAIBA_20, saiba_20_char_004E},
	{LCD_FONT_SAIBA_24, saiba_24_char_004E},
	{LCD_FONT_SAIBA_28, saiba_28_char_004E},
	{LCD_FONT_SAIBA_34, saiba_34_char_004E},
	{LCD_FONT_SAIBA_48, saiba_48_char_004E},
	{LCD_FONT_SAIBA_72, saiba_72_char_004E},
	{LCD_FONT_SAIBA_96, saiba_96_char_004E},
	{LCD_FONT_SAIBA_128, saiba_128_char_004E},
};

static const SAIBA_SizeBitmap saiba_glyph_004D[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_004D},
	{LCD_FONT_SAIBA_14, saiba_14_char_004D},
	{LCD_FONT_SAIBA_16, saiba_16_char_004D},
	{LCD_FONT_SAIBA_20, saiba_20_char_004D},
	{LCD_FONT_SAIBA_24, saiba_24_char_004D},
	{LCD_FONT_SAIBA_28, saiba_28_char_004D},
	{LCD_FONT_SAIBA_34, saiba_34_char_004D},
	{LCD_FONT_SAIBA_48, saiba_48_char_004D},
	{LCD_FONT_SAIBA_72, saiba_72_char_004D},
	{LCD_FONT_SAIBA_96, saiba_96_char_004D},
	{LCD_FONT_SAIBA_128, saiba_128_char_004D},
};

static const SAIBA_SizeBitmap saiba_glyph_002E[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_002E},
	{LCD_FONT_SAIBA_14, saiba_14_char_002E},
	{LCD_FONT_SAIBA_16, saiba_16_char_002E},
	{LCD_FONT_SAIBA_20, saiba_20_char_002E},
	{LCD_FONT_SAIBA_24, saiba_24_char_002E},
	{LCD_FONT_SAIBA_28, saiba_28_char_002E},
	{LCD_FONT_SAIBA_34, saiba_34_char_002E},
	{LCD_FONT_SAIBA_48, saiba_48_char_002E},
	{LCD_FONT_SAIBA_72, saiba_72_char_002E},
	{LCD_FONT_SAIBA_96, saiba_96_char_002E},
	{LCD_FONT_SAIBA_128, saiba_128_char_002E},
};

static const SAIBA_SizeBitmap saiba_glyph_003A[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_003A},
	{LCD_FONT_SAIBA_14, saiba_14_char_003A},
	{LCD_FONT_SAIBA_16, saiba_16_char_003A},
	{LCD_FONT_SAIBA_20, saiba_20_char_003A},
	{LCD_FONT_SAIBA_24, saiba_24_char_003A},
	{LCD_FONT_SAIBA_28, saiba_28_char_003A},
	{LCD_FONT_SAIBA_34, saiba_34_char_003A},
	{LCD_FONT_SAIBA_48, saiba_48_char_003A},
	{LCD_FONT_SAIBA_72, saiba_72_char_003A},
	{LCD_FONT_SAIBA_96, saiba_96_char_003A},
	{LCD_FONT_SAIBA_128, saiba_128_char_003A},
};

static const SAIBA_SizeBitmap saiba_glyph_002D[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_002D},
	{LCD_FONT_SAIBA_14, saiba_14_char_002D},
	{LCD_FONT_SAIBA_16, saiba_16_char_002D},
	{LCD_FONT_SAIBA_20, saiba_20_char_002D},
	{LCD_FONT_SAIBA_24, saiba_24_char_002D},
	{LCD_FONT_SAIBA_28, saiba_28_char_002D},
	{LCD_FONT_SAIBA_34, saiba_34_char_002D},
	{LCD_FONT_SAIBA_48, saiba_48_char_002D},
	{LCD_FONT_SAIBA_72, saiba_72_char_002D},
	{LCD_FONT_SAIBA_96, saiba_96_char_002D},
	{LCD_FONT_SAIBA_128, saiba_128_char_002D},
};

static const SAIBA_SizeBitmap saiba_glyph_003D[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_003D},
	{LCD_FONT_SAIBA_14, saiba_14_char_003D},
	{LCD_FONT_SAIBA_16, saiba_16_char_003D},
	{LCD_FONT_SAIBA_20, saiba_20_char_003D},
	{LCD_FONT_SAIBA_24, saiba_24_char_003D},
	{LCD_FONT_SAIBA_28, saiba_28_char_003D},
	{LCD_FONT_SAIBA_34, saiba_34_char_003D},
	{LCD_FONT_SAIBA_48, saiba_48_char_003D},
	{LCD_FONT_SAIBA_72, saiba_72_char_003D},
	{LCD_FONT_SAIBA_96, saiba_96_char_003D},
	{LCD_FONT_SAIBA_128, saiba_128_char_003D},
};

static const SAIBA_SizeBitmap saiba_glyph_002B[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_002B},
	{LCD_FONT_SAIBA_14, saiba_14_char_002B},
	{LCD_FONT_SAIBA_16, saiba_16_char_002B},
	{LCD_FONT_SAIBA_20, saiba_20_char_002B},
	{LCD_FONT_SAIBA_24, saiba_24_char_002B},
	{LCD_FONT_SAIBA_28, saiba_28_char_002B},
	{LCD_FONT_SAIBA_34, saiba_34_char_002B},
	{LCD_FONT_SAIBA_48, saiba_48_char_002B},
	{LCD_FONT_SAIBA_72, saiba_72_char_002B},
	{LCD_FONT_SAIBA_96, saiba_96_char_002B},
	{LCD_FONT_SAIBA_128, saiba_128_char_002B},
};

static const SAIBA_SizeBitmap saiba_glyph_005F[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_005F},
	{LCD_FONT_SAIBA_14, saiba_14_char_005F},
	{LCD_FONT_SAIBA_16, saiba_16_char_005F},
	{LCD_FONT_SAIBA_20, saiba_20_char_005F},
	{LCD_FONT_SAIBA_24, saiba_24_char_005F},
	{LCD_FONT_SAIBA_28, saiba_28_char_005F},
	{LCD_FONT_SAIBA_34, saiba_34_char_005F},
	{LCD_FONT_SAIBA_48, saiba_48_char_005F},
	{LCD_FONT_SAIBA_72, saiba_72_char_005F},
	{LCD_FONT_SAIBA_96, saiba_96_char_005F},
	{LCD_FONT_SAIBA_128, saiba_128_char_005F},
};

static const SAIBA_SizeBitmap saiba_glyph_0025[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0025},
	{LCD_FONT_SAIBA_14, saiba_14_char_0025},
	{LCD_FONT_SAIBA_16, saiba_16_char_0025},
	{LCD_FONT_SAIBA_20, saiba_20_char_0025},
	{LCD_FONT_SAIBA_24, saiba_24_char_0025},
	{LCD_FONT_SAIBA_28, saiba_28_char_0025},
	{LCD_FONT_SAIBA_34, saiba_34_char_0025},
	{LCD_FONT_SAIBA_48, saiba_48_char_0025},
	{LCD_FONT_SAIBA_72, saiba_72_char_0025},
	{LCD_FONT_SAIBA_96, saiba_96_char_0025},
	{LCD_FONT_SAIBA_128, saiba_128_char_0025},
};

static const SAIBA_SizeBitmap saiba_glyph_002A[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_002A},
	{LCD_FONT_SAIBA_14, saiba_14_char_002A},
	{LCD_FONT_SAIBA_16, saiba_16_char_002A},
	{LCD_FONT_SAIBA_20, saiba_20_char_002A},
	{LCD_FONT_SAIBA_24, saiba_24_char_002A},
	{LCD_FONT_SAIBA_28, saiba_28_char_002A},
	{LCD_FONT_SAIBA_34, saiba_34_char_002A},
	{LCD_FONT_SAIBA_48, saiba_48_char_002A},
	{LCD_FONT_SAIBA_72, saiba_72_char_002A},
	{LCD_FONT_SAIBA_96, saiba_96_char_002A},
	{LCD_FONT_SAIBA_128, saiba_128_char_002A},
};

static const SAIBA_SizeBitmap saiba_glyph_0023[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0023},
	{LCD_FONT_SAIBA_14, saiba_14_char_0023},
	{LCD_FONT_SAIBA_16, saiba_16_char_0023},
	{LCD_FONT_SAIBA_20, saiba_20_char_0023},
	{LCD_FONT_SAIBA_24, saiba_24_char_0023},
	{LCD_FONT_SAIBA_28, saiba_28_char_0023},
	{LCD_FONT_SAIBA_34, saiba_34_char_0023},
	{LCD_FONT_SAIBA_48, saiba_48_char_0023},
	{LCD_FONT_SAIBA_72, saiba_72_char_0023},
	{LCD_FONT_SAIBA_96, saiba_96_char_0023},
	{LCD_FONT_SAIBA_128, saiba_128_char_0023},
};

static const SAIBA_SizeBitmap saiba_glyph_0040[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0040},
	{LCD_FONT_SAIBA_14, saiba_14_char_0040},
	{LCD_FONT_SAIBA_16, saiba_16_char_0040},
	{LCD_FONT_SAIBA_20, saiba_20_char_0040},
	{LCD_FONT_SAIBA_24, saiba_24_char_0040},
	{LCD_FONT_SAIBA_28, saiba_28_char_0040},
	{LCD_FONT_SAIBA_34, saiba_34_char_0040},
	{LCD_FONT_SAIBA_48, saiba_48_char_0040},
	{LCD_FONT_SAIBA_72, saiba_72_char_0040},
	{LCD_FONT_SAIBA_96, saiba_96_char_0040},
	{LCD_FONT_SAIBA_128, saiba_128_char_0040},
};

static const SAIBA_SizeBitmap saiba_glyph_0021[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0021},
	{LCD_FONT_SAIBA_14, saiba_14_char_0021},
	{LCD_FONT_SAIBA_16, saiba_16_char_0021},
	{LCD_FONT_SAIBA_20, saiba_20_char_0021},
	{LCD_FONT_SAIBA_24, saiba_24_char_0021},
	{LCD_FONT_SAIBA_28, saiba_28_char_0021},
	{LCD_FONT_SAIBA_34, saiba_34_char_0021},
	{LCD_FONT_SAIBA_48, saiba_48_char_0021},
	{LCD_FONT_SAIBA_72, saiba_72_char_0021},
	{LCD_FONT_SAIBA_96, saiba_96_char_0021},
	{LCD_FONT_SAIBA_128, saiba_128_char_0021},
};

static const SAIBA_SizeBitmap saiba_glyph_005E[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_005E},
	{LCD_FONT_SAIBA_14, saiba_14_char_005E},
	{LCD_FONT_SAIBA_16, saiba_16_char_005E},
	{LCD_FONT_SAIBA_20, saiba_20_char_005E},
	{LCD_FONT_SAIBA_24, saiba_24_char_005E},
	{LCD_FONT_SAIBA_28, saiba_28_char_005E},
	{LCD_FONT_SAIBA_34, saiba_34_char_005E},
	{LCD_FONT_SAIBA_48, saiba_48_char_005E},
	{LCD_FONT_SAIBA_72, saiba_72_char_005E},
	{LCD_FONT_SAIBA_96, saiba_96_char_005E},
	{LCD_FONT_SAIBA_128, saiba_128_char_005E},
};

static const SAIBA_SizeBitmap saiba_glyph_0026[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0026},
	{LCD_FONT_SAIBA_14, saiba_14_char_0026},
	{LCD_FONT_SAIBA_16, saiba_16_char_0026},
	{LCD_FONT_SAIBA_20, saiba_20_char_0026},
	{LCD_FONT_SAIBA_24, saiba_24_char_0026},
	{LCD_FONT_SAIBA_28, saiba_28_char_0026},
	{LCD_FONT_SAIBA_34, saiba_34_char_0026},
	{LCD_FONT_SAIBA_48, saiba_48_char_0026},
	{LCD_FONT_SAIBA_72, saiba_72_char_0026},
	{LCD_FONT_SAIBA_96, saiba_96_char_0026},
	{LCD_FONT_SAIBA_128, saiba_128_char_0026},
};

static const SAIBA_SizeBitmap saiba_glyph_0028[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0028},
	{LCD_FONT_SAIBA_14, saiba_14_char_0028},
	{LCD_FONT_SAIBA_16, saiba_16_char_0028},
	{LCD_FONT_SAIBA_20, saiba_20_char_0028},
	{LCD_FONT_SAIBA_24, saiba_24_char_0028},
	{LCD_FONT_SAIBA_28, saiba_28_char_0028},
	{LCD_FONT_SAIBA_34, saiba_34_char_0028},
	{LCD_FONT_SAIBA_48, saiba_48_char_0028},
	{LCD_FONT_SAIBA_72, saiba_72_char_0028},
	{LCD_FONT_SAIBA_96, saiba_96_char_0028},
	{LCD_FONT_SAIBA_128, saiba_128_char_0028},
};

static const SAIBA_SizeBitmap saiba_glyph_0029[] = {
	{LCD_FONT_SAIBA_10, saiba_10_char_0029},
	{LCD_FONT_SAIBA_14, saiba_14_char_0029},
	{LCD_FONT_SAIBA_16, saiba_16_char_0029},
	{LCD_FONT_SAIBA_20, saiba_20_char_0029},
	{LCD_FONT_SAIBA_24, saiba_24_char_0029},
	{LCD_FONT_SAIBA_28, saiba_28_char_0029},
	{LCD_FONT_SAIBA_34, saiba_34_char_0029},
	{LCD_FONT_SAIBA_48, saiba_48_char_0029},
	{LCD_FONT_SAIBA_72, saiba_72_char_0029},
	{LCD_FONT_SAIBA_96, saiba_96_char_0029},
	{LCD_FONT_SAIBA_128, saiba_128_char_0029},
};

static const SAIBA_Glyph saiba_glyphs[] = {
	{'1', saiba_glyph_0031, sizeof(saiba_glyph_0031) / sizeof(saiba_glyph_0031[0])},
	{'2', saiba_glyph_0032, sizeof(saiba_glyph_0032) / sizeof(saiba_glyph_0032[0])},
	{'3', saiba_glyph_0033, sizeof(saiba_glyph_0033) / sizeof(saiba_glyph_0033[0])},
	{'4', saiba_glyph_0034, sizeof(saiba_glyph_0034) / sizeof(saiba_glyph_0034[0])},
	{'5', saiba_glyph_0035, sizeof(saiba_glyph_0035) / sizeof(saiba_glyph_0035[0])},
	{'6', saiba_glyph_0036, sizeof(saiba_glyph_0036) / sizeof(saiba_glyph_0036[0])},
	{'7', saiba_glyph_0037, sizeof(saiba_glyph_0037) / sizeof(saiba_glyph_0037[0])},
	{'8', saiba_glyph_0038, sizeof(saiba_glyph_0038) / sizeof(saiba_glyph_0038[0])},
	{'9', saiba_glyph_0039, sizeof(saiba_glyph_0039) / sizeof(saiba_glyph_0039[0])},
	{'0', saiba_glyph_0030, sizeof(saiba_glyph_0030) / sizeof(saiba_glyph_0030[0])},
	{'q', saiba_glyph_0071, sizeof(saiba_glyph_0071) / sizeof(saiba_glyph_0071[0])},
	{'w', saiba_glyph_0077, sizeof(saiba_glyph_0077) / sizeof(saiba_glyph_0077[0])},
	{'e', saiba_glyph_0065, sizeof(saiba_glyph_0065) / sizeof(saiba_glyph_0065[0])},
	{'r', saiba_glyph_0072, sizeof(saiba_glyph_0072) / sizeof(saiba_glyph_0072[0])},
	{'t', saiba_glyph_0074, sizeof(saiba_glyph_0074) / sizeof(saiba_glyph_0074[0])},
	{'y', saiba_glyph_0079, sizeof(saiba_glyph_0079) / sizeof(saiba_glyph_0079[0])},
	{'u', saiba_glyph_0075, sizeof(saiba_glyph_0075) / sizeof(saiba_glyph_0075[0])},
	{'i', saiba_glyph_0069, sizeof(saiba_glyph_0069) / sizeof(saiba_glyph_0069[0])},
	{'o', saiba_glyph_006F, sizeof(saiba_glyph_006F) / sizeof(saiba_glyph_006F[0])},
	{'p', saiba_glyph_0070, sizeof(saiba_glyph_0070) / sizeof(saiba_glyph_0070[0])},
	{'a', saiba_glyph_0061, sizeof(saiba_glyph_0061) / sizeof(saiba_glyph_0061[0])},
	{'s', saiba_glyph_0073, sizeof(saiba_glyph_0073) / sizeof(saiba_glyph_0073[0])},
	{'d', saiba_glyph_0064, sizeof(saiba_glyph_0064) / sizeof(saiba_glyph_0064[0])},
	{'f', saiba_glyph_0066, sizeof(saiba_glyph_0066) / sizeof(saiba_glyph_0066[0])},
	{'g', saiba_glyph_0067, sizeof(saiba_glyph_0067) / sizeof(saiba_glyph_0067[0])},
	{'h', saiba_glyph_0068, sizeof(saiba_glyph_0068) / sizeof(saiba_glyph_0068[0])},
	{'j', saiba_glyph_006A, sizeof(saiba_glyph_006A) / sizeof(saiba_glyph_006A[0])},
	{'k', saiba_glyph_006B, sizeof(saiba_glyph_006B) / sizeof(saiba_glyph_006B[0])},
	{'l', saiba_glyph_006C, sizeof(saiba_glyph_006C) / sizeof(saiba_glyph_006C[0])},
	{'z', saiba_glyph_007A, sizeof(saiba_glyph_007A) / sizeof(saiba_glyph_007A[0])},
	{'x', saiba_glyph_0078, sizeof(saiba_glyph_0078) / sizeof(saiba_glyph_0078[0])},
	{'c', saiba_glyph_0063, sizeof(saiba_glyph_0063) / sizeof(saiba_glyph_0063[0])},
	{'v', saiba_glyph_0076, sizeof(saiba_glyph_0076) / sizeof(saiba_glyph_0076[0])},
	{'b', saiba_glyph_0062, sizeof(saiba_glyph_0062) / sizeof(saiba_glyph_0062[0])},
	{'n', saiba_glyph_006E, sizeof(saiba_glyph_006E) / sizeof(saiba_glyph_006E[0])},
	{'m', saiba_glyph_006D, sizeof(saiba_glyph_006D) / sizeof(saiba_glyph_006D[0])},
	{'Q', saiba_glyph_0051, sizeof(saiba_glyph_0051) / sizeof(saiba_glyph_0051[0])},
	{'W', saiba_glyph_0057, sizeof(saiba_glyph_0057) / sizeof(saiba_glyph_0057[0])},
	{'E', saiba_glyph_0045, sizeof(saiba_glyph_0045) / sizeof(saiba_glyph_0045[0])},
	{'R', saiba_glyph_0052, sizeof(saiba_glyph_0052) / sizeof(saiba_glyph_0052[0])},
	{'T', saiba_glyph_0054, sizeof(saiba_glyph_0054) / sizeof(saiba_glyph_0054[0])},
	{'Y', saiba_glyph_0059, sizeof(saiba_glyph_0059) / sizeof(saiba_glyph_0059[0])},
	{'U', saiba_glyph_0055, sizeof(saiba_glyph_0055) / sizeof(saiba_glyph_0055[0])},
	{'I', saiba_glyph_0049, sizeof(saiba_glyph_0049) / sizeof(saiba_glyph_0049[0])},
	{'O', saiba_glyph_004F, sizeof(saiba_glyph_004F) / sizeof(saiba_glyph_004F[0])},
	{'P', saiba_glyph_0050, sizeof(saiba_glyph_0050) / sizeof(saiba_glyph_0050[0])},
	{'L', saiba_glyph_004C, sizeof(saiba_glyph_004C) / sizeof(saiba_glyph_004C[0])},
	{'K', saiba_glyph_004B, sizeof(saiba_glyph_004B) / sizeof(saiba_glyph_004B[0])},
	{'J', saiba_glyph_004A, sizeof(saiba_glyph_004A) / sizeof(saiba_glyph_004A[0])},
	{'H', saiba_glyph_0048, sizeof(saiba_glyph_0048) / sizeof(saiba_glyph_0048[0])},
	{'G', saiba_glyph_0047, sizeof(saiba_glyph_0047) / sizeof(saiba_glyph_0047[0])},
	{'F', saiba_glyph_0046, sizeof(saiba_glyph_0046) / sizeof(saiba_glyph_0046[0])},
	{'D', saiba_glyph_0044, sizeof(saiba_glyph_0044) / sizeof(saiba_glyph_0044[0])},
	{'S', saiba_glyph_0053, sizeof(saiba_glyph_0053) / sizeof(saiba_glyph_0053[0])},
	{'A', saiba_glyph_0041, sizeof(saiba_glyph_0041) / sizeof(saiba_glyph_0041[0])},
	{'Z', saiba_glyph_005A, sizeof(saiba_glyph_005A) / sizeof(saiba_glyph_005A[0])},
	{'X', saiba_glyph_0058, sizeof(saiba_glyph_0058) / sizeof(saiba_glyph_0058[0])},
	{'C', saiba_glyph_0043, sizeof(saiba_glyph_0043) / sizeof(saiba_glyph_0043[0])},
	{'V', saiba_glyph_0056, sizeof(saiba_glyph_0056) / sizeof(saiba_glyph_0056[0])},
	{'B', saiba_glyph_0042, sizeof(saiba_glyph_0042) / sizeof(saiba_glyph_0042[0])},
	{'N', saiba_glyph_004E, sizeof(saiba_glyph_004E) / sizeof(saiba_glyph_004E[0])},
	{'M', saiba_glyph_004D, sizeof(saiba_glyph_004D) / sizeof(saiba_glyph_004D[0])},
	{'.', saiba_glyph_002E, sizeof(saiba_glyph_002E) / sizeof(saiba_glyph_002E[0])},
	{':', saiba_glyph_003A, sizeof(saiba_glyph_003A) / sizeof(saiba_glyph_003A[0])},
	{'-', saiba_glyph_002D, sizeof(saiba_glyph_002D) / sizeof(saiba_glyph_002D[0])},
	{'=', saiba_glyph_003D, sizeof(saiba_glyph_003D) / sizeof(saiba_glyph_003D[0])},
	{'+', saiba_glyph_002B, sizeof(saiba_glyph_002B) / sizeof(saiba_glyph_002B[0])},
	{'_', saiba_glyph_005F, sizeof(saiba_glyph_005F) / sizeof(saiba_glyph_005F[0])},
	{'%', saiba_glyph_0025, sizeof(saiba_glyph_0025) / sizeof(saiba_glyph_0025[0])},
	{'*', saiba_glyph_002A, sizeof(saiba_glyph_002A) / sizeof(saiba_glyph_002A[0])},
	{'#', saiba_glyph_0023, sizeof(saiba_glyph_0023) / sizeof(saiba_glyph_0023[0])},
	{'@', saiba_glyph_0040, sizeof(saiba_glyph_0040) / sizeof(saiba_glyph_0040[0])},
	{'!', saiba_glyph_0021, sizeof(saiba_glyph_0021) / sizeof(saiba_glyph_0021[0])},
	{'^', saiba_glyph_005E, sizeof(saiba_glyph_005E) / sizeof(saiba_glyph_005E[0])},
	{'&', saiba_glyph_0026, sizeof(saiba_glyph_0026) / sizeof(saiba_glyph_0026[0])},
	{'(', saiba_glyph_0028, sizeof(saiba_glyph_0028) / sizeof(saiba_glyph_0028[0])},
	{')', saiba_glyph_0029, sizeof(saiba_glyph_0029) / sizeof(saiba_glyph_0029[0])},
};

static inline const uint8_t *SAIBA_FindBitmap(char ch, LCD_FontSize size) {
	uint32_t glyph_index;
	for (glyph_index = 0U; glyph_index < (sizeof(saiba_glyphs) / sizeof(saiba_glyphs[0])); ++glyph_index) {
		uint32_t size_index;
		if (saiba_glyphs[glyph_index].ch != ch) {
			continue;
		}
		for (size_index = 0U; size_index < saiba_glyphs[glyph_index].size_count; ++size_index) {
			if (saiba_glyphs[glyph_index].sizes[size_index].size == size) {
				return saiba_glyphs[glyph_index].sizes[size_index].bitmap;
			}
		}
		return 0;
	}
	return 0;
}

#endif /* SAIBA_INDEX_H */
