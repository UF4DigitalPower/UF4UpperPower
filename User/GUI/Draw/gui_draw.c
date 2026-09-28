/**   ******************************************************************************   * @file    gui_draw.c   * @brief   UF4 GUI module.   ******************************************************************************   */
#include "gui_internal.h"

uint8_t pt_in(uint16_t x, uint16_t y, uint16_t rx, uint16_t ry, uint16_t rw, uint16_t rh)
{
  return (uint8_t)(x >= rx && x < (uint16_t)(rx + rw) && y >= ry && y < (uint16_t)(ry + rh));
}

void fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
  LCD_FillRect(x, y, w, h, color);
}

void border(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
  if (w < 4U || h < 4U)
  {
    return;
  }
  fill(x, y, w, UI_BORDER, color);
  fill(x, (uint16_t)(y + h - UI_BORDER), w, UI_BORDER, color);
  fill(x, y, UI_BORDER, h, color);
  fill((uint16_t)(x + w - UI_BORDER), y, UI_BORDER, h, color);
}

static uint16_t font_size(uint8_t style)
{
  if (style <= 1U) { return 18U; }
  if (style == 2U) { return 24U; }
  if (style == 3U) { return 34U; }
  return 72U;
}
/* ---- Font glyph cache in SDRAM (avoid Flash reads while rendering) ---- */
#define GUI_FONT_CACHE_IN_SDRAM 0U

#if GUI_FONT_CACHE_IN_SDRAM
#define FONT_SDRAM_BASE       0xD03E0000UL

static const char FONT_CHARSET[] = "%-./0123456789:ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const uint16_t FONT_SIZES[] = { 18U, 24U, 34U, 72U };
#define FONT_SIZE_COUNT  (sizeof(FONT_SIZES) / sizeof(FONT_SIZES[0]))
#define FONT_CHAR_COUNT  (sizeof(FONT_CHARSET) - 1U)

static LCD_FontGlyph *g_font_glyph_table;
static uint8_t g_font_ch_index[128];
#endif

#if GUI_FONT_CACHE_IN_SDRAM
static uint8_t font_size_index(uint16_t size)
{
  uint8_t i;
  for (i = 0U; i < FONT_SIZE_COUNT; ++i)
  {
    if (FONT_SIZES[i] == size)
    {
      return i;
    }
  }
  return 0xFFU;
}
#endif

void gui_draw_init(void)
{
#if GUI_FONT_CACHE_IN_SDRAM
  uint32_t bitmap_bytes = 0U;
  uint8_t *bitmap_cursor;
  LCD_FontGlyph *table;
  uint16_t gi = 0U;
  uint8_t si;
  uint8_t ci;

  for (ci = 0U; ci < 128U; ++ci)
  {
    g_font_ch_index[ci] = 0xFFU;
  }
  for (ci = 0U; ci < FONT_CHAR_COUNT; ++ci)
  {
    g_font_ch_index[(uint8_t)FONT_CHARSET[ci]] = ci;
  }

  for (si = 0U; si < FONT_SIZE_COUNT; ++si)
  {
    uint16_t bpr = (uint16_t)((FONT_SIZES[si] + 7U) / 8U);
    for (ci = 0U; ci < FONT_CHAR_COUNT; ++ci)
    {
      const LCD_FontGlyph *g = Teko_SemiBold_FindGlyph(FONT_CHARSET[ci], FONT_SIZES[si]);
      if (g != 0)
      {
        bitmap_bytes += (uint32_t)bpr * FONT_SIZES[si];
      }
    }
  }

  bitmap_cursor = (uint8_t *)FONT_SDRAM_BASE;
  table = (LCD_FontGlyph *)((FONT_SDRAM_BASE + bitmap_bytes + 3U) & ~3U);

  for (si = 0U; si < FONT_SIZE_COUNT; ++si)
  {
    uint16_t size = FONT_SIZES[si];
    uint16_t bpr = (uint16_t)((size + 7U) / 8U);
    for (ci = 0U; ci < FONT_CHAR_COUNT; ++ci)
    {
      const LCD_FontGlyph *g = Teko_SemiBold_FindGlyph(FONT_CHARSET[ci], size);
      if (g == 0)
      {
        table[gi].bitmap = 0;
        table[gi].min_col = 0U;
        table[gi].width = 0U;
        table[gi].min_row = 0U;
        table[gi].height = 0U;
        table[gi].advance = 0U;
        ++gi;
        continue;
      }
      memcpy(bitmap_cursor, g->bitmap, (uint32_t)bpr * size);
      table[gi].bitmap = bitmap_cursor;
      table[gi].min_col = g->min_col;
      table[gi].width = g->width;
      table[gi].min_row = g->min_row;
      table[gi].height = g->height;
      table[gi].advance = g->advance;
      bitmap_cursor += (uint32_t)bpr * size;
      ++gi;
    }
  }

  g_font_glyph_table = table;
#endif
}

