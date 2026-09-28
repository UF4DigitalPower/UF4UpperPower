/**
  ******************************************************************************
  * @file    bsp_lcd.c
  * @author  UF4
  * @date    26-8-14
  * @brief   ST7701 RGB565 display driver with LTDC/DMA2D double-buffered frame sync.
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

#include "bsp_lcd.h"
#include "bsp_lcd_ll.h"
#include "bsp_st7701.h"
#include "tim.h"

#define LCD_PIPELINE_TIMEOUT_MS 250U

typedef enum {
  LCD_DMA_OWNER_NONE = 0,
  LCD_DMA_OWNER_DRAW,
  LCD_DMA_OWNER_PRESENT_SYNC
} lcd_dma_owner_t;

volatile lcd_dev LCD_DEV = {
  .width = LCD_WIDTH,
  .height = LCD_HEIGHT,
  .front_addr = LCD_FRAMEBUFFER_ADDR,
  .draw_addr = LCD_FRAMEBUFFER_BACK_ADDR,
};

uint32_t POINT_COLOR = WHITE;
uint32_t BACK_COLOR = BLACK;

static volatile uint32_t g_lcd_front_buffer_addr = LCD_FRAMEBUFFER_ADDR;
static volatile uint32_t g_lcd_draw_buffer_addr = LCD_FRAMEBUFFER_BACK_ADDR;
static uint8_t g_lcd_backlight_level = 5U;
static uint8_t g_lcd_backlight_pwm_started;
static volatile uint8_t g_lcd_dma_busy;
static volatile uint8_t g_lcd_dma_ok = 1U;
static volatile uint8_t g_lcd_dma_owner;
static volatile uint8_t g_lcd_present_state;
static volatile uint8_t g_lcd_present_swap;
static volatile uint8_t g_lcd_sync_index;
static volatile uint8_t g_lcd_sync_count;
static volatile uint32_t g_lcd_pending_buffer_addr;
static volatile lcd_rect_t g_lcd_sync_rects[LCD_MAX_DIRTY_RECTS];
static volatile lcd_performance_counters_t g_lcd_counters;

static void lcd_wait_pipeline_idle(void);
static void lcd_start_sync_rect(uint8_t index);

static uint16_t lcd_backlight_duty_permille(uint8_t level)
{
  static const uint16_t duty[] = {150U, 300U, 500U, 650U, 800U, 1000U};

  if (level >= (uint8_t)(sizeof(duty) / sizeof(duty[0])))
  {
    level = (uint8_t)((sizeof(duty) / sizeof(duty[0])) - 1U);
  }

  return duty[level];
}

static void lcd_backlight_apply_pwm(void)
{
  uint32_t period = __HAL_TIM_GET_AUTORELOAD(&htim2);
  uint16_t duty = lcd_backlight_duty_permille(g_lcd_backlight_level);
  uint32_t pulse;

  /*
   * TIM2 CH4 uses PWM1.  ARR is the last counter value, so ARR+1 is
   * required for a genuinely continuous high output at 100% duty.
   */
  if (duty >= 1000U)
  {
    pulse = period + 1U;
  }
  else
  {
    pulse = ((period + 1U) * duty) / 1000U;
  }

  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, pulse);
}

void LCD_SetBacklightLevel(uint8_t level)
{
  if (level > 5U)
  {
    level = 5U;
  }

  g_lcd_backlight_level = level;
  lcd_backlight_apply_pwm();
}

uint8_t LCD_GetBacklightLevel(void)
{
  return g_lcd_backlight_level;
}

static void lcd_ltdc_reload_immediate(void)
{
  if (LCD_LL_LTDCReloadImmediate() != HAL_OK)
  {
    Error_Handler();
  }
}

static void lcd_present_buffer_immediate(uint32_t buffer_addr)
{
  if (LCD_LL_LTDCSetAddressNoReload(buffer_addr) != HAL_OK)
  {
    Error_Handler();
  }

  lcd_ltdc_reload_immediate();
}

static void lcd_dma2d_fill_rgb565(uint32_t dst_addr, uint16_t width, uint16_t height,
                                  uint16_t dst_offline, uint16_t color)
{
  lcd_wait_pipeline_idle();
  LCD_LL_DMA2DClearFlags();
  g_lcd_dma_ok = 1U;
  g_lcd_dma_owner = LCD_DMA_OWNER_DRAW;
  g_lcd_dma_busy = 1U;
  LCD_LL_DMA2DStartFillRgb565(dst_addr, width, height, dst_offline, color);
  lcd_wait_pipeline_idle();
}

static void lcd_dma2d_copy_rgb565(uint32_t src_addr, uint32_t dst_addr,
                                  uint16_t width, uint16_t height,
                                  uint16_t src_offline, uint16_t dst_offline)
{
  lcd_wait_pipeline_idle();
  LCD_LL_DMA2DClearFlags();
  g_lcd_dma_ok = 1U;
  g_lcd_dma_owner = LCD_DMA_OWNER_DRAW;
  g_lcd_dma_busy = 1U;
  LCD_LL_DMA2DStartCopyRgb565(src_addr, dst_addr, width, height, src_offline, dst_offline);
  lcd_wait_pipeline_idle();
}

static void lcd_overlay_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
  uint16_t offline;
  uint32_t dst_addr;

  if (w == 0U || h == 0U || x >= LCD_OVERLAY_WIDTH || y >= LCD_OVERLAY_HEIGHT)
  {
    return;
  }

  if ((uint32_t)x + w > LCD_OVERLAY_WIDTH)
  {
    w = (uint16_t)(LCD_OVERLAY_WIDTH - x);
  }
  if ((uint32_t)y + h > LCD_OVERLAY_HEIGHT)
  {
    h = (uint16_t)(LCD_OVERLAY_HEIGHT - y);
  }

  offline = (uint16_t)(LCD_OVERLAY_WIDTH - w);
  dst_addr = LCD_OVERLAY_FRAMEBUFFER_ADDR +
             LCD_BYTES_PER_PIXEL * ((uint32_t)LCD_OVERLAY_WIDTH * y + x);
  lcd_dma2d_fill_rgb565(dst_addr, w, h, offline, color);
}

