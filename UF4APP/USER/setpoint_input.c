/**
  ******************************************************************************
  * @file    setpoint_input.c
  * @author  UF4
  * @brief   Encoder driven VSET/ISET input.
  ******************************************************************************
  */
#include "setpoint_input.h"

#include "gpio.h"
#include "tim.h"

#define SETPOINT_DIGIT_COUNT         4U
#define SETPOINT_MIN_VALUE           0.0F
#define SETPOINT_VSET_MAX_VALUE      45.0F
#define SETPOINT_ISET_MAX_VALUE      11.0F
#define SETPOINT_PUSH_DEBOUNCE_TICKS 3U
#define SETPOINT_TIM4_CENTER         0x8000U
#define SETPOINT_TIM2_CENTER         0x80000000UL
#define SETPOINT_ENCODER_COUNTS_PER_STEP 2

typedef struct
{
    TIM_HandleTypeDef *htim;
    GPIO_TypeDef *push_port;
    uint16_t push_pin;
    uint32_t center_count;
    int32_t encoder_accum;
    float value;
    uint8_t digit;
    GPIO_PinState idle_push_state;
    GPIO_PinState push_sample_state;
    GPIO_PinState debounced_push_state;
    GPIO_PinState last_debounced_push_state;
    uint8_t push_stable_ticks;
} SetpointInput_Channel_t;

static volatile SetpointInput_Channel_t g_vset_input =
{
    &htim4,
    KEY_V_PUSH_GPIO_Port,
    KEY_V_PUSH_Pin,
    SETPOINT_TIM4_CENTER,
    0,
    5.0F,
    3U,
    GPIO_PIN_SET,
    GPIO_PIN_SET,
    GPIO_PIN_SET,
    GPIO_PIN_SET,
    0U
};

static volatile SetpointInput_Channel_t g_iset_input =
{
    &htim2,
    KEY_I_PUSH_GPIO_Port,
    KEY_I_PUSH_Pin,
    SETPOINT_TIM2_CENTER,
    0,
    1.0F,
    3U,
    GPIO_PIN_SET,
    GPIO_PIN_SET,
    GPIO_PIN_SET,
    GPIO_PIN_SET,
    0U
};

static void SetpointInput_UpdatePush(volatile SetpointInput_Channel_t *channel);
static void SetpointInput_UpdateEncoder(volatile SetpointInput_Channel_t *channel);
static void SetpointInput_AdjustValue(volatile SetpointInput_Channel_t *channel, int32_t step);
static int32_t SetpointInput_ReadAndResetDelta(volatile SetpointInput_Channel_t *channel);
static float SetpointInput_GetDigitStep(uint8_t digit);
static float SetpointInput_Clamp(const volatile SetpointInput_Channel_t *channel, float value);

void SetpointInput_Init(void)
{
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_2);
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_2);

    __HAL_TIM_SET_COUNTER(&htim4, SETPOINT_TIM4_CENTER);
    __HAL_TIM_SET_COUNTER(&htim2, SETPOINT_TIM2_CENTER);

    g_vset_input.idle_push_state =
        HAL_GPIO_ReadPin(g_vset_input.push_port, g_vset_input.push_pin);
    g_iset_input.idle_push_state =
        HAL_GPIO_ReadPin(g_iset_input.push_port, g_iset_input.push_pin);

    g_vset_input.push_sample_state = g_vset_input.idle_push_state;
    g_iset_input.push_sample_state = g_iset_input.idle_push_state;
    g_vset_input.debounced_push_state = g_vset_input.idle_push_state;
    g_iset_input.debounced_push_state = g_iset_input.idle_push_state;
    g_vset_input.last_debounced_push_state = g_vset_input.idle_push_state;
    g_iset_input.last_debounced_push_state = g_iset_input.idle_push_state;
    g_vset_input.push_stable_ticks = 0U;
    g_iset_input.push_stable_ticks = 0U;
}

void SetpointInput_Update(void)
{
    SetpointInput_UpdateButtons();
    SetpointInput_UpdateEncoders();
}

void SetpointInput_UpdateButtons(void)
{
    SetpointInput_UpdatePush(&g_vset_input);
    SetpointInput_UpdatePush(&g_iset_input);
}

void SetpointInput_UpdateEncoders(void)
{
    SetpointInput_UpdateEncoder(&g_vset_input);
    SetpointInput_UpdateEncoder(&g_iset_input);
}

