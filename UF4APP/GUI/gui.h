/**
  ******************************************************************************
  * @file    gui.h
  * @author  UF4
  * @date    26-6-21 下午9:57
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
#ifndef GUI_H
#define GUI_H

#include <stdint.h>

typedef struct
{
        float vin;
        float iin;
        float pin;
        float efficiency;
        float fan;

        float vout;
        float iout;
        float pout;

        float vset;
        float iset;
        uint8_t vset_digit;
        uint8_t iset_digit;

        float cpu_temp;
        float buck_temp;
        float boost_temp;

        float power;
        uint16_t fps;
        uint8_t cpu_usage;

} GUI_Data_t;

void GUI_Init(void);

void GUI_Clear(void);

void GUI_DrawStatic(void);

void GUI_Update(const GUI_Data_t *data);
#endif //GUI_H
