/**
  ******************************************************************************
  * @file    bap_font.h
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
#ifndef STM32H743_BSP_LCD_FONT_H
#define STM32H743_BSP_LCD_FONT_H

#include "Teko_SemiBold_Index.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_FONT_BG_TRANSPARENT 0xFFFFFFFFUL

  uint16_t LCD_GetFontAdvance(uint8_t ch, uint16_t size);
  uint16_t LCD_MeasureFontString(const char *str, uint16_t size);

  void LCD_RenderFontStringToBuffer(
          uint16_t *buffer,
          uint16_t width,
          uint16_t height,
          uint16_t x,
          uint16_t y,
          const char *str,
          uint16_t size,
          uint32_t color);

  void LCD_DrawFontStringDMA(
          uint16_t x,
          uint16_t y,
          uint16_t w,
          uint16_t h,
          const char *str,
          uint16_t size,
          uint32_t color,
          uint32_t bg_color);

  void LCD_DrawFontChar(
          uint16_t x,
          uint16_t y,
          char ch,
          uint16_t size,
          uint32_t color,
          uint32_t bg_color);

  void LCD_DrawFontString(
          uint16_t x,
          uint16_t y,
          const char *str,
          uint16_t size,
          uint32_t color,
          uint32_t bg_color);

#ifdef __cplusplus
}
#endif


#endif //BAP_FONT_H