static void lcd_overlay_draw_hline(uint16_t x, uint16_t y, uint16_t w, uint16_t color)
{
  lcd_overlay_fill_rect(x, y, w, 1U, color);
}

static void lcd_overlay_draw_vline(uint16_t x, uint16_t y, uint16_t h, uint16_t color)
{
  lcd_overlay_fill_rect(x, y, 1U, h, color);
}

static void lcd_overlay_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
  if (w < 2U || h < 2U)
  {
    return;
  }

  lcd_overlay_draw_hline(x, y, w, color);
  lcd_overlay_draw_hline(x, (uint16_t)(y + h - 1U), w, color);
  lcd_overlay_draw_vline(x, y, h, color);
  lcd_overlay_draw_vline((uint16_t)(x + w - 1U), y, h, color);
}

static uint8_t lcd_demo_font_row(char ch, uint8_t row)
{
  static const uint8_t blank[7] = {0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U};
  static const uint8_t glyph_0[7] = {0x0EU, 0x11U, 0x13U, 0x15U, 0x19U, 0x11U, 0x0EU};
  static const uint8_t glyph_1[7] = {0x04U, 0x0CU, 0x04U, 0x04U, 0x04U, 0x04U, 0x0EU};
  static const uint8_t glyph_2[7] = {0x0EU, 0x11U, 0x01U, 0x02U, 0x04U, 0x08U, 0x1FU};
  static const uint8_t glyph_3[7] = {0x1EU, 0x01U, 0x01U, 0x0EU, 0x01U, 0x01U, 0x1EU};
  static const uint8_t glyph_4[7] = {0x12U, 0x12U, 0x12U, 0x1FU, 0x02U, 0x02U, 0x02U};
  static const uint8_t glyph_5[7] = {0x1FU, 0x10U, 0x10U, 0x1EU, 0x01U, 0x01U, 0x1EU};
  static const uint8_t glyph_6[7] = {0x0EU, 0x10U, 0x10U, 0x1EU, 0x11U, 0x11U, 0x0EU};
  static const uint8_t glyph_7[7] = {0x1FU, 0x01U, 0x02U, 0x04U, 0x08U, 0x08U, 0x08U};
  static const uint8_t glyph_8[7] = {0x0EU, 0x11U, 0x11U, 0x0EU, 0x11U, 0x11U, 0x0EU};
  static const uint8_t glyph_9[7] = {0x0EU, 0x11U, 0x11U, 0x0FU, 0x01U, 0x01U, 0x0EU};
  static const uint8_t glyph_dot[7] = {0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x0CU, 0x0CU};
  static const uint8_t glyph_slash[7] = {0x01U, 0x02U, 0x02U, 0x04U, 0x08U, 0x08U, 0x10U};
  static const uint8_t glyph_a[7] = {0x0EU, 0x11U, 0x11U, 0x1FU, 0x11U, 0x11U, 0x11U};
  static const uint8_t glyph_b[7] = {0x1EU, 0x11U, 0x11U, 0x1EU, 0x11U, 0x11U, 0x1EU};
  static const uint8_t glyph_c[7] = {0x0FU, 0x10U, 0x10U, 0x10U, 0x10U, 0x10U, 0x0FU};
  static const uint8_t glyph_d[7] = {0x1EU, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x1EU};
  static const uint8_t glyph_e[7] = {0x1FU, 0x10U, 0x10U, 0x1EU, 0x10U, 0x10U, 0x1FU};
  static const uint8_t glyph_f[7] = {0x1FU, 0x10U, 0x10U, 0x1EU, 0x10U, 0x10U, 0x10U};
  static const uint8_t glyph_g[7] = {0x0EU, 0x11U, 0x10U, 0x17U, 0x11U, 0x11U, 0x0FU};
  static const uint8_t glyph_h[7] = {0x11U, 0x11U, 0x11U, 0x1FU, 0x11U, 0x11U, 0x11U};
  static const uint8_t glyph_i[7] = {0x0EU, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x0EU};
  static const uint8_t glyph_j[7] = {0x07U, 0x02U, 0x02U, 0x02U, 0x12U, 0x12U, 0x0CU};
  static const uint8_t glyph_k[7] = {0x11U, 0x12U, 0x14U, 0x18U, 0x14U, 0x12U, 0x11U};
  static const uint8_t glyph_l[7] = {0x10U, 0x10U, 0x10U, 0x10U, 0x10U, 0x10U, 0x1FU};
  static const uint8_t glyph_m[7] = {0x11U, 0x1BU, 0x15U, 0x15U, 0x11U, 0x11U, 0x11U};
  static const uint8_t glyph_n[7] = {0x11U, 0x19U, 0x15U, 0x13U, 0x11U, 0x11U, 0x11U};
  static const uint8_t glyph_o[7] = {0x0EU, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x0EU};
  static const uint8_t glyph_p[7] = {0x1EU, 0x11U, 0x11U, 0x1EU, 0x10U, 0x10U, 0x10U};
  static const uint8_t glyph_q[7] = {0x0EU, 0x11U, 0x11U, 0x11U, 0x15U, 0x12U, 0x0DU};
  static const uint8_t glyph_r[7] = {0x1EU, 0x11U, 0x11U, 0x1EU, 0x14U, 0x12U, 0x11U};
  static const uint8_t glyph_s[7] = {0x0FU, 0x10U, 0x10U, 0x0EU, 0x01U, 0x01U, 0x1EU};
  static const uint8_t glyph_t[7] = {0x1FU, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U};
  static const uint8_t glyph_u[7] = {0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x0EU};
  static const uint8_t glyph_v[7] = {0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x0AU, 0x04U};
  static const uint8_t glyph_w[7] = {0x11U, 0x11U, 0x11U, 0x15U, 0x15U, 0x15U, 0x0AU};
  static const uint8_t glyph_x[7] = {0x11U, 0x11U, 0x0AU, 0x04U, 0x0AU, 0x11U, 0x11U};
  static const uint8_t glyph_y[7] = {0x11U, 0x11U, 0x0AU, 0x04U, 0x04U, 0x04U, 0x04U};
  static const uint8_t glyph_z[7] = {0x1FU, 0x01U, 0x02U, 0x04U, 0x08U, 0x10U, 0x1FU};
  const uint8_t *glyph = blank;

  switch (ch)
  {
  case '0': glyph = glyph_0; break;
  case '1': glyph = glyph_1; break;
  case '2': glyph = glyph_2; break;
  case '3': glyph = glyph_3; break;
  case '4': glyph = glyph_4; break;
  case '5': glyph = glyph_5; break;
  case '6': glyph = glyph_6; break;
  case '7': glyph = glyph_7; break;
  case '8': glyph = glyph_8; break;
  case '9': glyph = glyph_9; break;
  case '.': glyph = glyph_dot; break;
  case '/': glyph = glyph_slash; break;
  case 'A': glyph = glyph_a; break;
  case 'B': glyph = glyph_b; break;
  case 'C': glyph = glyph_c; break;
  case 'D': glyph = glyph_d; break;
  case 'E': glyph = glyph_e; break;
  case 'F': glyph = glyph_f; break;
  case 'G': glyph = glyph_g; break;
  case 'H': glyph = glyph_h; break;
  case 'I': glyph = glyph_i; break;
  case 'J': glyph = glyph_j; break;
  case 'K': glyph = glyph_k; break;
  case 'L': glyph = glyph_l; break;
  case 'M': glyph = glyph_m; break;
  case 'N': glyph = glyph_n; break;
  case 'O': glyph = glyph_o; break;
  case 'P': glyph = glyph_p; break;
  case 'Q': glyph = glyph_q; break;
  case 'R': glyph = glyph_r; break;
  case 'S': glyph = glyph_s; break;
  case 'T': glyph = glyph_t; break;
  case 'U': glyph = glyph_u; break;
  case 'V': glyph = glyph_v; break;
  case 'W': glyph = glyph_w; break;
  case 'X': glyph = glyph_x; break;
  case 'Y': glyph = glyph_y; break;
  case 'Z': glyph = glyph_z; break;
  default: break;
  }

  return glyph[row];
}

