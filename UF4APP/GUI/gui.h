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

        float ovp;
        float ocp;
        float otp;
        uint8_t ovp_enabled;
        uint8_t ocp_enabled;
        uint8_t otp_enabled;
        uint8_t output_enabled;
        uint8_t ble_state;
        uint8_t panel_field;
        uint8_t comm_state;
        uint8_t comm_stream_enabled;
        uint8_t comm_last_tx_cmd;
        uint8_t comm_last_tx_ok;
        uint32_t comm_tx_ok_count;
        uint32_t comm_tx_fail_count;
        uint32_t comm_rx_frame_count;
        uint32_t comm_rx_error_count;

        float cpu_temp;
        float buck_temp;
        float boost_temp;

        float power;

} GUI_Data_t;

void GUI_Init(void);

void GUI_Clear(void);

void GUI_DrawStatic(void);

void GUI_Update(const GUI_Data_t *data);
#endif //GUI_H