float SetpointInput_GetVset(void)
{
    return g_vset_input.value;
}

float SetpointInput_GetIset(void)
{
    return g_iset_input.value;
}

void SetpointInput_SetVset(float value)
{
    g_vset_input.value = SetpointInput_Clamp(&g_vset_input, value);
}

void SetpointInput_SetIset(float value)
{
    g_iset_input.value = SetpointInput_Clamp(&g_iset_input, value);
}

void SetpointInput_AdjustVset(int32_t step)
{
    SetpointInput_AdjustValue(&g_vset_input, step);
}

void SetpointInput_AdjustIset(int32_t step)
{
    SetpointInput_AdjustValue(&g_iset_input, step);
}

uint8_t SetpointInput_GetVsetDigit(void)
{
    return g_vset_input.digit;
}

uint8_t SetpointInput_GetIsetDigit(void)
{
    return g_iset_input.digit;
}

static void SetpointInput_UpdatePush(volatile SetpointInput_Channel_t *channel)
{
    GPIO_PinState sample =
        HAL_GPIO_ReadPin(channel->push_port, channel->push_pin);

    if (sample == channel->push_sample_state)
    {
        if (channel->push_stable_ticks < SETPOINT_PUSH_DEBOUNCE_TICKS)
        {
            ++channel->push_stable_ticks;
        }
    }
    else
    {
        channel->push_sample_state = sample;
        channel->push_stable_ticks = 0U;
    }

    if (channel->push_stable_ticks >= SETPOINT_PUSH_DEBOUNCE_TICKS)
    {
        channel->debounced_push_state = sample;
    }

    if (channel->debounced_push_state != channel->last_debounced_push_state)
    {
        __HAL_TIM_SET_COUNTER(channel->htim, channel->center_count);

        if (channel->debounced_push_state != channel->idle_push_state)
        {
            channel->digit = (channel->digit == 0U)
                ? (uint8_t)(SETPOINT_DIGIT_COUNT - 1U)
                : (uint8_t)(channel->digit - 1U);
        }

        channel->last_debounced_push_state = channel->debounced_push_state;
    }
}

static void SetpointInput_UpdateEncoder(volatile SetpointInput_Channel_t *channel)
{
    int32_t delta =
        SetpointInput_ReadAndResetDelta(channel);
    int32_t step;

    channel->encoder_accum += delta;
    step = channel->encoder_accum / SETPOINT_ENCODER_COUNTS_PER_STEP;

    if (step != 0)
    {
        channel->encoder_accum -= step * SETPOINT_ENCODER_COUNTS_PER_STEP;
        SetpointInput_AdjustValue(channel, step);
    }
}

static void SetpointInput_AdjustValue(volatile SetpointInput_Channel_t *channel, int32_t step)
{
    float value_step = SetpointInput_GetDigitStep(channel->digit);

    channel->value =
        SetpointInput_Clamp(channel, channel->value + ((float)step * value_step));
}

static int32_t SetpointInput_ReadAndResetDelta(volatile SetpointInput_Channel_t *channel)
{
    uint32_t count =
        __HAL_TIM_GET_COUNTER(channel->htim);
    int32_t delta;

    if (channel->htim->Instance == TIM4)
    {
        delta = (int16_t)((uint16_t)count - (uint16_t)channel->center_count);
    }
    else
    {
        delta = (int32_t)(count - channel->center_count);
    }

    __HAL_TIM_SET_COUNTER(channel->htim, channel->center_count);

    return delta;
}

static float SetpointInput_GetDigitStep(uint8_t digit)
{
    static const float steps[SETPOINT_DIGIT_COUNT] =
    {
        10.0F,
        1.0F,
        0.1F,
        0.01F
    };

    if (digit >= SETPOINT_DIGIT_COUNT)
    {
        return 0.01F;
    }

    return steps[digit];
}

static float SetpointInput_Clamp(const volatile SetpointInput_Channel_t *channel, float value)
{
    float max_value = SETPOINT_ISET_MAX_VALUE;

    if (channel == &g_vset_input)
    {
        max_value = SETPOINT_VSET_MAX_VALUE;
    }

    if (value < SETPOINT_MIN_VALUE)
    {
        return SETPOINT_MIN_VALUE;
    }

    if (value > max_value)
    {
        return max_value;
    }

    return value;
}
