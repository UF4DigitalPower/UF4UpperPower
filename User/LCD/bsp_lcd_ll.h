/**
  ******************************************************************************
  * @file    bsp_lcd_ll.h
  * @author  UF4
  * @date    26-8-14
  * @brief   Low-level LTDC/DMA2D helpers for the display driver.
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

#ifndef __BSP_LCD_LL_H__
#define __BSP_LCD_LL_H__

#include "bsp_lcd.h"
#include "ltdc.h"

#include <stdint.h>

#ifdef __GNUC__
#define LCD_LL_FORCE_INLINE static inline __attribute__((always_inline))
#else
#define LCD_LL_FORCE_INLINE static inline
#endif

LCD_LL_FORCE_INLINE uint32_t LCD_LL_PixelAddress(uint32_t base_addr, uint16_t x, uint16_t y)
{
  return base_addr + LCD_BYTES_PER_PIXEL * ((uint32_t)LCD_PHYS_WIDTH * y + x);
}

LCD_LL_FORCE_INLINE void LCD_LL_DMA2DClearFlags(void)
{
  DMA2D->IFCR = DMA2D_IFCR_CTCIF |
                DMA2D_IFCR_CTEIF |
                DMA2D_IFCR_CCEIF |
                DMA2D_IFCR_CCTCIF |
                DMA2D_IFCR_CAECIF |
                DMA2D_IFCR_CTWIF;
}

LCD_LL_FORCE_INLINE void LCD_LL_DMA2DStartFillRgb565(uint32_t dst_addr,
                                                     uint16_t width,
                                                     uint16_t height,
                                                     uint16_t dst_offline,
                                                     uint16_t color)
{
  __HAL_RCC_DMA2D_CLK_ENABLE();
  DMA2D->AMTCR = DMA2D_AMTCR_EN | (0x20UL << DMA2D_AMTCR_DT_Pos);
  DMA2D->CR = DMA2D_R2M | DMA2D_CR_TCIE | DMA2D_CR_TEIE |
              DMA2D_CR_CEIE | DMA2D_CR_CAEIE;
  DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;
  DMA2D->OOR = dst_offline;
  DMA2D->OMAR = dst_addr;
  DMA2D->NLR = (uint32_t)height | ((uint32_t)width << 16);
  DMA2D->OCOLR = color;
  DMA2D->CR |= DMA2D_CR_START;
}

LCD_LL_FORCE_INLINE void LCD_LL_DMA2DStartCopyRgb565(uint32_t src_addr,
                                                     uint32_t dst_addr,
                                                     uint16_t width,
                                                     uint16_t height,
                                                     uint16_t src_offline,
                                                     uint16_t dst_offline)
{
  __HAL_RCC_DMA2D_CLK_ENABLE();
  DMA2D->AMTCR = DMA2D_AMTCR_EN | (0x20UL << DMA2D_AMTCR_DT_Pos);
  DMA2D->CR = DMA2D_M2M | DMA2D_CR_TCIE | DMA2D_CR_TEIE |
              DMA2D_CR_CEIE | DMA2D_CR_CAEIE;
  DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_RGB565;
  DMA2D->FGOR = src_offline;
  DMA2D->OOR = dst_offline;
  DMA2D->FGMAR = src_addr;
  DMA2D->OMAR = dst_addr;
  DMA2D->NLR = (uint32_t)height | ((uint32_t)width << 16);
  DMA2D->CR |= DMA2D_CR_START;
}

LCD_LL_FORCE_INLINE void LCD_LL_DMA2DStartBlendRgb565(uint32_t fg_addr,
                                                      uint32_t bg_addr,
                                                      uint32_t dst_addr,
                                                      uint16_t width,
                                                      uint16_t height,
                                                      uint16_t fg_offline,
                                                      uint16_t bg_offline,
                                                      uint16_t dst_offline,
                                                      uint8_t alpha)
{
  __HAL_RCC_DMA2D_CLK_ENABLE();
  DMA2D->AMTCR = DMA2D_AMTCR_EN | (0x20UL << DMA2D_AMTCR_DT_Pos);
  DMA2D->CR = (2UL << DMA2D_CR_MODE_Pos) |             /* memory-to-memory with blending */
              DMA2D_CR_TCIE | DMA2D_CR_TEIE | DMA2D_CR_CEIE | DMA2D_CR_CAEIE;
  DMA2D->OPFCCR = LTDC_PIXEL_FORMAT_RGB565;
  DMA2D->FGPFCCR = (2UL << DMA2D_FGPFCCR_CM_Pos)       /* FG RGB565 */
                 | (1UL << DMA2D_FGPFCCR_AM_Pos)       /* replace FG alpha */
                 | ((uint32_t)alpha << 24U);            /* constant alpha */
  DMA2D->BGPFCCR = (2UL << DMA2D_BGPFCCR_CM_Pos);      /* BG RGB565 */
  DMA2D->FGMAR = fg_addr;
  DMA2D->BGMAR = bg_addr;
  DMA2D->OMAR = dst_addr;
  DMA2D->FGOR = fg_offline;
  DMA2D->BGOR = bg_offline;
  DMA2D->OOR = dst_offline;
  DMA2D->NLR = (uint32_t)height | ((uint32_t)width << 16);
  DMA2D->CR |= DMA2D_CR_START;
}

LCD_LL_FORCE_INLINE HAL_StatusTypeDef LCD_LL_LTDCSetAddressNoReload(uint32_t buffer_addr)
{
  return HAL_LTDC_SetAddress_NoReload(&hltdc, buffer_addr, 0);
}

LCD_LL_FORCE_INLINE HAL_StatusTypeDef LCD_LL_LTDCReloadImmediate(void)
{
  return HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_IMMEDIATE);
}

LCD_LL_FORCE_INLINE HAL_StatusTypeDef LCD_LL_LTDCReloadVBlank(void)
{
  return HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_VERTICAL_BLANKING);
}

#endif /* __BSP_LCD_LL_H__ */