static void lcd_overlay_draw_char(uint16_t x, uint16_t y, char ch, uint8_t scale, uint16_t color)
{
  uint8_t row;

  for (row = 0U; row < 7U; ++row)
  {
    uint8_t bits = lcd_demo_font_row(ch, row);
    uint8_t col;

    for (col = 0U; col < 5U; ++col)
    {
      if ((bits & (uint8_t)(1U << (4U - col))) != 0U)
      {
        lcd_overlay_fill_rect((uint16_t)(x + col * scale),
                              (uint16_t)(y + row * scale),
                              scale,
                              scale,
                              color);
      }
    }
  }
}

static void lcd_overlay_draw_string(uint16_t x, uint16_t y, const char *str, uint8_t scale, uint16_t color)
{
  while (*str != '\0')
  {
    lcd_overlay_draw_char(x, y, *str, scale, color);
    x = (uint16_t)(x + 6U * scale);
    ++str;
  }
}

static void lcd_overlay_clear_popup(void)
{
  lcd_overlay_fill_rect(0U, 0U, LCD_POPUP_WIDTH, LCD_POPUP_HEIGHT, 0xFFFFU);
}

static void lcd_overlay_draw_popup(const char *title, const char *message)
{
  lcd_overlay_clear_popup();
  lcd_overlay_fill_rect(0U, 0U, LCD_POPUP_WIDTH, LCD_POPUP_HEIGHT, 0xFFFFU);
  lcd_overlay_fill_rect(0U, 0U, LCD_POPUP_WIDTH, 6U, 0xD7E8U);
  lcd_overlay_fill_rect(0U, 0U, 6U, LCD_POPUP_HEIGHT, 0x1082U);
  lcd_overlay_draw_rect(0U, 0U, LCD_POPUP_WIDTH, LCD_POPUP_HEIGHT, 0x1082U);
  lcd_overlay_draw_string(12U, 12U, title, 2U, 0x1082U);
  lcd_overlay_draw_string(12U, 44U, message, 1U, 0x5AEBU);
}

void LCD_OverlayPopupShow(uint16_t x, uint16_t y, const char *title, const char *message)
{
  if (title == 0)
  {
    title = "";
  }
  if (message == 0)
  {
    message = "";
  }

  lcd_overlay_draw_popup(title, message);
  lcd_wait_pipeline_idle();
  if (HAL_LTDC_SetWindowPosition_NoReload(&hltdc, x, y, LTDC_LAYER_2) != HAL_OK)
  {
    Error_Handler();
  }
  __HAL_LTDC_LAYER_ENABLE(&hltdc, LTDC_LAYER_2);
  if (HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_IMMEDIATE) != HAL_OK)
  {
    Error_Handler();
  }
}

void LCD_OverlayPopupHide(void)
{
  lcd_wait_pipeline_idle();
  __HAL_LTDC_LAYER_DISABLE(&hltdc, LTDC_LAYER_2);
  if (HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_IMMEDIATE) != HAL_OK)
  {
    Error_Handler();
  }
}

