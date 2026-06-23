/**
  ******************************************************************************
  * @file    panel_keys.c
  * @brief   Five-key protection/output panel control.
  ******************************************************************************
  */
#include "panel_keys.h"

#include "gpio.h"
#include "setpoint_input.h"

#define PANEL_KEY_DEBOUNCE_TICKS       3U
#define PANEL_KEY_LONG_TICKS           50U
#define PANEL_KEY_FAST_TICKS           200U
#define PANEL_KEY_REPEAT_SLOW_TICKS    15U
#define PANEL_KEY_REPEAT_FAST_TICKS    5U

#define PANEL_OVP_MIN                  0.0F
#define PANEL_OVP_MAX                  99.9F
#define PANEL_OVP_STEP                 0.1F
#define PANEL_OCP_MIN                  0.0F
#define PANEL_OCP_MAX                  99.9F
#define PANEL_OCP_STEP                 0.1F
#define PANEL_OTP_MIN                  0.0F
#define PANEL_OTP_MAX                  150.0F
#define PANEL_OTP_STEP                 1.0F

typedef enum
{
    PANEL_KEY_L = 0,
    PANEL_KEY_R,
    PANEL_KEY_UP,
    PANEL_KEY_DN,
    PANEL_KEY_M,
    PANEL_KEY_COUNT
} PanelKeyId_t;

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    GPIO_PinState idle_state;
    GPIO_PinState sample_state;
    GPIO_PinState debounced_state;
    GPIO_PinState last_debounced_state;
    uint16_t stable_ticks;
    uint16_t press_ticks;
    uint16_t repeat_ticks;
    uint8_t press_event;
    uint8_t repeat_event;
    uint8_t long_event;
    uint8_t long_reported;
} PanelKey_t;

