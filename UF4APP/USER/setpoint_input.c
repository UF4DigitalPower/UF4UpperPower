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

#define SETPOINT_DIGIT_COUNT 4U
#define SETPOINT_MIN_VALUE   0.0F
#define SETPOINT_MAX_VALUE   99.99F

typedef struct
{
    TIM_HandleTypeDef *htim;
    GPIO_TypeDef *push_port;
    uint16_t push_pin;
    uint32_t last_count;
    float value;
    uint8_t digit;
} SetpointInput_Channel_t;

static SetpointInput_Channel_t g_vset_input =
{
    &htim4,
    KEY_V_PUSH_GPIO_Port,
    KEY_V_PUSH_Pin,
    0U,
    5.0F,
    3U
};

static SetpointInput_Channel_t g_iset_input =
{
    &htim2,
    KEY_I_PUSH_GPIO_Port,
    KEY_I_PUSH_Pin,
    0U,
    1.0F,
    3U
};

static void SetpointInput_UpdateChannel(SetpointInput_Channel_t *channel);
static void SetpointInput_HandleStep(SetpointInput_Channel_t *channel, int32_t step);
static void SetpointInput_AdjustValue(SetpointInput_Channel_t *channel, int32_t step);
static int32_t SetpointInput_GetDelta(SetpointInput_Channel_t *channel);
static float SetpointInput_GetDigitStep(uint8_t digit);
static uint8_t SetpointInput_IsPushPressed(const SetpointInput_Channel_t *channel);
static float SetpointInput_Clamp(float value);

void SetpointInput_Init(void)
{
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

    __HAL_TIM_SET_COUNTER(&htim4, 0U);
    __HAL_TIM_SET_COUNTER(&htim2, 0U);

    g_vset_input.last_count = 0U;
    g_iset_input.last_count = 0U;
}

void SetpointInput_Update(void)
{
    SetpointInput_UpdateChannel(&g_vset_input);
    SetpointInput_UpdateChannel(&g_iset_input);
}

float SetpointInput_GetVset(void)
{
    return g_vset_input.value;
}

float SetpointInput_GetIset(void)
{
    return g_iset_input.value;
}

uint8_t SetpointInput_GetVsetDigit(void)
{
    return g_vset_input.digit;
}

uint8_t SetpointInput_GetIsetDigit(void)
{
    return g_iset_input.digit;
}

static void SetpointInput_UpdateChannel(SetpointInput_Channel_t *channel)
{
    int32_t delta = SetpointInput_GetDelta(channel);

    while (delta >= 1)
    {
        SetpointInput_HandleStep(channel, 1);
        --delta;
    }

    while (delta <= -1)
    {
        SetpointInput_HandleStep(channel, -1);
        ++delta;
    }
}

static void SetpointInput_HandleStep(SetpointInput_Channel_t *channel, int32_t step)
{
    if (SetpointInput_IsPushPressed(channel))
    {
        if (step > 0)
        {
            channel->digit = (uint8_t)((channel->digit + 1U) % SETPOINT_DIGIT_COUNT);
        }
        else
        {
            channel->digit = (uint8_t)((channel->digit + SETPOINT_DIGIT_COUNT - 1U) % SETPOINT_DIGIT_COUNT);
        }
    }
    else
    {
        SetpointInput_AdjustValue(channel, step);
    }
}

static void SetpointInput_AdjustValue(SetpointInput_Channel_t *channel, int32_t step)
{
    float value_step = SetpointInput_GetDigitStep(channel->digit);

    channel->value =
        SetpointInput_Clamp(channel->value + ((float)step * value_step));
}

static int32_t SetpointInput_GetDelta(SetpointInput_Channel_t *channel)
{
    uint32_t current =
        __HAL_TIM_GET_COUNTER(channel->htim);
    int32_t delta;

    if (channel->htim->Instance == TIM4)
    {
        delta = (int16_t)((uint16_t)current - (uint16_t)channel->last_count);
    }
    else
    {
        delta = (int32_t)(current - channel->last_count);
    }

    channel->last_count = current;

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

static uint8_t SetpointInput_IsPushPressed(const SetpointInput_Channel_t *channel)
{
    return (uint8_t)(HAL_GPIO_ReadPin(channel->push_port, channel->push_pin) == GPIO_PIN_RESET);
}

static float SetpointInput_Clamp(float value)
{
    if (value < SETPOINT_MIN_VALUE)
    {
        return SETPOINT_MIN_VALUE;
    }

    if (value > SETPOINT_MAX_VALUE)
    {
        return SETPOINT_MAX_VALUE;
    }

    return value;
}