static uint8_t lcd_clip_wh(uint16_t *x, uint16_t *y, uint16_t *w, uint16_t *h)
{
  if (*w == 0U || *h == 0U || *x >= LCD_DEV.width || *y >= LCD_DEV.height)
  {
    return 0U;
  }

  if ((uint32_t)*x + *w > LCD_DEV.width)
  {
    *w = (uint16_t)(LCD_DEV.width - *x);
  }
  if ((uint32_t)*y + *h > LCD_DEV.height)
  {
    *h = (uint16_t)(LCD_DEV.height - *y);
  }

  return (uint8_t)(*w != 0U && *h != 0U);
}

static uint8_t lcd_text_scale(uint8_t size)
{
  uint8_t scale = (uint8_t)(size / 8U);
  return (scale == 0U) ? 1U : scale;
}

static void lcd_draw_scaled_block(uint16_t x, uint16_t y, uint8_t scale, uint32_t color)
{
  LCD_FillRect(x, y, scale, scale, color);
}

static void lcd_draw_demo_char(uint16_t x, uint16_t y, char ch, uint8_t scale, uint16_t color)
{
  uint8_t row;

  for (row = 0U; row < 7U; ++row)
  {
    uint8_t bits = lcd_demo_font_row(ch, row);
    uint8_t col;

    for (col = 0U; col < 5U; ++col)
    {
      if ((bits & (uint8_t)(1U << (4U - col))) != 0U)
      {
        LCD_FillRect((uint16_t)(x + col * scale),
                     (uint16_t)(y + row * scale),
                     scale,
                     scale,
                     color);
      }
    }
  }
}

static void lcd_draw_demo_string(uint16_t x, uint16_t y, const char *str, uint8_t scale, uint16_t color)
{
  while (*str != '\0')
  {
    lcd_draw_demo_char(x, y, *str, scale, color);
    x = (uint16_t)(x + 6U * scale);
    ++str;
  }
}

static void lcd_draw_rect_outline(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
  if (w < 2U || h < 2U)
  {
    return;
  }

  LCD_FillRect(x, y, w, 1U, color);
  LCD_FillRect(x, (uint16_t)(y + h - 1U), w, 1U, color);
  LCD_FillRect(x, y, 1U, h, color);
  LCD_FillRect((uint16_t)(x + w - 1U), y, 1U, h, color);
}

void LCD_Init(void)
{
  if (ST7701Init() != HAL_OK)
  {
    Error_Handler();
  }

  lcd_backlight_apply_pwm();
  if (g_lcd_backlight_pwm_started == 0U)
  {
    if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4) != HAL_OK)
    {
      Error_Handler();
    }
    g_lcd_backlight_pwm_started = 1U;
  }

  g_lcd_front_buffer_addr = LCD_FRAMEBUFFER_ADDR;
  g_lcd_draw_buffer_addr = LCD_FRAMEBUFFER_BACK_ADDR;
  LCD_DEV.width = LCD_WIDTH;
  LCD_DEV.height = LCD_HEIGHT;
  LCD_DEV.front_addr = g_lcd_front_buffer_addr;
  LCD_DEV.draw_addr = g_lcd_draw_buffer_addr;
  POINT_COLOR = WHITE;
  BACK_COLOR = BLACK;

  g_lcd_dma_busy = 0U;
  g_lcd_dma_owner = LCD_DMA_OWNER_NONE;
  g_lcd_present_state = 0U;
  g_lcd_sync_count = 0U;
  LCD_ClearPerformanceCounters();
  LTDC->ICR = LTDC_ICR_CRRIF | LTDC_ICR_CFUIF | LTDC_ICR_CTERRIF;
  LTDC->IER |= LTDC_IER_FUIE | LTDC_IER_TERRIE;

  lcd_dma2d_fill_rgb565(g_lcd_front_buffer_addr, LCD_PHYS_WIDTH, LCD_PHYS_HEIGHT, 0U, BLACK);
  lcd_dma2d_fill_rgb565(g_lcd_draw_buffer_addr, LCD_PHYS_WIDTH, LCD_PHYS_HEIGHT, 0U, BLACK);
  lcd_dma2d_fill_rgb565(LCD_OVERLAY_FRAMEBUFFER_ADDR,
                        LCD_OVERLAY_WIDTH, LCD_OVERLAY_HEIGHT, 0U, BLACK);
  lcd_dma2d_fill_rgb565(LCD_SCRIM_FRAMEBUFFER_ADDR,
                        LCD_PHYS_WIDTH, LCD_PHYS_HEIGHT, 0U, BLACK);
  lcd_present_buffer_immediate(g_lcd_front_buffer_addr);
}

uint32_t LCD_GetFrontBufferAddress(void)
{
  return g_lcd_front_buffer_addr;
}

uint32_t LCD_GetDrawBufferAddress(void)
{
  return g_lcd_draw_buffer_addr;
}

void LCD_SetFrameBuffers(uint32_t front_addr, uint32_t draw_addr)
{
  lcd_wait_pipeline_idle();
  g_lcd_front_buffer_addr = front_addr;
  g_lcd_draw_buffer_addr = draw_addr;
  LCD_DEV.front_addr = front_addr;
  LCD_DEV.draw_addr = g_lcd_draw_buffer_addr;
}

void LCD_DrawPixelColor(uint16_t x, uint16_t y, uint32_t color)
{
  if (x >= LCD_DEV.width || y >= LCD_DEV.height)
  {
    return;
  }

  lcd_wait_pipeline_idle();
  *(uint16_t *)LCD_LL_PixelAddress(g_lcd_draw_buffer_addr, x, y) = (uint16_t)color;
}

