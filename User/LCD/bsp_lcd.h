#ifndef __BSP_LCD_H__
#define __BSP_LCD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

#define LCD_PHYS_WIDTH               480U
#define LCD_PHYS_HEIGHT              800U
#define LCD_DOUBLE_BUFFER            1U
#define LCD_OVERLAY_WIDTH            360U
#define LCD_OVERLAY_HEIGHT           300U
#define LCD_POPUP_WIDTH              236U
#define LCD_POPUP_HEIGHT              84U
#define LCD_MAX_DIRTY_RECTS            8U

#define LCD_WIDTH                    LCD_PHYS_WIDTH
#define LCD_HEIGHT                   LCD_PHYS_HEIGHT

#define LCD_BYTES_PER_PIXEL          2U
#define LCD_FRAMEBUFFER_PIXELS       ((uint32_t)LCD_PHYS_WIDTH * (uint32_t)LCD_PHYS_HEIGHT)
#define LCD_FRAMEBUFFER_BYTES        (LCD_FRAMEBUFFER_PIXELS * LCD_BYTES_PER_PIXEL)
#define LCD_OVERLAY_FRAMEBUFFER_BYTES ((uint32_t)LCD_OVERLAY_WIDTH * (uint32_t)LCD_OVERLAY_HEIGHT * LCD_BYTES_PER_PIXEL)

/* STM32F429 SDRAM bank 2 is mapped at 0xD0000000. */
#define LCD_FRAMEBUFFER_ADDR         0xD0000000UL
#define LCD_FRAMEBUFFER_BACK_ADDR    (LCD_FRAMEBUFFER_ADDR + LCD_FRAMEBUFFER_BYTES)
#define LCD_OVERLAY_FRAMEBUFFER_ADDR (LCD_FRAMEBUFFER_BACK_ADDR + LCD_FRAMEBUFFER_BYTES)
#define LCD_PAGE_CACHE_ADDR          (LCD_OVERLAY_FRAMEBUFFER_ADDR + LCD_OVERLAY_FRAMEBUFFER_BYTES)
#define LCD_PAGE_CACHE_BYTES         (LCD_FRAMEBUFFER_BYTES)
#define LCD_PAGE_CACHE0_ADDR         (LCD_PAGE_CACHE_ADDR)
#define LCD_PAGE_CACHE1_ADDR         (LCD_PAGE_CACHE0_ADDR + LCD_PAGE_CACHE_BYTES)
#define LCD_PAGE_CACHE2_ADDR         (LCD_PAGE_CACHE1_ADDR + LCD_PAGE_CACHE_BYTES)
#define LCD_SCRIM_FRAMEBUFFER_ADDR    0xD0400000UL
#define LCD_SCRIM_FRAMEBUFFER_BYTES   (LCD_FRAMEBUFFER_BYTES)
#define LCD_POPUP_BACKUP_ADDR         0xD0500000UL
#define LCD_PAGE_CACHE3_ADDR          0xD0600000UL

#define WHITE      0xFFFFU
#define BLACK      0x0000U
#define BLUE       0x001FU
#define BRED       0xF81FU
#define GRED       0xFFE0U
#define GBLUE      0x07FFU
#define RED        0xF800U
#define MAGENTA    0xF81FU
#define GREEN      0x07E0U
#define CYAN       0x7FFFU
#define YELLOW     0xFFE0U
#define GRAY       0x8430U
#define LIGHTGRAY  0xEF5BU

typedef struct {
  uint16_t width;
  uint16_t height;
  uint32_t front_addr;
  uint32_t draw_addr;
} lcd_dev;

typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;
} lcd_rect_t;

typedef struct {
  uint32_t frames_presented;
  uint32_t ltdc_fifo_underruns;
  uint32_t ltdc_transfer_errors;
  uint32_t dma2d_transfer_errors;
  uint32_t pipeline_timeouts;
} lcd_performance_counters_t;

extern volatile lcd_dev LCD_DEV;
extern uint32_t POINT_COLOR;
extern uint32_t BACK_COLOR;

void LCD_Init(void);
void LCD_Clear(uint32_t color);
void LCD_Rect_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);
void LCD_BlitRGB565(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *pixels);
void LCD_BlitRGB565FromAddr(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t src_addr, uint16_t src_stride);
void LCD_CopyRectFromDrawToAddr(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                               uint32_t dst_addr, uint16_t dst_stride);
void LCD_DimRectRGB565(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t alpha);
void LCD_DrawPixelColor(uint16_t x, uint16_t y, uint32_t color);
void LCD_ShowChar(uint16_t x, uint16_t y, char ch, uint8_t size, uint8_t mode);
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, const char *str);
void LCD_TestShowText(const char *text);
void LCD_Present(void);
void LCD_PresentImmediate(void);
void LCD_PresentBuffer(uint32_t buffer_addr);
void LCD_CommitFrameFromDraw(void);
void LCD_CommitFrameFromDrawNoSync(void);
void LCD_SyncRectToFront(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void LCD_SyncRectFromFront(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void LCD_CommitRectFromDraw(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void LCD_CommitRectsFromDraw(const lcd_rect_t *rects, uint8_t count);
uint8_t LCD_IsPresentPending(void);
void LCD_WaitForIdle(void);
void LCD_GetPerformanceCounters(lcd_performance_counters_t *counters);
void LCD_ClearPerformanceCounters(void);
void LCD_DMA2D_IRQHandler(void);
void LCD_LTDC_IRQHandler(void);
void LCD_LTDC_ER_IRQHandler(void);
void LCD_SetFrameBuffers(uint32_t front_addr, uint32_t draw_addr);
void LCD_OverlayPopupShow(uint16_t x, uint16_t y, const char *title, const char *message);
void LCD_OverlayPopupHide(void);
void LCD_DrawOverlayDialogDemo(void);
void LCD_DrawMainUiTestControl(void);
void LCD_SetBacklightLevel(uint8_t level);
uint8_t LCD_GetBacklightLevel(void);

uint32_t LCD_GetFrontBufferAddress(void);
uint32_t LCD_GetDrawBufferAddress(void);
#ifdef __cplusplus
}
#endif

#endif /* __BSP_LCD_H__ */
