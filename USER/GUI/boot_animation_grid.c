#include "boot_animation.h"
#include "gui_internal.h"

#define BOOT_DURATION_MS 2400U
#define BOOT_FRAME_MS      16U

static uint16_t boot_ease(uint32_t now, uint32_t start, uint32_t end)
{
  uint32_t t;
  if (now <= start) return 0U;
  if (now >= end) return 1000U;
  t = (now - start) * 1000U / (end - start);
  return (uint16_t)(t * t * (3000U - 2U * t) / 1000000U);
}

static uint16_t boot_mix(uint16_t background, uint16_t foreground, uint16_t opacity)
{
  uint32_t inverse = 1000U - opacity;
  uint32_t red = (((background >> 11U) & 31U) * inverse +
                  ((foreground >> 11U) & 31U) * opacity) / 1000U;
  uint32_t green = (((background >> 5U) & 63U) * inverse +
                    ((foreground >> 5U) & 63U) * opacity) / 1000U;
  uint32_t blue = ((background & 31U) * inverse +
                   (foreground & 31U) * opacity) / 1000U;
  return (uint16_t)((red << 11U) | (green << 5U) | blue);
}

static void boot_cell(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                      uint16_t color)
{
  fill(x, y, w, h, color);
  border(x, y, w, h, UI_COLOR_BORDER_DEFAULT);
}

static void boot_draw(uint32_t elapsed)
{
  static const char *const status[5] = {
    "UF4", "DISPLAY", "CONTROL", "OUTPUT", "SIM"
  };
  static const char *const nav[4] = {
    "HOME", "PARAMS", "SETTINGS", "LOG"
  };
  uint16_t intro = boot_ease(elapsed, 84U, 648U);
  uint16_t reveal = boot_ease(elapsed, 336U, 888U);
  uint16_t load = boot_ease(elapsed, 576U, 1896U);
  uint16_t brand_ink = boot_mix(UI_COLOR_INK_DEFAULT, UI_COLOR_BG_DEFAULT, intro);
  uint16_t brand_muted = boot_mix(UI_COLOR_INK_DEFAULT, UI_COLOR_MUTED_DEFAULT, intro);
  uint16_t brand_accent = boot_mix(UI_COLOR_INK_DEFAULT, UI_COLOR_ACCENT_DEFAULT, intro);
  uint16_t brand_y = (uint16_t)(211U + (1000U - intro) * 32U / 1000U);
  uint16_t rule_width = (uint16_t)(448U * reveal / 1000U);
  uint16_t progress_width = (uint16_t)(448U * load / 1000U);
  char percent[8];
  uint8_t i;

  LCD_Clear(UI_COLOR_BG_DEFAULT);
  draw_text(16U, 10U, "UF4 DIGITAL POWER", 2U, UI_COLOR_MUTED_DEFAULT);
  draw_text(16U, 35U, "DC POWER SUPPLY", 3U, UI_COLOR_TEXT_DEFAULT);
  boot_cell(362U, 0U, 118U, 76U, UI_COLOR_DISABLED_DEFAULT);
  draw_text_center(362U, 0U, 118U, 76U, "BOOT", 2U, UI_COLOR_MUTED_DEFAULT);

  for (i = 0U; i < 5U; ++i)
  {
    uint16_t x = (uint16_t)i * 96U;
    boot_cell(x, 76U, 96U, 42U, UI_COLOR_PANEL_DEFAULT);
    draw_text_center(x, 76U, 96U, 42U, status[i], 1U, UI_COLOR_MUTED_DEFAULT);
  }

  boot_cell(0U, 118U, UI_W, 385U, UI_COLOR_INK_DEFAULT);
  draw_text(16U, 137U, "STARTUP / INTERFACE", 2U, UI_COLOR_BG_DEFAULT);
  draw_text_right(315U, 139U, 149U, "480 X 800", 1U, UI_COLOR_BG_DEFAULT);
  fill(16U, (uint16_t)(brand_y + 6U), 6U, 69U, brand_accent);
  draw_text(40U, brand_y, "UF4", 4U, brand_ink);
  draw_text(40U, (uint16_t)(brand_y + 83U), "DIGITAL POWER SUPPLY", 3U, brand_ink);
  draw_text(40U, (uint16_t)(brand_y + 135U), "PRECISION / CONTROL / OUTPUT", 1U, brand_muted);
  if (rule_width != 0U)
  {
    fill(16U, 432U, rule_width, 4U, UI_COLOR_ACCENT_DEFAULT);
  }
  draw_text(16U, 466U, "POWER CONTROL INTERFACE", 1U, UI_COLOR_BG_DEFAULT);
  draw_text_right(365U, 466U, 99U, "LVGL", 1U, UI_COLOR_BG_DEFAULT);

  boot_cell(0U, 503U, UI_W, 110U, UI_COLOR_PANEL_DEFAULT);
  draw_text(16U, 514U, "INTERFACE STARTUP", 1U, UI_COLOR_MUTED_DEFAULT);
  (void)snprintf(percent, sizeof(percent), "%u%%", (unsigned)(load / 10U));
  draw_text(16U, 536U, percent, 4U, UI_COLOR_TEXT_DEFAULT);
  draw_text_right(280U, 559U, 184U,
                  elapsed >= 1896U ? "READY" : "INITIALIZING",
                  2U, UI_COLOR_MUTED_DEFAULT);

  boot_cell(0U, 613U, UI_W, 135U, UI_COLOR_BG_DEFAULT);
  draw_text(16U, 634U, "LOADING INTERFACE", 2U, UI_COLOR_TEXT_DEFAULT);
  draw_text_right(345U, 636U, 119U, "01 / 01", 1U, UI_COLOR_MUTED_DEFAULT);
  fill(16U, 686U, 448U, 14U, UI_COLOR_DISABLED_DEFAULT);
  if (progress_width != 0U)
  {
    fill(16U, 686U, progress_width, 14U, UI_COLOR_ACCENT_DEFAULT);
  }
  draw_text(16U, 711U, "UF4 / DIGITAL POWER SUPPLY", 1U, UI_COLOR_MUTED_DEFAULT);

  for (i = 0U; i < 4U; ++i)
  {
    uint16_t x = (uint16_t)i * 120U;
    boot_cell(x, 748U, 120U, 52U,
              i == 0U ? UI_COLOR_INK_DEFAULT : UI_COLOR_BG_DEFAULT);
    draw_text_center(x, 748U, 120U, 52U, nav[i], 2U,
                     i == 0U ? UI_COLOR_BG_DEFAULT : UI_COLOR_MUTED_DEFAULT);
  }
}

void BootAnimation_PlayGrid(void)
{
  uint32_t start = HAL_GetTick();
  uint32_t elapsed;

  do
  {
    elapsed = HAL_GetTick() - start;
    if (elapsed > BOOT_DURATION_MS) elapsed = BOOT_DURATION_MS;
    boot_draw(elapsed);
    LCD_CommitFrameFromDrawNoSync();
    HAL_Delay(BOOT_FRAME_MS);
  } while (elapsed < BOOT_DURATION_MS);

  LCD_WaitForIdle();
}