void LCD_ShowChar(uint16_t x, uint16_t y, char ch, uint8_t size, uint8_t mode)
{
  uint8_t row;
  uint8_t scale = lcd_text_scale(size);

  for (row = 0U; row < 7U; ++row)
  {
    uint8_t bits = lcd_demo_font_row(ch, row);
    uint8_t col;

    for (col = 0U; col < 5U; ++col)
    {
      uint16_t px = (uint16_t)(x + col * scale);
      uint16_t py = (uint16_t)(y + row * scale);

      if ((bits & (uint8_t)(1U << (4U - col))) != 0U)
      {
        lcd_draw_scaled_block(px, py, scale, POINT_COLOR);
      }
      else if (mode != 0U)
      {
        lcd_draw_scaled_block(px, py, scale, BACK_COLOR);
      }
    }
  }
}

void LCD_TestShowText(const char *text)
{
  const uint8_t scale = 8U;
  const uint16_t char_w = 6U * scale;
  const uint16_t char_h = 8U * scale;
  uint16_t x = 8U;
  uint16_t y = 10U;

  LCD_Clear(BLACK);

  if (text != NULL)
  {
    while (*text != '\0')
    {
      uint8_t row;

      if ((uint32_t)x + char_w > LCD_WIDTH)
      {
        x = 8U;
        y = (uint16_t)(y + char_h);
      }
      if ((uint32_t)y + char_h > LCD_HEIGHT)
      {
        break;
      }

      for (row = 0U; row < 7U; ++row)
      {
        uint8_t bits = lcd_demo_font_row(*text, row);
        uint8_t col;

        for (col = 0U; col < 5U; ++col)
        {
          if ((bits & (uint8_t)(1U << (4U - col))) != 0U)
          {
            LCD_FillRect((uint16_t)(x + col * scale),
                         (uint16_t)(y + row * scale), scale, scale, WHITE);
          }
        }
      }

      x = (uint16_t)(x + char_w);
      ++text;
    }
  }

  LCD_CommitFrameFromDraw();
}

void LCD_Rect_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{
  uint16_t w;
  uint16_t h;
  uint16_t offline;
  uint32_t dst_addr;

  if (ex < sx || ey < sy)
  {
    return;
  }

  w = (uint16_t)(ex - sx + 1U);
  h = (uint16_t)(ey - sy + 1U);
  if (lcd_clip_wh(&sx, &sy, &w, &h) == 0U)
  {
    return;
  }

  offline = (uint16_t)(LCD_PHYS_WIDTH - w);
  dst_addr = LCD_LL_PixelAddress(g_lcd_draw_buffer_addr, sx, sy);
  lcd_dma2d_fill_rgb565(dst_addr, w, h, offline, (uint16_t)color);
}

void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color)
{
  if (w == 0U || h == 0U)
  {
    return;
  }

  LCD_Rect_Fill(x, y, (uint16_t)(x + w - 1U), (uint16_t)(y + h - 1U), color);
}

void LCD_Clear(uint32_t color)
{
  LCD_Rect_Fill(0U, 0U, (uint16_t)(LCD_DEV.width - 1U), (uint16_t)(LCD_DEV.height - 1U), color);
}

void LCD_BlitRGB565(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *pixels)
{
  uint16_t src_offline;
  uint16_t src_width = w;
  uint16_t offline;
  uint32_t dst_addr;

  if (pixels == NULL || lcd_clip_wh(&x, &y, &w, &h) == 0U)
  {
    return;
  }

  src_offline = (uint16_t)(src_width - w);
  offline = (uint16_t)(LCD_PHYS_WIDTH - w);
  dst_addr = LCD_LL_PixelAddress(g_lcd_draw_buffer_addr, x, y);
  lcd_dma2d_copy_rgb565((uint32_t)pixels, dst_addr, w, h, src_offline, offline);
}

void LCD_BlitRGB565FromAddr(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t src_addr, uint16_t src_stride)
{
  uint16_t dst_offline;
  if (src_addr == 0U || lcd_clip_wh(&x, &y, &w, &h) == 0U)
  {
    return;
  }

  dst_offline = (uint16_t)(LCD_PHYS_WIDTH - w);
  lcd_dma2d_copy_rgb565(src_addr, LCD_LL_PixelAddress(g_lcd_draw_buffer_addr, x, y), w, h,
                        (uint16_t)(src_stride - w), dst_offline);
}

void LCD_CopyRectFromDrawToAddr(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                               uint32_t dst_addr, uint16_t dst_stride)
{
  if (dst_addr == 0U || dst_stride < w || lcd_clip_wh(&x, &y, &w, &h) == 0U)
  {
    return;
  }

  lcd_dma2d_copy_rgb565(LCD_LL_PixelAddress(g_lcd_draw_buffer_addr, x, y), dst_addr,
                         w, h, (uint16_t)(LCD_PHYS_WIDTH - w),
                         (uint16_t)(dst_stride - w));
}

void LCD_DimRectRGB565(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t alpha)
{
  uint16_t offline;
  uint32_t fg_addr;
  uint32_t bg_addr;

  if (alpha == 0U || lcd_clip_wh(&x, &y, &w, &h) == 0U)
  {
    return;
  }

  lcd_wait_pipeline_idle();

  offline = (uint16_t)(LCD_PHYS_WIDTH - w);
  fg_addr = LCD_SCRIM_FRAMEBUFFER_ADDR +
            LCD_BYTES_PER_PIXEL * ((uint32_t)LCD_PHYS_WIDTH * y + x);
  bg_addr = LCD_LL_PixelAddress(g_lcd_draw_buffer_addr, x, y);

  LCD_LL_DMA2DClearFlags();
  g_lcd_dma_ok = 1U;
  g_lcd_dma_owner = LCD_DMA_OWNER_DRAW;
  g_lcd_dma_busy = 1U;
  LCD_LL_DMA2DStartBlendRgb565(fg_addr, bg_addr, bg_addr, w, h,
                               offline, offline, offline, alpha);
  lcd_wait_pipeline_idle();
}

