/*
 * Copyright 2026 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "custom_internal.h"

const palette_t palettes[] = {
    {0xF7BD, 0x1082, 0xD7E8, 0xFFFF, 0x6B4C, 0xE73B, 0x1082, 0x1082},
    {0xFBFF, 0x0861, 0x9E6D, 0xFFFF, 0x7BEF, 0xE73B, 0x0861, 0x0861},
    {0x1082, 0xF7BD, 0xD7E8, 0x2104, 0xC638, 0x39E7, 0xF7BD, 0xF7BD},
    {0x1A69, 0xFFFF, 0x07FF, 0x294A, 0x6B4D, 0x4208, 0x07FF, 0xFFFF},
    {0xD6FC, 0x3186, 0xFBE0, 0xFFDF, 0x8C71, 0xC618, 0x3186, 0x3186},
    {0xDFF3, 0x0468, 0x07E0, 0xFFFF, 0x6C8D, 0xCE7B, 0x0468, 0x0468},
    {0xE72A, 0x2A08, 0xFD20, 0xFFF7, 0xA534, 0xD69A, 0x2A08, 0x2A08},
    {0xFEF5, 0x2124, 0xFD60, 0xFFFF, 0x8C51, 0xDEDB, 0x2124, 0x2124},
};

param_t params[PARAM_COUNT] = {
    {0x22, "OCP", 10.0f, 0.01f, 10.0f, "A", 2},
    {0x1E, "OTP", 120.0f, 40.0f, 150.0f, "C", 2},
    {0x30, "VIN_OVP", 75.0f, 0.01f, 100.0f, "V", 2},
    {0x31, "VIN_UVP", 5.0f, 0.0f, 100.0f, "V", 2},
    {0x32, "VOUT_OVP", 75.0f, 0.01f, 100.0f, "V", 2},
    {0x4F, "DIR_MODE", 1.0f, 0.0f, 1.0f, "BOOL", 0},
    {0x33, "ADC_VREF", 3.3f, 2.5f, 3.6f, "V", 2},
    {0x34, "VIN_R_UP", 750.0f, 1.0f, 65535.0f, "100R", 0},
    {0x35, "VIN_R_LOW", 30.0f, 1.0f, 65535.0f, "100R", 0},
    {0x36, "VOUT_R_UP", 750.0f, 1.0f, 65535.0f, "100R", 0},
    {0x37, "VOUT_R_LOW", 30.0f, 1.0f, 65535.0f, "100R", 0},
    {0x38, "SHUNT_IN", 800.0f, 1.0f, 65535.0f, "10UR", 0},
    {0x39, "SHUNT_OUT", 800.0f, 1.0f, 65535.0f, "10UR", 0},
    {0x3A, "GAIN_IN", 1.0f, 0.01f, 10.0f, "X", 2},
    {0x3B, "GAIN_OUT", 1.0f, 0.01f, 10.0f, "X", 2},
    {0x3C, "I_OFFSET", 0.0f, -327.68f, 327.67f, "A", 2},
    {0x3D, "VIN_CAL_GAIN", 1.0f, 0.10f, 3.0f, "X", 2},
    {0x3E, "VIN_CAL_OFF", 0.0f, -327.68f, 327.67f, "V", 2},
    {0x3F, "VOUT_CAL_GAIN", 1.0f, 0.10f, 3.0f, "X", 2},
    {0x40, "VOUT_CAL_OFF", 0.0f, -327.68f, 327.67f, "V", 2},
    {0x41, "IIN_CAL_GAIN", 1.0f, 0.10f, 3.0f, "X", 2},
    {0x42, "IIN_CAL_OFF", 0.0f, -327.68f, 327.67f, "A", 2},
    {0x43, "IOUT_CAL_GAIN", 1.0f, 0.10f, 3.0f, "X", 2},
    {0x44, "IOUT_CAL_OFF", 0.0f, -327.68f, 327.67f, "A", 2},
    {0x48, "CURRENT_KP", 0.0f, 0.0f, 6.5535f, "GAIN", 4},
    {0x49, "CURRENT_KI", 0.0f, 0.0f, 6.5535f, "GAIN", 4},
    {0x4A, "CURRENT_KD", 0.0f, 0.0f, 0.0100f, "GAIN", 4},
    {0x51, "VOLTAGE_KP", 1.0f, 0.0f, 6.5535f, "GAIN", 4},
    {0x52, "VOLTAGE_KI", 0.0f, 0.0f, 6.5535f, "GAIN", 4},
    {0x53, "VOLTAGE_KD", 0.0f, 0.0f, 0.0100f, "GAIN", 4},
    {0x27, "FAN_SET", 100.0f, 0.0f, 100.0f, "%", 0},
};

static const char *const heartbeat_options[] = {"1S", "5S", "NEVER"};
static const char *const test_options[] = {"TEST"};
static const char *const brightness_options[] = {"15%", "30%", "50%", "65%", "80%", "100%"};
static const char *const sleep_options[] = {"30S", "2MIN", "10MIN", "NEVER"};
static const char *const color_options[] = {"CLASSIC", "LIGHT", "DARK", "LIME", "OCEAN", "SUNSET", "MINT", "AMBER"};
static const char *const esp_options[] = {"NONE", "WIFI", "BLE"};

setting_t settings[SETTING_COUNT] = {
    {"SYSTEM", "HEARTBEAT", "F429 USART1 runtime log period.", heartbeat_options, 3, 0},
    {"UI", "POPUP_TEST", "Show a local notification test message.", test_options, 1, 0},
    {"DISPLAY", "BRIGHTNESS", "LCD backlight brightness.", brightness_options, 6, 5},
    {"DISPLAY", "SLEEP_TIME", "Screen sleep timeout.", sleep_options, 4, 3},
    {"DISPLAY", "COLOR_PRESET", "Apply a preset color scheme.", color_options, 8, 0},
    {"ESP32", "START_MODE", "Select ESP32 service and lock local settings.", esp_options, 3, 0},
};

ui_state_t state;

char log_lines[LOG_CAPACITY][52];
uint8_t log_count;

void log_event(const char *message)
{
    if(log_count == LOG_CAPACITY) {
        memmove(log_lines, log_lines + 1, sizeof(log_lines[0]) * (LOG_CAPACITY - 1));
        log_count--;
    }
    snprintf(log_lines[log_count++], sizeof(log_lines[0]), "[%06lu MS] %s",
             (unsigned long)lv_tick_get(), message);
}
