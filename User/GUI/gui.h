/**
  ******************************************************************************
  * @file    gui.h
  * @author  UF4
  * @date    26-8-13 下午9:34
  * @brief   Fixed-pixel UI public interface for the UF4 digital power supply.
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
#ifndef GUI_H
#define GUI_H

#include <stdint.h>

void GUI_Init(void);
/* Call from the GUI timer interrupt only. */
void GUI_Tick(void);
/* Call continuously from the main loop; handles input and display updates. */
void GUI_Handler(void);
/* Call from the touch EXTI callback.  It only records the event; I2C access
   and UI updates remain in the main-loop GUI handler. */
void GUI_TouchIrqNotify(void);
void GUI_PopupShow(const char *title, const char *message, uint32_t duration_ms);
void GUI_PopupHide(void);

void DrawINArea(void);
void DrawOUTArea(void);
void DrawChartArea(void);

/* Parameter / setting accessors (by register id). */
uint8_t GUI_ParamGet(uint16_t id, float *value);
uint8_t GUI_ParamSet(uint16_t id, float value);
uint8_t GUI_SettingGet(uint16_t id, uint8_t *value);
uint8_t GUI_SettingSet(uint16_t id, uint8_t value);

#endif //GUI_H