void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, const char *str)
{
  uint16_t x0 = x;
  uint8_t scale = lcd_text_scale(size);
  uint16_t char_w = (uint16_t)(6U * scale);
  uint16_t char_h = (uint16_t)(8U * scale);
  uint32_t right = (uint32_t)x + width;
  uint32_t bottom = (uint32_t)y + height;

  if (str == NULL || width == 0U || height == 0U)
  {
    return;
  }

  while (*str != '\0')
  {
    if (*str == '\n')
    {
      x = x0;
      y = (uint16_t)(y + char_h);
      ++str;
      continue;
    }

    if ((uint32_t)x + char_w > right)
    {
      x = x0;
      y = (uint16_t)(y + char_h);
    }
    if ((uint32_t)y + char_h > bottom)
    {
      break;
    }

    LCD_ShowChar(x, y, *str, size, 0U);
    x = (uint16_t)(x + char_w);
    ++str;
  }
}

static void lcd_wait_pipeline_idle(void)
{
  uint32_t start = HAL_GetTick();

  while (g_lcd_dma_busy != 0U || g_lcd_present_state != 0U)
  {
    __WFI();
    if ((uint32_t)(HAL_GetTick() - start) > LCD_PIPELINE_TIMEOUT_MS)
    {
      ++g_lcd_counters.pipeline_timeouts;
      Error_Handler();
    }
  }

  if (g_lcd_dma_ok == 0U)
  {
    Error_Handler();
  }
}

void LCD_WaitForIdle(void)
{
  lcd_wait_pipeline_idle();
}

uint8_t LCD_IsPresentPending(void)
{
  return (uint8_t)(g_lcd_present_state != 0U);
}

static void lcd_start_sync_rect(uint8_t index)
{
  const volatile lcd_rect_t *rect = &g_lcd_sync_rects[index];
  uint16_t offline = (uint16_t)(LCD_PHYS_WIDTH - rect->w);

  LCD_LL_DMA2DClearFlags();
  g_lcd_dma_ok = 1U;
  g_lcd_dma_owner = LCD_DMA_OWNER_PRESENT_SYNC;
  g_lcd_dma_busy = 1U;
  LCD_LL_DMA2DStartCopyRgb565(
      LCD_LL_PixelAddress(g_lcd_front_buffer_addr, rect->x, rect->y),
      LCD_LL_PixelAddress(g_lcd_draw_buffer_addr, rect->x, rect->y),
      rect->w, rect->h, offline, offline);
}

static void lcd_request_present(uint32_t buffer_addr, uint8_t swap_buffers)
{
  lcd_wait_pipeline_idle();
  g_lcd_pending_buffer_addr = buffer_addr;
  g_lcd_present_swap = swap_buffers;

  if (LCD_LL_LTDCSetAddressNoReload(buffer_addr) != HAL_OK)
  {
    Error_Handler();
  }

  g_lcd_present_state = 1U;
  __DSB();
  LTDC->ICR = LTDC_ICR_CRRIF;
  LTDC->IER |= LTDC_IER_RRIE;
  LTDC->SRCR = LTDC_SRCR_VBR;
}

void LCD_PresentBuffer(uint32_t buffer_addr)
{
  lcd_wait_pipeline_idle();
  g_lcd_sync_count = 0U;
  lcd_request_present(buffer_addr, (uint8_t)(buffer_addr == g_lcd_draw_buffer_addr));
}

void LCD_Present(void)
{
  LCD_CommitFrameFromDraw();
}

void LCD_PresentImmediate(void)
{
  uint32_t old_front_addr;

  lcd_wait_pipeline_idle();
  lcd_present_buffer_immediate(g_lcd_draw_buffer_addr);

  old_front_addr = g_lcd_front_buffer_addr;
  g_lcd_front_buffer_addr = g_lcd_draw_buffer_addr;
  g_lcd_draw_buffer_addr = old_front_addr;
  LCD_DEV.front_addr = g_lcd_front_buffer_addr;
  LCD_DEV.draw_addr = g_lcd_draw_buffer_addr;
}

void LCD_CommitFrameFromDraw(void)
{
  const lcd_rect_t full = {0U, 0U, LCD_PHYS_WIDTH, LCD_PHYS_HEIGHT};
  LCD_CommitRectsFromDraw(&full, 1U);
}

void LCD_CommitFrameFromDrawNoSync(void)
{
  lcd_wait_pipeline_idle();
  g_lcd_sync_count = 0U;
  lcd_request_present(g_lcd_draw_buffer_addr, 1U);
}

void LCD_SyncRectToFront(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  uint16_t offline;
  uint32_t src_addr;
  uint32_t dst_addr;

  if (lcd_clip_wh(&x, &y, &w, &h) == 0U)
  {
    return;
  }

  offline = (uint16_t)(LCD_PHYS_WIDTH - w);
  src_addr = LCD_LL_PixelAddress(g_lcd_draw_buffer_addr, x, y);
  dst_addr = LCD_LL_PixelAddress(g_lcd_front_buffer_addr, x, y);
  lcd_dma2d_copy_rgb565(src_addr, dst_addr, w, h, offline, offline);
}

void LCD_SyncRectFromFront(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  uint16_t offline;
  uint32_t src_addr;
  uint32_t dst_addr;

  if (lcd_clip_wh(&x, &y, &w, &h) == 0U)
  {
    return;
  }

  offline = (uint16_t)(LCD_PHYS_WIDTH - w);
  src_addr = LCD_LL_PixelAddress(g_lcd_front_buffer_addr, x, y);
  dst_addr = LCD_LL_PixelAddress(g_lcd_draw_buffer_addr, x, y);
  lcd_dma2d_copy_rgb565(src_addr, dst_addr, w, h, offline, offline);
}

void LCD_CommitRectFromDraw(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  const lcd_rect_t rect = {x, y, w, h};
  LCD_CommitRectsFromDraw(&rect, 1U);
}

