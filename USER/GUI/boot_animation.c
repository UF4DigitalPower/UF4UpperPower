#include "boot_animation.h"

#include "bsp_lcd.h"
#include "Font/Teko_SemiBold_144.h"

#define BOOT_BG       0x0861U  /* #0d0e0c */
#define BOOT_INK      0xF7BDU  /* #f4f4ec */
#define BOOT_ACCENT   0xDFE7U  /* #d9ff3f */
#define BOOT_MUTED    0xB594U  /* #b1b1a7 */

#define BOOT_BRAND_X  53
#define BOOT_BRAND_Y  304
#define BOOT_BRAND_W  374
#define BOOT_NAME_H   128
#define BOOT_RULE_Y   440
#define BOOT_END_MS   2250U

/* "DigitalPower" at 24 px from the existing Teko_SemiBold.ttf. */
static const uint8_t boot_subtitle_bitmap[266] = {
  0x00, 0x07, 0x00, 0x0E, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x7F, 0xC7, 0x00, 0x0E, 0x00, 0x00, 0x0E, 0x7F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x7F, 0xE7, 0x00, 0x0E, 0x38, 0x00, 0x0E, 0x7F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x79, 0xE0, 0x00, 0x00, 0x38, 0x00, 0x0E, 0x7F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x78, 0xE7, 0x1F, 0xCE, 0x7E, 0x3F, 0xCE, 0x71, 0xC7, 0xF3, 0x8E, 0x39, 0xFC, 0x7E,
  0x78, 0xE7, 0x3F, 0xCE, 0x7E, 0x7F, 0xCE, 0x71, 0xCF, 0xFB, 0x8E, 0x3B, 0xFE, 0x7E,
  0x78, 0xE7, 0x3F, 0xCE, 0x7E, 0x7F, 0xCE, 0x71, 0xCF, 0x79, 0xDF, 0x73, 0xDE, 0x7E,
  0x78, 0xE7, 0x39, 0xCE, 0x38, 0x73, 0xCE, 0x71, 0xCE, 0x39, 0xDF, 0x73, 0x8E, 0x78,
  0x78, 0xE7, 0x39, 0xCE, 0x38, 0x73, 0xCE, 0x7F, 0xCE, 0x39, 0xDB, 0x73, 0x8E, 0x78,
  0x78, 0xE7, 0x39, 0xCE, 0x38, 0x73, 0xCE, 0x7F, 0xCE, 0x39, 0xDB, 0x73, 0xFE, 0x78,
  0x78, 0xE7, 0x39, 0xCE, 0x38, 0x73, 0xCE, 0x7F, 0x0E, 0x39, 0xDB, 0x73, 0xFE, 0x78,
  0x78, 0xE7, 0x39, 0xCE, 0x38, 0x73, 0xCE, 0x70, 0x0E, 0x38, 0xDB, 0x63, 0x80, 0x78,
  0x78, 0xE7, 0x39, 0xCE, 0x38, 0x73, 0xCE, 0x70, 0x0E, 0x38, 0xFB, 0xE3, 0x8E, 0x78,
  0x79, 0xE7, 0x3F, 0xCE, 0x3E, 0x7F, 0xCE, 0x70, 0x0F, 0x78, 0xFB, 0xE3, 0xDE, 0x78,
  0x7F, 0xE7, 0x3F, 0xCE, 0x3E, 0x7F, 0xCE, 0x70, 0x0F, 0xF8, 0xF1, 0xE3, 0xFE, 0x78,
  0x7F, 0xC7, 0x0D, 0xCE, 0x1E, 0x3F, 0xCE, 0x70, 0x07, 0xF0, 0xF1, 0xE1, 0xFC, 0x78,
  0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x3F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const LCD_FontGlyph boot_subtitle_glyph = {
  boot_subtitle_bitmap, 0U, 112U, 0U, 19U, 112U
};

static const LCD_FontGlyph *boot_find_glyph(char ch)
{
  switch (ch)
  {
  case 'U': return &glyph_144_0055;
  case 'F': return &glyph_144_0046;
  case '4': return &glyph_144_0034;
  default: return 0;
  }
}

static float boot_ease(uint32_t elapsed, uint32_t delay, uint32_t duration)
{
  float x;
  float lo = 0.0f;
  float hi = 1.0f;
  uint8_t i;

  if (elapsed <= delay) return 0.0f;
  if (elapsed - delay >= duration) return 1.0f;
  x = (float)(elapsed - delay) / (float)duration;

  /* CSS cubic-bezier(.16, 1, .3, 1): invert its time axis. */
  for (i = 0U; i < 12U; ++i)
  {
    float t = (lo + hi) * 0.5f;
    float inv = 1.0f - t;
    float curve_x = 3.0f * inv * inv * t * 0.16f +
                    3.0f * inv * t * t * 0.3f + t * t * t;
    if (curve_x < x) lo = t;
    else hi = t;
  }
  x = 1.0f - (lo + hi) * 0.5f;
  return 1.0f - x * x * x;
}

static uint16_t boot_blend(uint16_t fg, float opacity)
{
  uint16_t r = (uint16_t)((float)((BOOT_BG >> 11) & 31U) * (1.0f - opacity) +
                          (float)((fg >> 11) & 31U) * opacity + 0.5f);
  uint16_t g = (uint16_t)((float)((BOOT_BG >> 5) & 63U) * (1.0f - opacity) +
                          (float)((fg >> 5) & 63U) * opacity + 0.5f);
  uint16_t b = (uint16_t)((float)(BOOT_BG & 31U) * (1.0f - opacity) +
                          (float)(fg & 31U) * opacity + 0.5f);
  return (uint16_t)((r << 11) | (g << 5) | b);
}

static void boot_glyph(const LCD_FontGlyph *glyph, uint16_t size,
                       int16_t x, int16_t y, int16_t clip_y, int16_t clip_h,
                       uint16_t fg, float opacity, float angle)
{
  uint16_t row;
  uint16_t color;
  uint16_t stride;
  float sine;
  float cosine;
  float half_width;
  float half_height;

  if (glyph == 0 || opacity <= 0.0f) return;
  color = boot_blend(fg, opacity);
  stride = (uint16_t)((size + 7U) / 8U);
  sine = angle * 0.121869f; /* 7 degrees at the start of the rise. */
  cosine = 1.0f - 0.5f * sine * sine;
  half_width = (float)glyph->width * 0.5f;
  half_height = (float)glyph->height * 0.5f;

  for (row = 0U; row < glyph->height; ++row)
  {
    uint16_t col;
    const uint8_t *bits;
    float dy = (float)row - half_height;

    bits = glyph->bitmap + (uint32_t)(glyph->min_row + row) * stride;
    for (col = 0U; col < glyph->width; ++col)
    {
      uint16_t source_col = (uint16_t)(glyph->min_col + col);
      if ((bits[source_col >> 3U] & (uint8_t)(0x80U >> (source_col & 7U))) != 0U)
      {
        float dx = (float)col - half_width;
        int16_t px = (int16_t)((float)x + half_width + dx * cosine - dy * sine + 0.5f);
        int16_t py = (int16_t)((float)y + half_height + dx * sine + dy * cosine + 0.5f);
        if (px >= 0 && px < LCD_WIDTH && py >= clip_y && py < clip_y + clip_h &&
            py >= 0 && py < LCD_HEIGHT)
        {
          LCD_DrawPixelColor((uint16_t)px, (uint16_t)py, color);
        }
      }
    }
  }
}

static void boot_draw_name(uint32_t elapsed)
{
  static const char letters[] = "UF4";
  static const uint16_t delays[] = {180U, 310U, 440U};
  int16_t x = BOOT_BRAND_X;
  uint8_t i;

  for (i = 0U; i < 3U; ++i)
  {
    const LCD_FontGlyph *glyph = boot_find_glyph(letters[i]);
    float progress = boot_ease(elapsed, delays[i], 860U);
    int16_t rise = (int16_t)((1.0f - progress) * 134.0f);
    boot_glyph(glyph, 144U, x, (int16_t)(BOOT_BRAND_Y + 18 + rise),
               BOOT_BRAND_Y, BOOT_NAME_H, BOOT_INK, progress, 1.0f - progress);
    if (glyph != 0) x = (int16_t)(x + (int16_t)glyph->advance);
  }
}

static void boot_rule_piece(int16_t x, int16_t w)
{
  int16_t left = x < BOOT_BRAND_X ? BOOT_BRAND_X : x;
  int16_t right = x + w > BOOT_BRAND_X + BOOT_BRAND_W ?
                  BOOT_BRAND_X + BOOT_BRAND_W : x + w;
  if (right > left)
  {
    LCD_FillRect((uint16_t)left, BOOT_RULE_Y,
                 (uint16_t)(right - left), 3U, BOOT_ACCENT);
  }
}

static void boot_draw_rule(uint32_t elapsed)
{
  float first = boot_ease(elapsed, 890U, 720U);
  float second = boot_ease(elapsed, 980U, 720U);
  boot_rule_piece((int16_t)(BOOT_BRAND_X - (1.0f - first) * 140.0f), 135);
  boot_rule_piece((int16_t)(BOOT_BRAND_X + 135 + (1.0f - second) * 249.0f), 239);
}

static void boot_draw_subtitle(uint32_t elapsed)
{
  float progress = boot_ease(elapsed, 1250U, 700U);
  boot_glyph(&boot_subtitle_glyph, 112U, BOOT_BRAND_X,
             (int16_t)(BOOT_RULE_Y + 3 + 24 + 4 +
                       (int16_t)((1.0f - progress) * 14.0f)),
             BOOT_RULE_Y + 3, 60, BOOT_MUTED, progress, 0.0f);
}

void BootAnimation_Play(void)
{
  uint32_t start = HAL_GetTick();
  uint32_t elapsed;

  do
  {
    elapsed = HAL_GetTick() - start;
    if (elapsed > BOOT_END_MS) elapsed = BOOT_END_MS;
    LCD_Clear(BOOT_BG);
    boot_draw_name(elapsed);
    boot_draw_rule(elapsed);
    boot_draw_subtitle(elapsed);
    LCD_CommitFrameFromDrawNoSync();
    HAL_Delay(16U);
  } while (elapsed < BOOT_END_MS);
  LCD_WaitForIdle();
}