static PanelKey_t g_panel_keys[PANEL_KEY_COUNT] =
{
    {KEY_L_GPIO_Port, KEY_L_Pin, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    {KEY_R_GPIO_Port, KEY_R_Pin, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    {KEY_UP_GPIO_Port, KEY_UP_Pin, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    {KEY_DN_GPIO_Port, KEY_DN_Pin, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
    {KEY_M_GPIO_Port, KEY_M_Pin, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, 0U, 0U, 0U, 0U, 0U, 0U, 0U}
};

static PanelField_t g_selected_field = PANEL_FIELD_OTP;
static float g_ovp = 24.0F;
static float g_ocp = 15.0F;
static float g_otp = 80.0F;
static uint8_t g_ovp_enabled = 1U;
static uint8_t g_ocp_enabled = 1U;
static uint8_t g_otp_enabled = 1U;
static uint8_t g_output_enabled = 0U;
static PanelBleState_t g_ble_state = PANEL_BLE_OFF;
static PanelPage_t g_page = PANEL_PAGE_MAIN;
static PanelApplyEvent_t g_apply_event = PANEL_APPLY_NONE;
static PanelScopeField_t g_scope_field = PANEL_SCOPE_FIELD_VSET;
static uint8_t g_scope_timebase = 1U;
static uint8_t g_scope_ch1_enabled = 1U;
static uint8_t g_scope_ch2_enabled = 1U;
static uint8_t g_scope_hold = 0U;
static PanelScopeTrigger_t g_scope_trigger = PANEL_SCOPE_TRIGGER_AUTO;
static PanelScopeSource_t g_scope_ch1_source = PANEL_SCOPE_SOURCE_VOUT;
static PanelScopeSource_t g_scope_ch2_source = PANEL_SCOPE_SOURCE_IOUT;
static uint8_t g_scope_ch1_scale = 2U;
static uint8_t g_scope_ch2_scale = 2U;

static void PanelKeys_UpdateKey(PanelKey_t *key);
static uint8_t PanelKeys_ConsumePress(PanelKeyId_t key);
static uint8_t PanelKeys_ConsumeRepeat(PanelKeyId_t key);
static uint8_t PanelKeys_ConsumeLong(PanelKeyId_t key);
static void PanelKeys_SelectNext(void);
static void PanelKeys_SelectPrevious(void);
static void PanelKeys_AdjustSelected(int8_t dir);
static void PanelKeys_ToggleSelected(void);
static void PanelKeys_UpdateScopePage(void);
static void PanelKeys_SelectScopeNext(void);
static void PanelKeys_SelectScopePrevious(void);
static void PanelKeys_AdjustScopeSelected(int8_t dir);
static void PanelKeys_ToggleScopeSelected(void);
static float PanelKeys_Clamp(float value, float min_value, float max_value);

void PanelKeys_Init(void)
{
    uint32_t i;

    for (i = 0U; i < PANEL_KEY_COUNT; ++i)
    {
        GPIO_PinState state =
            HAL_GPIO_ReadPin(g_panel_keys[i].port, g_panel_keys[i].pin);

        g_panel_keys[i].idle_state = state;
        g_panel_keys[i].sample_state = state;
        g_panel_keys[i].debounced_state = state;
        g_panel_keys[i].last_debounced_state = state;
        g_panel_keys[i].stable_ticks = 0U;
        g_panel_keys[i].press_ticks = 0U;
        g_panel_keys[i].repeat_ticks = 0U;
        g_panel_keys[i].press_event = 0U;
        g_panel_keys[i].repeat_event = 0U;
        g_panel_keys[i].long_event = 0U;
        g_panel_keys[i].long_reported = 0U;
    }
}

void PanelKeys_Update(void)
{
    uint32_t i;

    for (i = 0U; i < PANEL_KEY_COUNT; ++i)
    {
        PanelKeys_UpdateKey(&g_panel_keys[i]);
    }

    if (g_page == PANEL_PAGE_SCOPE)
    {
        PanelKeys_UpdateScopePage();
        return;
    }

    if (PanelKeys_ConsumePress(PANEL_KEY_L) != 0U)
    {
        PanelKeys_SelectPrevious();
    }
    if (PanelKeys_ConsumePress(PANEL_KEY_R) != 0U)
    {
        PanelKeys_SelectNext();
    }
    if (PanelKeys_ConsumePress(PANEL_KEY_UP) != 0U ||
        PanelKeys_ConsumeRepeat(PANEL_KEY_UP) != 0U)
    {
        PanelKeys_AdjustSelected(1);
    }
    if (PanelKeys_ConsumePress(PANEL_KEY_DN) != 0U ||
        PanelKeys_ConsumeRepeat(PANEL_KEY_DN) != 0U)
    {
        PanelKeys_AdjustSelected(-1);
    }
    if (PanelKeys_ConsumeLong(PANEL_KEY_M) != 0U)
    {
        g_apply_event = PANEL_APPLY_COMM_START;
        (void)PanelKeys_ConsumePress(PANEL_KEY_M);
    }
    else if (PanelKeys_ConsumePress(PANEL_KEY_M) != 0U)
    {
        PanelKeys_ToggleSelected();
    }
}

PanelField_t PanelKeys_GetSelectedField(void)
{
    return g_selected_field;
}

PanelPage_t PanelKeys_GetPage(void)
{
    return g_page;
}

float PanelKeys_GetOvp(void)
{
    return g_ovp;
}

float PanelKeys_GetOcp(void)
{
    return g_ocp;
}

float PanelKeys_GetOtp(void)
{
    return g_otp;
}

uint8_t PanelKeys_GetOvpEnabled(void)
{
    return g_ovp_enabled;
}

uint8_t PanelKeys_GetOcpEnabled(void)
{
    return g_ocp_enabled;
}

uint8_t PanelKeys_GetOtpEnabled(void)
{
    return g_otp_enabled;
}

uint8_t PanelKeys_GetOutputEnabled(void)
{
    return g_output_enabled;
}

PanelBleState_t PanelKeys_GetBleState(void)
{
    return g_ble_state;
}

PanelScopeField_t PanelKeys_GetScopeField(void)
{
    return g_scope_field;
}

uint8_t PanelKeys_GetScopeTimebase(void)
{
    return g_scope_timebase;
}

uint8_t PanelKeys_GetScopeCh1Enabled(void)
{
    return g_scope_ch1_enabled;
}

uint8_t PanelKeys_GetScopeCh2Enabled(void)
{
    return g_scope_ch2_enabled;
}

uint8_t PanelKeys_GetScopeHold(void)
{
    return g_scope_hold;
}

PanelScopeTrigger_t PanelKeys_GetScopeTrigger(void)
{
    return g_scope_trigger;
}

PanelScopeSource_t PanelKeys_GetScopeCh1Source(void)
{
    return g_scope_ch1_source;
}

PanelScopeSource_t PanelKeys_GetScopeCh2Source(void)
{
    return g_scope_ch2_source;
}

uint8_t PanelKeys_GetScopeCh1Scale(void)
{
    return g_scope_ch1_scale;
}

uint8_t PanelKeys_GetScopeCh2Scale(void)
{
    return g_scope_ch2_scale;
}

void PanelKeys_SetProtectionState(float ovp, float ocp, float otp, uint8_t ovp_enabled, uint8_t ocp_enabled, uint8_t otp_enabled)
{
    g_ovp = PanelKeys_Clamp(ovp, PANEL_OVP_MIN, PANEL_OVP_MAX);
    g_ocp = PanelKeys_Clamp(ocp, PANEL_OCP_MIN, PANEL_OCP_MAX);
    g_otp = PanelKeys_Clamp(otp, PANEL_OTP_MIN, PANEL_OTP_MAX);
    g_ovp_enabled = (uint8_t)(ovp_enabled != 0U ? 1U : 0U);
    g_ocp_enabled = (uint8_t)(ocp_enabled != 0U ? 1U : 0U);
    g_otp_enabled = (uint8_t)(otp_enabled != 0U ? 1U : 0U);
}

void PanelKeys_SetOutputEnabled(uint8_t enabled)
{
    g_output_enabled = (uint8_t)(enabled != 0U ? 1U : 0U);
}

PanelApplyEvent_t PanelKeys_TakeApplyEvent(void)
{
    PanelApplyEvent_t event = g_apply_event;

    g_apply_event = PANEL_APPLY_NONE;
    return event;
}

static void PanelKeys_UpdateKey(PanelKey_t *key)
{
    GPIO_PinState sample =
        HAL_GPIO_ReadPin(key->port, key->pin);
    uint8_t pressed;

    if (sample == key->sample_state)
    {
        if (key->stable_ticks < PANEL_KEY_DEBOUNCE_TICKS)
        {
            ++key->stable_ticks;
        }
    }
    else
    {
        key->sample_state = sample;
        key->stable_ticks = 0U;
    }

    if (key->stable_ticks >= PANEL_KEY_DEBOUNCE_TICKS)
    {
        key->debounced_state = sample;
    }

    pressed = (uint8_t)(key->debounced_state != key->idle_state);
    if (key->debounced_state != key->last_debounced_state)
    {
        if (pressed != 0U)
        {
            key->press_event = 1U;
            key->press_ticks = 0U;
            key->repeat_ticks = 0U;
            key->long_reported = 0U;
        }
        else
        {
            key->press_ticks = 0U;
            key->repeat_ticks = 0U;
            key->long_reported = 0U;
        }

        key->last_debounced_state = key->debounced_state;
    }

    if (pressed != 0U)
    {
        uint16_t repeat_period;

        if (key->press_ticks < 0xFFFFU)
        {
            ++key->press_ticks;
        }
        if (key->press_ticks >= PANEL_KEY_LONG_TICKS &&
            key->long_reported == 0U)
        {
            key->long_event = 1U;
            key->long_reported = 1U;
        }

        repeat_period = (key->press_ticks >= PANEL_KEY_FAST_TICKS)
            ? PANEL_KEY_REPEAT_FAST_TICKS
            : PANEL_KEY_REPEAT_SLOW_TICKS;
        if (key->press_ticks >= PANEL_KEY_LONG_TICKS)
        {
            ++key->repeat_ticks;
            if (key->repeat_ticks >= repeat_period)
            {
                key->repeat_ticks = 0U;
                key->repeat_event = 1U;
            }
        }
    }
}

static uint8_t PanelKeys_ConsumePress(PanelKeyId_t key)
{
    uint8_t event = g_panel_keys[key].press_event;

    g_panel_keys[key].press_event = 0U;
    return event;
}

static uint8_t PanelKeys_ConsumeRepeat(PanelKeyId_t key)
{
    uint8_t event = g_panel_keys[key].repeat_event;

    g_panel_keys[key].repeat_event = 0U;
    return event;
}

static uint8_t PanelKeys_ConsumeLong(PanelKeyId_t key)
{
    uint8_t event = g_panel_keys[key].long_event;

    g_panel_keys[key].long_event = 0U;
    return event;
}

static void PanelKeys_SelectNext(void)
{
    g_selected_field =
        (PanelField_t)(((uint8_t)g_selected_field + 1U) % (uint8_t)PANEL_FIELD_COUNT);
}

static void PanelKeys_SelectPrevious(void)
{
    g_selected_field = (g_selected_field == PANEL_FIELD_OTP)
        ? PANEL_FIELD_SCOPE
        : (PanelField_t)((uint8_t)g_selected_field - 1U);
}

static void PanelKeys_AdjustSelected(int8_t dir)
{
    float sign = (dir >= 0) ? 1.0F : -1.0F;

    switch (g_selected_field)
    {
        case PANEL_FIELD_OVP:
            g_ovp = PanelKeys_Clamp(g_ovp + sign * PANEL_OVP_STEP, PANEL_OVP_MIN, PANEL_OVP_MAX);
            break;
        case PANEL_FIELD_OCP:
            g_ocp = PanelKeys_Clamp(g_ocp + sign * PANEL_OCP_STEP, PANEL_OCP_MIN, PANEL_OCP_MAX);
            break;
        case PANEL_FIELD_OTP:
            g_otp = PanelKeys_Clamp(g_otp + sign * PANEL_OTP_STEP, PANEL_OTP_MIN, PANEL_OTP_MAX);
            break;
        case PANEL_FIELD_OUT:
            g_output_enabled = (dir >= 0) ? 1U : 0U;
            break;
        case PANEL_FIELD_BLE:
            g_ble_state = (dir >= 0) ? PANEL_BLE_ON : PANEL_BLE_OFF;
            break;
        case PANEL_FIELD_SCOPE:
            break;
        default:
            break;
    }
}

static void PanelKeys_ToggleSelected(void)
{
    switch (g_selected_field)
    {
        case PANEL_FIELD_OVP:
            break;
        case PANEL_FIELD_OCP:
            break;
        case PANEL_FIELD_OTP:
            break;
        case PANEL_FIELD_OUT:
            g_output_enabled = (uint8_t)!g_output_enabled;
            g_apply_event = PANEL_APPLY_OUTPUT;
            break;
        case PANEL_FIELD_BLE:
            g_ble_state = (g_ble_state == PANEL_BLE_OFF) ? PANEL_BLE_ON : PANEL_BLE_OFF;
            g_apply_event = PANEL_APPLY_BLE;
            break;
        case PANEL_FIELD_SCOPE:
            g_page = PANEL_PAGE_SCOPE;
            g_scope_field = PANEL_SCOPE_FIELD_VSET;
            break;
        default:
            break;
    }
}

/**
  * @brief Consume keys with an independent focus model while the scope page is active.
  */
static void PanelKeys_UpdateScopePage(void)
{
    if (PanelKeys_ConsumePress(PANEL_KEY_L) != 0U)
    {
        PanelKeys_SelectScopePrevious();
    }
    if (PanelKeys_ConsumePress(PANEL_KEY_R) != 0U)
    {
        PanelKeys_SelectScopeNext();
    }
    if (PanelKeys_ConsumePress(PANEL_KEY_UP) != 0U ||
        PanelKeys_ConsumeRepeat(PANEL_KEY_UP) != 0U)
    {
        PanelKeys_AdjustScopeSelected(1);
    }
    if (PanelKeys_ConsumePress(PANEL_KEY_DN) != 0U ||
        PanelKeys_ConsumeRepeat(PANEL_KEY_DN) != 0U)
    {
        PanelKeys_AdjustScopeSelected(-1);
    }

    (void)PanelKeys_ConsumeLong(PANEL_KEY_M);
    if (PanelKeys_ConsumePress(PANEL_KEY_M) != 0U)
    {
        PanelKeys_ToggleScopeSelected();
    }
}

static void PanelKeys_SelectScopeNext(void)
{
    g_scope_field = (PanelScopeField_t)(((uint8_t)g_scope_field + 1U) % (uint8_t)PANEL_SCOPE_FIELD_COUNT);
}

static void PanelKeys_SelectScopePrevious(void)
{
    g_scope_field = (g_scope_field == PANEL_SCOPE_FIELD_VSET)
        ? PANEL_SCOPE_FIELD_BACK
        : (PanelScopeField_t)((uint8_t)g_scope_field - 1U);
}

static void PanelKeys_AdjustScopeSelected(int8_t dir)
{
    switch (g_scope_field)
    {
        case PANEL_SCOPE_FIELD_VSET:
            SetpointInput_AdjustVset(dir);
            break;
        case PANEL_SCOPE_FIELD_ISET:
            SetpointInput_AdjustIset(dir);
            break;
        case PANEL_SCOPE_FIELD_CH1:
            if (dir >= 0)
            {
                g_scope_ch1_source = (PanelScopeSource_t)(((uint8_t)g_scope_ch1_source + 1U) % (uint8_t)PANEL_SCOPE_SOURCE_COUNT);
            }
            else
            {
                g_scope_ch1_source = (g_scope_ch1_source == PANEL_SCOPE_SOURCE_VOUT)
                    ? PANEL_SCOPE_SOURCE_FAN
                    : (PanelScopeSource_t)((uint8_t)g_scope_ch1_source - 1U);
            }
            break;
        case PANEL_SCOPE_FIELD_CH2:
            if (dir >= 0)
            {
                g_scope_ch2_source = (PanelScopeSource_t)(((uint8_t)g_scope_ch2_source + 1U) % (uint8_t)PANEL_SCOPE_SOURCE_COUNT);
            }
            else
            {
                g_scope_ch2_source = (g_scope_ch2_source == PANEL_SCOPE_SOURCE_VOUT)
                    ? PANEL_SCOPE_SOURCE_FAN
                    : (PanelScopeSource_t)((uint8_t)g_scope_ch2_source - 1U);
            }
            break;
        case PANEL_SCOPE_FIELD_Y1:
            if (dir >= 0 && g_scope_ch1_scale < 4U)
            {
                ++g_scope_ch1_scale;
            }
            else if (dir < 0 && g_scope_ch1_scale > 0U)
            {
                --g_scope_ch1_scale;
            }
            break;
        case PANEL_SCOPE_FIELD_Y2:
            if (dir >= 0 && g_scope_ch2_scale < 4U)
            {
                ++g_scope_ch2_scale;
            }
            else if (dir < 0 && g_scope_ch2_scale > 0U)
            {
                --g_scope_ch2_scale;
            }
            break;
        case PANEL_SCOPE_FIELD_TIME:
            if (dir >= 0 && g_scope_timebase < 4U)
            {
                ++g_scope_timebase;
            }
            else if (dir < 0 && g_scope_timebase > 0U)
            {
                --g_scope_timebase;
            }
            break;
        case PANEL_SCOPE_FIELD_TRIG:
            if (dir >= 0)
            {
                g_scope_trigger = (PanelScopeTrigger_t)(((uint8_t)g_scope_trigger + 1U) % (uint8_t)PANEL_SCOPE_TRIGGER_COUNT);
            }
            else
            {
                g_scope_trigger = (g_scope_trigger == PANEL_SCOPE_TRIGGER_AUTO)
                    ? PANEL_SCOPE_TRIGGER_STATE
                    : (PanelScopeTrigger_t)((uint8_t)g_scope_trigger - 1U);
            }
            break;
        case PANEL_SCOPE_FIELD_HOLD:
            g_scope_hold = (dir >= 0) ? 1U : 0U;
            break;
        case PANEL_SCOPE_FIELD_BACK:
            break;
        default:
            break;
    }
}

static void PanelKeys_ToggleScopeSelected(void)
{
    switch (g_scope_field)
    {
        case PANEL_SCOPE_FIELD_CH1:
            g_scope_ch1_enabled = (uint8_t)!g_scope_ch1_enabled;
            break;
        case PANEL_SCOPE_FIELD_CH2:
            g_scope_ch2_enabled = (uint8_t)!g_scope_ch2_enabled;
            break;
        case PANEL_SCOPE_FIELD_VSET:
        case PANEL_SCOPE_FIELD_ISET:
        case PANEL_SCOPE_FIELD_Y1:
        case PANEL_SCOPE_FIELD_Y2:
        case PANEL_SCOPE_FIELD_TIME:
            PanelKeys_AdjustScopeSelected(1);
            break;
        case PANEL_SCOPE_FIELD_TRIG:
            PanelKeys_AdjustScopeSelected(1);
            break;
        case PANEL_SCOPE_FIELD_HOLD:
            g_scope_hold = (uint8_t)!g_scope_hold;
            break;
        case PANEL_SCOPE_FIELD_BACK:
            g_page = PANEL_PAGE_MAIN;
            break;
        default:
            break;
    }
}

static float PanelKeys_Clamp(float value, float min_value, float max_value)
{
    if (value < min_value)
    {
        return min_value;
    }
    if (value > max_value)
    {
        return max_value;
    }

    return value;
}