static const LCD_FontGlyph *FontSdram_GetGlyph(char ch, uint16_t size)
{
#if GUI_FONT_CACHE_IN_SDRAM
  uint8_t si = font_size_index(size);
  uint8_t ci;
  uint16_t gi;

  if (si == 0xFFU)
  {
    return 0;
  }
  if ((uint8_t)ch >= 128U)
  {
    return 0;
  }
  ci = g_font_ch_index[(uint8_t)ch];
  if (ci == 0xFFU)
  {
    return 0;
  }
  gi = (uint16_t)((uint16_t)si * FONT_CHAR_COUNT + ci);
  if (g_font_glyph_table == 0 || g_font_glyph_table[gi].bitmap == 0)
  {
    return 0;
  }
  return &g_font_glyph_table[gi];
#else
  return Teko_SemiBold_FindGlyph(ch, size);
#endif
}


static uint16_t font_space(uint16_t size)
{
  return (uint16_t)(size / 3U);
}

static uint16_t digit_advance(uint16_t size)
{
  const LCD_FontGlyph *zero = FontSdram_GetGlyph('0', size);

  return (zero != 0) ? zero->advance : font_space(size);
}

static uint8_t is_tabular_char(char ch)
{
  return (uint8_t)(ch == '0' || ch == '1' || ch == '2' || ch == '3' ||
                   ch == '4' || ch == '5' || ch == '6' || ch == '7' ||
                   ch == '8' || ch == '9' || ch == '-' || ch == '.' ||
                   ch == ':' || ch == '%' || ch == '/' || ch == '_' ||
                   ch == '+');
}

static uint16_t glyph_advance(char ch, uint16_t size)
{
  const LCD_FontGlyph *glyph;

  if (ch == ' ')
  {
    return font_space(size);
  }
  if (ch >= 'a' && ch <= 'z')
  {
    ch = (char)(ch - 'a' + 'A');
  }
  if (is_tabular_char(ch))
  {
    return digit_advance(size);
  }
  glyph = FontSdram_GetGlyph(ch, size);
  if (glyph != 0)
  {
    return glyph->advance;
  }
  if (ch == '_' || ch == '+')
  {
    return (uint16_t)(size / 2U);
  }
  return font_space(size);
}

static uint16_t text_width(const char *s, uint8_t style)
{
  uint16_t size = font_size(style);
  uint16_t width = 0U;

  while (*s != '\0')
  {
    width = (uint16_t)(width + glyph_advance(*s, size));
    ++s;
  }
  return width;
}

static void draw_symbol(uint16_t x, uint16_t y, char ch, uint16_t size, uint16_t cell, uint16_t fg)
{
  uint16_t w = cell;
  uint16_t thick = (size >= 34U) ? 4U : 3U;

  if (ch == '_')
  {
    fill(x, (uint16_t)(y + size - thick - 2U), w, thick, fg);
  }
  else if (ch == '+')
  {
    fill((uint16_t)(x + (w - thick) / 2U), (uint16_t)(y + size / 5U), thick,
         (uint16_t)(size - 2U * (size / 5U)), fg);
    fill(x, (uint16_t)(y + (size - thick) / 2U), w, thick, fg);
  }
}

static void draw_char(uint16_t x, uint16_t y, char ch, uint16_t size, uint16_t fg)
{
  const LCD_FontGlyph *glyph;
  uint16_t bytes_per_row = (uint16_t)((size + 7U) / 8U);
  uint16_t row;

  if (x >= LCD_DEV.width || y >= LCD_DEV.height)
  {
    return;
  }
  if (ch >= 'a' && ch <= 'z')
  {
    ch = (char)(ch - 'a' + 'A');
  }
  if (ch == '_' || ch == '+')
  {
    draw_symbol(x, y, ch, size, digit_advance(size), fg);
    return;
  }

  glyph = FontSdram_GetGlyph(ch, size);
  if (glyph == 0)
  {
    return;
  }

  /* Center tabular characters inside the uniform cell when their glyph is narrower. */
  if (is_tabular_char(ch))
  {
    uint16_t cell = digit_advance(size);

    if (glyph->advance < cell)
    {
      x = (uint16_t)(x + (cell - glyph->advance) / 2U);
    }
  }

  for (row = 0U; row < glyph->height; ++row)
  {
    uint16_t max_col;
    uint16_t src_row;
    uint16_t dst_y;
    const uint8_t *src;
    uint16_t min_col;
    uint16_t col;

    dst_y = (uint16_t)(y + row);
    if (dst_y >= LCD_DEV.height)
    {
      break;
    }

    max_col = glyph->width;
    if ((uint32_t)x + max_col > LCD_DEV.width)
    {
      max_col = (uint16_t)(LCD_DEV.width - x);
    }
    if (max_col == 0U)
    {
      break;
    }

    src_row = (uint16_t)(glyph->min_row + row);
    src = &glyph->bitmap[(uint32_t)src_row * bytes_per_row];
    min_col = glyph->min_col;
    for (col = 0U; col < max_col; ++col)
    {
      uint16_t sc = (uint16_t)(min_col + col);
      if ((src[sc >> 3U] & (uint8_t)(0x80U >> (sc & 0x07U))) != 0U)
      {
        LCD_DrawPixelColor((uint16_t)(x + col), dst_y, fg);
      }
    }
  }
}

