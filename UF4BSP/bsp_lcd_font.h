/**
 * @file    : bsp_lcd_font.h
 * @brief   : LCD bitmap font drawing
 */

#ifndef STM32H743_BSP_LCD_FONT_H
#define STM32H743_BSP_LCD_FONT_H

#include "Teko_SemiBold_Config.h"

#define LCD_FONT_BG_TRANSPARENT 0xFFFFFFFFUL

void LCD_DrawFontChar(uint16_t x, uint16_t y, char ch, LCD_FontSize size, uint32_t color, uint32_t bg_color);
void LCD_DrawFontString(uint16_t x, uint16_t y, const char *str, LCD_FontSize size, uint32_t color, uint32_t bg_color);

#endif /* STM32H743_BSP_LCD_FONT_H */
