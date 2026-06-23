/**
  ******************************************************************************
  * @file    panel_keys.h
  * @brief   Five-key protection/output panel control.
  ******************************************************************************
  */
#ifndef PANEL_KEYS_H
#define PANEL_KEYS_H

#include <stdint.h>

typedef enum
{
    PANEL_FIELD_OTP = 0,
    PANEL_FIELD_OVP,
    PANEL_FIELD_OCP,
    PANEL_FIELD_OUT,
    PANEL_FIELD_BLE,
    PANEL_FIELD_SCOPE,
    PANEL_FIELD_COUNT
} PanelField_t;

typedef enum
{
    PANEL_APPLY_NONE = 0,
    PANEL_APPLY_OVP,
    PANEL_APPLY_OCP,
    PANEL_APPLY_OTP,
    PANEL_APPLY_OUTPUT,
    PANEL_APPLY_BLE,
    PANEL_APPLY_COMM_START
} PanelApplyEvent_t;

typedef enum
{
    PANEL_BLE_OFF = 0,
    PANEL_BLE_ON,
    PANEL_BLE_STATE_COUNT
} PanelBleState_t;

typedef enum
{
    PANEL_PAGE_MAIN = 0,
    PANEL_PAGE_SCOPE
} PanelPage_t;

typedef enum
{
    PANEL_SCOPE_FIELD_CH1 = 0,
    PANEL_SCOPE_FIELD_CH2,
    PANEL_SCOPE_FIELD_Y1,
    PANEL_SCOPE_FIELD_Y2,
    PANEL_SCOPE_FIELD_TIME,
    PANEL_SCOPE_FIELD_TRIG,
    PANEL_SCOPE_FIELD_HOLD,
    PANEL_SCOPE_FIELD_BACK,
    PANEL_SCOPE_FIELD_COUNT
} PanelScopeField_t;

typedef enum
{
    PANEL_SCOPE_SOURCE_VOUT = 0,
    PANEL_SCOPE_SOURCE_IOUT,
    PANEL_SCOPE_SOURCE_POUT,
    PANEL_SCOPE_SOURCE_VIN,
    PANEL_SCOPE_SOURCE_IIN,
    PANEL_SCOPE_SOURCE_PIN,
    PANEL_SCOPE_SOURCE_VSET,
    PANEL_SCOPE_SOURCE_ISET,
    PANEL_SCOPE_SOURCE_EFF,
    PANEL_SCOPE_SOURCE_FAN,
    PANEL_SCOPE_SOURCE_COUNT
} PanelScopeSource_t;

typedef enum
{
    PANEL_SCOPE_TRIGGER_AUTO = 0,
    PANEL_SCOPE_TRIGGER_FAULT,
    PANEL_SCOPE_TRIGGER_STATE,
    PANEL_SCOPE_TRIGGER_COUNT
} PanelScopeTrigger_t;

void PanelKeys_Init(void);
void PanelKeys_Update(void);

PanelField_t PanelKeys_GetSelectedField(void);
PanelPage_t PanelKeys_GetPage(void);
float PanelKeys_GetOvp(void);
float PanelKeys_GetOcp(void);
float PanelKeys_GetOtp(void);
uint8_t PanelKeys_GetOvpEnabled(void);
uint8_t PanelKeys_GetOcpEnabled(void);
uint8_t PanelKeys_GetOtpEnabled(void);
uint8_t PanelKeys_GetOutputEnabled(void);
PanelBleState_t PanelKeys_GetBleState(void);
PanelScopeField_t PanelKeys_GetScopeField(void);
uint8_t PanelKeys_GetScopeTimebase(void);
uint8_t PanelKeys_GetScopeCh1Enabled(void);
uint8_t PanelKeys_GetScopeCh2Enabled(void);
uint8_t PanelKeys_GetScopeHold(void);
PanelScopeTrigger_t PanelKeys_GetScopeTrigger(void);
PanelScopeSource_t PanelKeys_GetScopeCh1Source(void);
PanelScopeSource_t PanelKeys_GetScopeCh2Source(void);
uint8_t PanelKeys_GetScopeCh1Scale(void);
uint8_t PanelKeys_GetScopeCh2Scale(void);
void PanelKeys_SetProtectionState(float ovp, float ocp, float otp, uint8_t ovp_enabled, uint8_t ocp_enabled, uint8_t otp_enabled);
void PanelKeys_SetOutputEnabled(uint8_t enabled);

PanelApplyEvent_t PanelKeys_TakeApplyEvent(void);

#endif /* PANEL_KEYS_H */