void draw_text(uint16_t x, uint16_t y, const char *s, uint8_t style, uint16_t fg)
{
  uint16_t size = font_size(style);

  while (*s != '\0')
  {
    if (*s != ' ')
    {
      draw_char(x, y, *s, size, fg);
    }
    x = (uint16_t)(x + glyph_advance(*s, size));
    ++s;
  }
}

void draw_text_center(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                             const char *s, uint8_t style, uint16_t fg)
{
  uint16_t size = font_size(style);
  uint16_t tw = text_width(s, style);
  uint16_t tx = x;
  uint16_t ty = y;

  if (tw < w)
  {
    tx = (uint16_t)(x + (w - tw) / 2U);
  }
  if (size < h)
  {
    ty = (uint16_t)(y + (h - size) / 2U);
  }
  draw_text(tx, ty, s, style, fg);
}

void draw_text_right(uint16_t x, uint16_t y, uint16_t w, const char *s,
                            uint8_t style, uint16_t fg)
{
  uint16_t tw = text_width(s, style);
  uint16_t tx = (tw < w) ? (uint16_t)(x + w - tw) : x;
  draw_text(tx, y, s, style, fg);
}

void format_float(char *buf, uint32_t len, float value, uint8_t digits)
{
  switch (digits)
  {
  case 0U: (void)snprintf(buf, len, "%.0f", (double)value); break;
  case 1U: (void)snprintf(buf, len, "%.1f", (double)value); break;
  case 2U: (void)snprintf(buf, len, "%.2f", (double)value); break;
  case 4U: (void)snprintf(buf, len, "%.4f", (double)value); break;
  default: (void)snprintf(buf, len, "%.3f", (double)value); break;
  }
}

void format_param_value(char *buf, uint32_t len, const ui_param_t *param)
{
  if (strcmp(param->unit, "HEX") == 0)
  {
    (void)snprintf(buf, len, "0X%04X", (unsigned)((uint32_t)param->value & 0xFFFFU));
    return;
  }
  format_float(buf, len, param->value, param->digits);
}

void draw_cell(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t fill_color)
{
  fill(x, y, w, h, fill_color);
  border(x, y, w, h, UI_COLOR_BORDER);
}

void draw_button(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                        const char *label, uint8_t active, uint8_t disabled)
{
  uint16_t bg = disabled ? UI_COLOR_DISABLED : (active ? UI_COLOR_INK : UI_COLOR_BG);
  uint16_t fg = disabled ? UI_COLOR_MUTED : (active ? UI_COLOR_BG : UI_COLOR_MUTED);
  draw_cell(x, y, w, h, bg);
  draw_text_center(x, y, w, h, label, 2U, fg);
}

void draw_big_value(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                           const char *title, const char *badge, const char *value,
                           const char *unit, const char *meta_l, const char *meta_r,
                           uint8_t inverted, const char *side_label, const char *side_value)
{
  uint16_t panel = inverted ? UI_COLOR_INK : UI_COLOR_PANEL;
  uint16_t fg    = inverted ? UI_COLOR_BG : UI_COLOR_TEXT;
  uint16_t sub   = inverted ? UI_COLOR_BG : UI_COLOR_MUTED;
  uint16_t line  = inverted ? UI_COLOR_BG : UI_COLOR_BORDER;
  uint16_t value_x = (uint16_t)(x + 16U);
  uint16_t unit_x;

  draw_cell(x, y, w, h, panel);
  draw_text((uint16_t)(x + 16U), (uint16_t)(y + 8U), title, 1U, sub);
  fill((uint16_t)(x + w - 72U), (uint16_t)(y + 8U), 56U, 24U, UI_COLOR_ACCENT);
  draw_text_center((uint16_t)(x + w - 72U), (uint16_t)(y + 8U), 56U, 24U, badge, 1U, UI_COLOR_INK);

  /* Optional right-side metric block (energy / efficiency). */
  if (side_label != 0 && side_value != 0)
  {
    uint16_t sx = (uint16_t)(x + w - 124U);

    draw_text(sx, (uint16_t)(y + 34U), side_label, 1U, sub);
    draw_text(sx, (uint16_t)(y + 58U), side_value, 3U, fg);
  }

  draw_text(value_x, (uint16_t)(y + 40U), value, 4U, fg);
  unit_x = (uint16_t)(value_x + text_width(value, 4U) + 12U);
  draw_text(unit_x, (uint16_t)(y + 58U), unit, 3U, fg);
  fill((uint16_t)(x + UI_BORDER), (uint16_t)(y + h - 26U),
       (uint16_t)(w - 2U * UI_BORDER), UI_BORDER, line);
  draw_text((uint16_t)(x + 16U), (uint16_t)(y + h - 20U), meta_l, 1U, sub);
  draw_text_right((uint16_t)(x + 200U), (uint16_t)(y + h - 20U), (uint16_t)(w - 216U), meta_r, 1U, sub);
}