void LCD_CommitRectsFromDraw(const lcd_rect_t *rects, uint8_t count)
{
  uint8_t i;

  if (rects == NULL || count == 0U)
  {
    return;
  }

  lcd_wait_pipeline_idle();
  g_lcd_sync_count = 0U;
  if (count > LCD_MAX_DIRTY_RECTS)
  {
    g_lcd_sync_rects[0].x = 0U;
    g_lcd_sync_rects[0].y = 0U;
    g_lcd_sync_rects[0].w = LCD_PHYS_WIDTH;
    g_lcd_sync_rects[0].h = LCD_PHYS_HEIGHT;
    g_lcd_sync_count = 1U;
  }
  for (i = 0U; i < count && count <= LCD_MAX_DIRTY_RECTS; ++i)
  {
    lcd_rect_t rect = rects[i];
    if (lcd_clip_wh(&rect.x, &rect.y, &rect.w, &rect.h) != 0U)
    {
      g_lcd_sync_rects[g_lcd_sync_count++] = rect;
    }
  }

  if (g_lcd_sync_count != 0U)
  {
    g_lcd_sync_index = 0U;
    lcd_request_present(g_lcd_draw_buffer_addr, 1U);
  }
}

void LCD_DMA2D_IRQHandler(void)
{
  uint32_t status = DMA2D->ISR;
  uint8_t owner = g_lcd_dma_owner;
  uint8_t ok = (uint8_t)((status & DMA2D_ISR_TCIF) != 0U &&
                         (status & (DMA2D_ISR_TEIF | DMA2D_ISR_CEIF | DMA2D_ISR_CAEIF)) == 0U);

  LCD_LL_DMA2DClearFlags();
  g_lcd_dma_ok = ok;
  g_lcd_dma_busy = 0U;

  if (ok == 0U)
  {
    ++g_lcd_counters.dma2d_transfer_errors;
    g_lcd_dma_owner = LCD_DMA_OWNER_NONE;
    g_lcd_present_state = 0U;
    return;
  }

  if (owner == LCD_DMA_OWNER_PRESENT_SYNC)
  {
    ++g_lcd_sync_index;
    if (g_lcd_sync_index < g_lcd_sync_count)
    {
      lcd_start_sync_rect(g_lcd_sync_index);
      return;
    }
    g_lcd_present_state = 0U;
  }

  g_lcd_dma_owner = LCD_DMA_OWNER_NONE;
}

void LCD_LTDC_IRQHandler(void)
{
  if ((LTDC->ISR & LTDC_ISR_RRIF) != 0U)
  {
    uint32_t old_front_addr;

    LTDC->ICR = LTDC_ICR_CRRIF;
    LTDC->IER &= ~LTDC_IER_RRIE;
    if (g_lcd_present_state != 1U)
    {
      return;
    }

    if (g_lcd_present_swap != 0U)
    {
      old_front_addr = g_lcd_front_buffer_addr;
      g_lcd_front_buffer_addr = g_lcd_pending_buffer_addr;
      g_lcd_draw_buffer_addr = old_front_addr;
      LCD_DEV.front_addr = g_lcd_front_buffer_addr;
      LCD_DEV.draw_addr = g_lcd_draw_buffer_addr;
    }
    ++g_lcd_counters.frames_presented;

    if (g_lcd_sync_count != 0U && g_lcd_present_swap != 0U)
    {
      g_lcd_present_state = 2U;
      lcd_start_sync_rect(0U);
    }
    else
    {
      g_lcd_present_state = 0U;
    }
  }
}

void LCD_LTDC_ER_IRQHandler(void)
{
  uint32_t status = LTDC->ISR;
  uint32_t clear = 0U;

  if ((status & LTDC_ISR_FUIF) != 0U)
  {
    ++g_lcd_counters.ltdc_fifo_underruns;
    clear |= LTDC_ICR_CFUIF;
  }
  if ((status & LTDC_ISR_TERRIF) != 0U)
  {
    ++g_lcd_counters.ltdc_transfer_errors;
    clear |= LTDC_ICR_CTERRIF;
  }
  LTDC->ICR = clear;
}

void LCD_GetPerformanceCounters(lcd_performance_counters_t *counters)
{
  uint32_t primask;

  if (counters == NULL)
  {
    return;
  }
  primask = __get_PRIMASK();
  __disable_irq();
  counters->frames_presented = g_lcd_counters.frames_presented;
  counters->ltdc_fifo_underruns = g_lcd_counters.ltdc_fifo_underruns;
  counters->ltdc_transfer_errors = g_lcd_counters.ltdc_transfer_errors;
  counters->dma2d_transfer_errors = g_lcd_counters.dma2d_transfer_errors;
  counters->pipeline_timeouts = g_lcd_counters.pipeline_timeouts;
  __set_PRIMASK(primask);
}

void LCD_ClearPerformanceCounters(void)
{
  uint32_t primask = __get_PRIMASK();

  __disable_irq();
  g_lcd_counters.frames_presented = 0U;
  g_lcd_counters.ltdc_fifo_underruns = 0U;
  g_lcd_counters.ltdc_transfer_errors = 0U;
  g_lcd_counters.dma2d_transfer_errors = 0U;
  g_lcd_counters.pipeline_timeouts = 0U;
  __set_PRIMASK(primask);
}

void LCD_DrawOverlayDialogDemo(void)
{
  lcd_overlay_fill_rect(0U, 0U, LCD_OVERLAY_WIDTH, LCD_OVERLAY_HEIGHT, 0x2104U);
  lcd_overlay_fill_rect(44U, 54U, 272U, 176U, WHITE);
  lcd_overlay_draw_rect(44U, 54U, 272U, 176U, 0x39E7U);
  lcd_overlay_fill_rect(44U, 54U, 272U, 34U, 0x0451U);
  lcd_overlay_draw_string(70U, 66U, "UF4 DIALOG", 2U, WHITE);

  lcd_overlay_fill_rect(64U, 112U, 232U, 2U, 0xD69AU);
  lcd_overlay_draw_string(82U, 134U, "LAYER 1", 3U, 0x2104U);

  lcd_overlay_fill_rect(128U, 188U, 104U, 30U, 0x0451U);
  lcd_overlay_draw_rect(128U, 188U, 104U, 30U, 0x0330U);
  lcd_overlay_draw_string(168U, 196U, "OK", 2U, WHITE);

  if (LCD_LL_LTDCReloadVBlank() != HAL_OK)
  {
    Error_Handler();
  }
}

void LCD_DrawMainUiTestControl(void)
{
  const uint16_t bg = 0xF7BEU;          /* #f6f6f2 */
  const uint16_t surface = 0xFFFFU;     /* #ffffff */
  const uint16_t ink = 0x0861U;         /* #111111 */
  const uint16_t muted = 0x5AEBU;       /* #5d5d55 */
  const uint16_t quiet = 0x8C51U;       /* #8a8a80 */
  const uint16_t line = 0xAD75U;        /* light weak line */
  const uint16_t accent = 0xD7E7U;      /* #d9ff3f */

  LCD_Clear(bg);

  LCD_FillRect(0U, 0U, 480U, 72U, bg);
  LCD_FillRect(0U, 71U, 480U, 1U, line);

  LCD_FillRect(24U, 16U, 72U, 40U, bg);
  lcd_draw_rect_outline(24U, 16U, 72U, 40U, ink);
  lcd_draw_demo_string(43U, 29U, "UF4", 2U, ink);

  LCD_FillRect(126U, 17U, 54U, 38U, ink);
  lcd_draw_demo_string(142U, 31U, "HOME", 1U, bg);
  lcd_draw_rect_outline(190U, 17U, 58U, 38U, ink);
  lcd_draw_demo_string(205U, 31U, "BLOG", 1U, muted);
  lcd_draw_rect_outline(258U, 17U, 72U, 38U, ink);
  lcd_draw_demo_string(272U, 31U, "PROJECT", 1U, muted);
  lcd_draw_rect_outline(340U, 17U, 62U, 38U, ink);
  lcd_draw_demo_string(355U, 31U, "NOTES", 1U, muted);
  lcd_draw_rect_outline(412U, 17U, 44U, 38U, ink);
  LCD_FillRect(428U, 30U, 12U, 12U, accent);
  lcd_draw_rect_outline(428U, 30U, 12U, 12U, ink);

  LCD_FillRect(0U, 72U, 480U, 1U, ink);
  LCD_FillRect(252U, 72U, 1U, 610U, ink);

  lcd_draw_demo_string(28U, 128U, "OPEN SOURCE / CONTROL / POWER", 1U, quiet);
  lcd_draw_demo_string(24U, 184U, "UF4", 12U, ink);
  LCD_FillRect(26U, 316U, 176U, 4U, accent);

  lcd_draw_demo_string(28U, 354U, "EMBEDDED SYSTEMS", 2U, ink);
  lcd_draw_demo_string(28U, 386U, "DIGITAL POWER", 2U, muted);
  lcd_draw_demo_string(28U, 418U, "DESKTOP TOOLING", 2U, muted);

  LCD_FillRect(28U, 488U, 132U, 42U, ink);
  lcd_draw_demo_string(47U, 502U, "PROJECTS", 2U, bg);
  lcd_draw_rect_outline(174U, 488U, 54U, 42U, ink);
  lcd_draw_demo_string(189U, 502U, "GIT", 2U, ink);

  lcd_draw_rect_outline(278U, 118U, 164U, 158U, ink);
  LCD_FillRect(302U, 146U, 116U, 1U, ink);
  LCD_FillRect(302U, 178U, 116U, 1U, ink);
  LCD_FillRect(302U, 210U, 116U, 1U, ink);
  LCD_FillRect(302U, 242U, 116U, 1U, ink);
  LCD_FillRect(318U, 130U, 1U, 128U, line);
  LCD_FillRect(350U, 130U, 1U, 128U, line);
  LCD_FillRect(382U, 130U, 1U, 128U, line);
  LCD_FillRect(414U, 130U, 1U, 128U, line);
  LCD_FillRect(334U, 162U, 68U, 68U, accent);
  lcd_draw_rect_outline(334U, 162U, 68U, 68U, ink);
  lcd_draw_demo_string(354U, 188U, "UF4", 2U, ink);

  LCD_FillRect(278U, 320U, 164U, 132U, surface);
  lcd_draw_rect_outline(278U, 320U, 164U, 132U, ink);
  lcd_draw_demo_string(294U, 340U, "UF4.LOGIC", 1U, quiet);
  lcd_draw_demo_string(294U, 372U, "STRONG", 2U, ink);
  lcd_draw_demo_string(294U, 402U, "LINES", 2U, ink);
  LCD_FillRect(294U, 432U, 116U, 3U, accent);

  lcd_draw_rect_outline(278U, 488U, 72U, 72U, ink);
  lcd_draw_demo_string(295U, 515U, "MCU", 2U, ink);
  LCD_FillRect(370U, 488U, 72U, 72U, ink);
  lcd_draw_demo_string(388U, 515U, "PWR", 2U, bg);

  LCD_FillRect(24U, 682U, 432U, 1U, ink);
  lcd_draw_demo_string(24U, 714U, "PROFILE", 1U, muted);
  LCD_FillRect(112U, 718U, 48U, 2U, accent);
  lcd_draw_demo_string(184U, 714U, "BLOG", 1U, muted);
  LCD_FillRect(242U, 718U, 48U, 2U, accent);
  lcd_draw_demo_string(314U, 714U, "GITHUB", 1U, muted);
}
