#include "g474_remote.h"

#include "gui_internal.h"
#include "main.h"
#include "uf4com.h"
#include "uf4com_usrt.h"

#include <stdio.h>

#define G474_REG_COUNT  62U
#define G474_STREAM_ID_COUNT 18U
#define G474_SYNC_IDS_PER_REQUEST 19U
#define G474_POLL_PERIOD_MS 40U
#define G474_LINK_KEEPALIVE_MS 100U
#define G474_OUTPUT_HEARTBEAT_MS 100U
/* A journal compaction includes a 4 KiB erase before the commit record.
 * Keep the stream suspended long enough for the G474 Flash operation rather
 * than treating a valid first save as a communications failure. */
#define G474_TRANSACTION_TIMEOUT_MS 10000U

typedef enum
{
    G474_TRANSACTION_IDLE = 0U,
    G474_TRANSACTION_STOP_WAIT,
    G474_TRANSACTION_WRITE_WAIT,
    G474_TRANSACTION_SAVE_WAIT,
    G474_TRANSACTION_RESUME
} g474_transaction_state_t;

typedef struct
{
    uint8_t id;
    uint16_t value;
    uint8_t valid;
} g474_register_t;

/* All IDs currently exported by the G474 parameter and telemetry tables. */
static const uint8_t s_poll_ids[G474_REG_COUNT] = {
    UF4_ID_SET_VOLTAGE_LIMIT, UF4_ID_SET_CURRENT_LIMIT,
    UF4_ID_OCP_SET_VALUE, UF4_ID_OTP_SET_VALUE,
    UF4_ID_CFG_VIN_OVP, UF4_ID_CFG_VIN_UVP, UF4_ID_CFG_VOUT_OVP,
    UF4_ID_CFG_POWER_DIRECTION_MODE, UF4_ID_CFG_ADC_VREF,
    UF4_ID_CFG_VIN_R_UPPER, UF4_ID_CFG_VIN_R_LOWER,
    UF4_ID_CFG_VOUT_R_UPPER, UF4_ID_CFG_VOUT_R_LOWER,
    UF4_ID_CFG_SHUNT_IN, UF4_ID_CFG_SHUNT_OUT,
    UF4_ID_CFG_GAIN_IN, UF4_ID_CFG_GAIN_OUT,
    UF4_ID_CFG_CURRENT_OFFSET, UF4_ID_CFG_VIN_GAIN, UF4_ID_CFG_VIN_OFFSET,
    UF4_ID_CFG_VOUT_GAIN, UF4_ID_CFG_VOUT_OFFSET,
    UF4_ID_CFG_IIN_GAIN, UF4_ID_CFG_IIN_OFFSET,
    UF4_ID_CFG_IOUT_GAIN, UF4_ID_CFG_IOUT_OFFSET,
    UF4_ID_FAN_SET_VALUE, UF4_ID_POWER_DIRECTION, UF4_ID_OUTPUT_ENABLE,
    UF4_ID_INPUT_VOLTAGE, UF4_ID_INPUT_VOLTAGE_RAW,
    UF4_ID_INPUT_CURRENT, UF4_ID_INPUT_CURRENT_RAW,
    UF4_ID_OUTPUT_VOLTAGE, UF4_ID_OUTPUT_VOLTAGE_RAW,
    UF4_ID_OUTPUT_CURRENT, UF4_ID_OUTPUT_CURRENT_RAW,
    UF4_ID_DUTY_CMD, UF4_ID_FMAC_SELFTEST_FIRST_RAW,
    UF4_ID_LOOP_CURRENT_FEEDBACK, UF4_ID_LOOP_CURRENT_REFERENCE,
    UF4_ID_VOLTAGE_LOOP_CURRENT_REFERENCE, UF4_ID_FMAC_SELFTEST_SECOND_RAW,
    UF4_ID_FMAC_DIAGNOSTIC_STAGE, UF4_ID_FMAC_DIAGNOSTIC_STATUS,
    UF4_ID_DAC3_CH1_OUTPUT_CODE, UF4_ID_DAC4_CH1_OUTPUT_CODE,
    UF4_ID_SELF_CHECK_STATUS, UF4_ID_LATCHED_FAULT_REASON,
    UF4_ID_POWER_STATE, UF4_ID_FAULT_STATE,
    UF4_ID_STATE_MACHINE_FLAG_BITS, UF4_ID_CC_CV_MODE,
    UF4_ID_POWER_DIRECTION_STATUS, UF4_ID_POWER_CONVERTER_MODE,
    UF4_ID_CONTROL_ISR_LAST_CYCLES, UF4_ID_CONTROL_ISR_MAX_CYCLES,
    UF4_ID_CONTROL_ISR_OVERRUN_COUNT, UF4_ID_FAN_SPEED,
    UF4_ID_CORE_TEMPERATURE, UF4_ID_TEMP1_TEMPERATURE,
    UF4_ID_TEMP2_TEMPERATURE
};
static const uint8_t s_stream_ids[G474_STREAM_ID_COUNT] = {
    UF4_ID_INPUT_VOLTAGE, UF4_ID_INPUT_CURRENT,
    UF4_ID_OUTPUT_VOLTAGE, UF4_ID_OUTPUT_CURRENT,
    UF4_ID_CORE_TEMPERATURE, UF4_ID_TEMP1_TEMPERATURE, UF4_ID_TEMP2_TEMPERATURE,
    UF4_ID_CC_CV_MODE, UF4_ID_POWER_STATE, UF4_ID_FAULT_STATE,
    UF4_ID_POWER_DIRECTION_STATUS, UF4_ID_POWER_CONVERTER_MODE,
    UF4_ID_CONTROL_ISR_LAST_CYCLES, UF4_ID_CONTROL_ISR_MAX_CYCLES,
    UF4_ID_CONTROL_ISR_OVERRUN_COUNT, UF4_ID_OUTPUT_ENABLE,
    UF4_ID_DUTY_CMD, UF4_ID_FAN_SPEED
};
static g474_register_t s_registers[G474_REG_COUNT];
static uint8_t s_sequence;
static uint8_t s_poll_index;
static uint8_t s_request_pending;
static uint8_t s_full_sync_active;
static uint8_t s_full_sync_index;
static uint8_t s_full_sync_batch_count;
static uint8_t s_ui_has_data;
static uint32_t s_last_request_ms;
static uint32_t s_last_poll_ms;
static uint32_t s_last_response_ms;
static uint32_t s_last_stream_request_ms;
static uint32_t s_last_stream_data_ms;
static uint32_t s_last_output_command_ms;
static uint8_t s_output_commanded;
static uint8_t s_stream_requested;
static uint8_t s_stream_active;
static uint8_t s_fast_poll_pending;
static uint32_t s_rx_frames;
static uint32_t s_rx_rejected;
static uint32_t s_rx_values;
static uint32_t s_ack_done;
static uint32_t s_ack_timeout;
static uint32_t s_ack_dropped;
static uint32_t s_tx_submit_failures;
static uint32_t s_last_debug_ms;
static g474_transaction_state_t s_transaction_state;
static uint8_t s_transaction_data[UF4_MAX_DATA_LEN];
static uint8_t s_transaction_len;
static uint32_t s_transaction_started_ms;
static uint8_t s_save_result;

#define G474_SAVE_RESULT_NONE     0U
#define G474_SAVE_RESULT_SUCCESS  1U
#define G474_SAVE_RESULT_FAILED   2U

static void G474_Remote_AckEvent(uint8_t seq,
                                 uint8_t cmd,
                                 uf4_ack_status_t status,
                                 void *user)
{
    (void)seq;
    (void)cmd;
    (void)user;

    if(status == UF4_ACK_DONE)
    {
        s_ack_done++;
    }
    else if(status == UF4_ACK_TIMEOUT)
    {
        const uint32_t now_ms = HAL_GetTick();

        s_ack_timeout++;
        s_request_pending = 0U;
        s_last_request_ms = now_ms;

        if(cmd == UF4_CMD_READ_RSP)
        {
            s_fast_poll_pending = 0U;
        }
        else if(cmd == UF4_CMD_STREAM_START_RSP)
        {
            /* The initial stream request is the first link handshake.  Do not
             * leave the remote state machine permanently pending when G474 is
             * booting, resetting, or the UART was briefly unavailable. */
            s_stream_requested = 0U;
            s_stream_active = 0U;
            s_last_stream_request_ms = now_ms;
        }
    }
    else if(status == UF4_ACK_DROPPED)
    {
        s_ack_dropped++;
    }
}

static void G474_Remote_ClearUiData(void)
{
    printf("[F429][CFG] parameter cache cleared: G474 offline\r\n");
    for(uint8_t i = 0U; i < G474_REG_COUNT; ++i)
    {
        s_registers[i].value = 0U;
        s_registers[i].valid = 0U;
    }

    for(uint16_t i = 0U; i < UI_PARAM_COUNT; ++i)
    {
        g_params[i].value = 0.0f;
    }

    g_ui.voltage_set = 0.0f;
    g_ui.current_limit = 0.0f;
    g_ui.output_voltage = 0.0f;
    g_ui.output_current = 0.0f;
    g_ui.output_on = 0U;
    UI_Params_ClearDirty();
    s_ui_has_data = 0U;
}

static uint8_t G474_Remote_StartNextFullSyncRead(void)
{
    uint8_t request[G474_SYNC_IDS_PER_REQUEST * 3U];
    uint8_t count = 0U;

    if(s_full_sync_active == 0U || s_full_sync_index >= G474_REG_COUNT)
    {
        s_full_sync_active = 0U;
        return 0U;
    }

    while(count < G474_SYNC_IDS_PER_REQUEST &&
          (uint8_t)(s_full_sync_index + count) < G474_REG_COUNT)
    {
        request[count * 3U] = s_poll_ids[s_full_sync_index + count];
        request[count * 3U + 1U] = 0U;
        request[count * 3U + 2U] = 0U;
        ++count;
    }

    if(UF4_SendFrame(s_sequence++, UF4_FLAG_ACK_REQ,
                       UF4_CMD_READ_REQ, request, (uint8_t)(count * 3U)) == 0U)
    {
        s_tx_submit_failures++;
        s_last_request_ms = HAL_GetTick();
        return 0U;
    }

    s_full_sync_batch_count = count;
    s_request_pending = 1U;
    s_last_request_ms = HAL_GetTick();
    return 1U;
}

static uint8_t G474_Remote_StartFastPoll(void)
{
    uint8_t request[G474_STREAM_ID_COUNT * 3U];

    for(uint8_t i = 0U; i < G474_STREAM_ID_COUNT; ++i)
    {
        request[i * 3U] = s_stream_ids[i];
        request[i * 3U + 1U] = 0U;
        request[i * 3U + 2U] = 0U;
    }

    if(UF4_SendFrame(s_sequence++, UF4_FLAG_ACK_REQ,
                       UF4_CMD_READ_REQ, request, sizeof(request)) == 0U)
    {
        s_tx_submit_failures++;
        return 0U;
    }

    s_fast_poll_pending = 1U;
    s_request_pending = 1U;
    s_last_request_ms = HAL_GetTick();
    s_last_poll_ms = s_last_request_ms;
    return 1U;
}

static uint8_t G474_Remote_SendKeepalive(void)
{
    const uint8_t request[3] = { UF4_ID_INPUT_VOLTAGE, 0U, 0U };

    if(UF4_SendFrame(s_sequence++, UF4_FLAG_ACK_REQ,
                       UF4_CMD_READ_REQ, request, sizeof(request)) == 0U)
    {
        s_tx_submit_failures++;
        return 0U;
    }

    s_request_pending = 1U;
    s_last_request_ms = HAL_GetTick();
    s_last_poll_ms = s_last_request_ms;
    return 1U;
}

static uint8_t G474_Remote_SendOutputHeartbeat(void)
{
    const uint8_t data[3] = { UF4_ID_OUTPUT_ENABLE, 0U, s_output_commanded };

    if(UF4_SendFrame(s_sequence++, UF4_FLAG_ACK_REQ,
                       UF4_CMD_WRITE_REQ, data, sizeof(data)) == 0U)
    {
        s_tx_submit_failures++;
        return 0U;
    }

    s_request_pending = 1U;
    s_last_request_ms = HAL_GetTick();
    s_last_output_command_ms = s_last_request_ms;
    return 1U;
}

static uint8_t G474_Remote_StartStream(void)
{
    uint8_t request[G474_STREAM_ID_COUNT * 3U];

    for(uint8_t i = 0U; i < G474_STREAM_ID_COUNT; ++i)
    {
        request[i * 3U] = s_stream_ids[i];
        request[i * 3U + 1U] = 0U;
        request[i * 3U + 2U] = 0U;
    }

    if(UF4_SendFrame(s_sequence++, UF4_FLAG_ACK_REQ,
                       UF4_CMD_STREAM_START_REQ, request, sizeof(request)) == 0U)
    {
        s_tx_submit_failures++;
        return 0U;
    }

    s_stream_requested = 1U;
    s_request_pending = 1U;
    s_last_stream_request_ms = HAL_GetTick();
    return 1U;
}

static uint8_t G474_Remote_SendSave(void)
{
    const uint8_t seq = s_sequence;

    s_transaction_state = G474_TRANSACTION_SAVE_WAIT;
    printf("[F429][SAVE] SAVE_REQ seq=%u\r\n", (unsigned)seq);
    if(UF4_SendFrame(s_sequence++, UF4_FLAG_ACK_REQ,
                       UF4_CMD_SAVE_REQ, NULL, 0U) == 0U)
    {
        s_tx_submit_failures++;
        s_transaction_state = G474_TRANSACTION_IDLE;
        printf("[F429][SAVE] SAVE_REQ submit failed seq=%u\r\n", (unsigned)seq);
        return 0U;
    }
    return 1U;
}

static uint8_t G474_Remote_SendWrite(void)
{
    const uint8_t seq = s_sequence;

    s_transaction_state = G474_TRANSACTION_WRITE_WAIT;
    printf("[F429][SAVE] WRITE_REQ seq=%u count=%u", (unsigned)seq,
           (unsigned)(s_transaction_len / 3U));
    for(uint8_t i = 0U; i < s_transaction_len; i += 3U)
    {
        const uint16_t value = ((uint16_t)s_transaction_data[i + 1U] << 8U) |
                               s_transaction_data[i + 2U];
        printf(" id=%02X raw=%04X", (unsigned)s_transaction_data[i],
               (unsigned)value);
    }
    printf("\r\n");
    if(UF4_SendFrame(s_sequence++, UF4_FLAG_ACK_REQ,
                       UF4_CMD_WRITE_REQ, s_transaction_data,
                       s_transaction_len) == 0U)
    {
        s_tx_submit_failures++;
        s_transaction_state = G474_TRANSACTION_IDLE;
        printf("[F429][SAVE] WRITE_REQ submit failed seq=%u\r\n", (unsigned)seq);
        return 0U;
    }
    return 1U;
}

static void G474_Remote_LogStatus(uint32_t now_ms)
{
    if((uint32_t)(now_ms - s_last_debug_ms) < 1000U)
    {
        return;
    }

    s_last_debug_ms = now_ms;
    printf("[F429][UART6] g474_on=%u stream_req=%u stream=%u age_rsp=%lums age_stream=%lums "
           "rxF=%lu rxV=%lu rej=%lu ack=%lu to=%lu drop=%lu submit_fail=%lu\r\n",
           (unsigned)G474_Remote_IsOnline(),
           (unsigned)s_stream_requested,
           (unsigned)G474_Remote_IsStreaming(),
           (unsigned long)(now_ms - s_last_response_ms),
           (unsigned long)(now_ms - s_last_stream_data_ms),
           (unsigned long)s_rx_frames,
           (unsigned long)s_rx_values,
           (unsigned long)s_rx_rejected,
           (unsigned long)s_ack_done,
           (unsigned long)s_ack_timeout,
           (unsigned long)s_ack_dropped,
           (unsigned long)s_tx_submit_failures);
}

static int16_t G474_Remote_Find(uint8_t id)
{
    for(uint8_t i = 0U; i < G474_REG_COUNT; ++i)
    {
        if(s_registers[i].id == id)
        {
            return (int16_t)i;
        }
    }
    return -1;
}

static float G474_Remote_Scale(uint8_t id, uint16_t value)
{
    switch(id)
    {
        case UF4_ID_INPUT_VOLTAGE: case UF4_ID_INPUT_CURRENT:
        case UF4_ID_OUTPUT_VOLTAGE: case UF4_ID_OUTPUT_CURRENT:
        case UF4_ID_SET_VOLTAGE_LIMIT: case UF4_ID_SET_CURRENT_LIMIT:
        case UF4_ID_OTP_VALUE: case UF4_ID_OTP_SET_VALUE:
        case UF4_ID_OVP_VALUE: case UF4_ID_OVP_SET_VALUE:
        case UF4_ID_OCP_VALUE: case UF4_ID_OCP_SET_VALUE:
        case UF4_ID_CFG_VIN_OVP: case UF4_ID_CFG_VIN_UVP:
        case UF4_ID_CFG_VOUT_OVP: case UF4_ID_CFG_ADC_VREF:
        case UF4_ID_LOOP_CURRENT_FEEDBACK: case UF4_ID_LOOP_CURRENT_REFERENCE:
        case UF4_ID_VOLTAGE_LOOP_CURRENT_REFERENCE:
            return (float)value / 100.0f;
        case UF4_ID_CORE_TEMPERATURE: case UF4_ID_TEMP1_TEMPERATURE:
        case UF4_ID_TEMP2_TEMPERATURE:
            return (float)(int16_t)value / 100.0f;
        case UF4_ID_FAN_SET_VALUE:
            return (float)value / 10.0f;
        case UF4_ID_DUTY_CMD:
            return (float)value / 1000.0f;
        case UF4_ID_CFG_CURRENT_OFFSET: case UF4_ID_CFG_VIN_OFFSET:
        case UF4_ID_CFG_VOUT_OFFSET: case UF4_ID_CFG_IIN_OFFSET:
        case UF4_ID_CFG_IOUT_OFFSET:
            return (float)(int16_t)value / 100.0f;
        case UF4_ID_CFG_GAIN_IN: case UF4_ID_CFG_GAIN_OUT:
        case UF4_ID_CFG_VIN_GAIN: case UF4_ID_CFG_VOUT_GAIN:
        case UF4_ID_CFG_IIN_GAIN: case UF4_ID_CFG_IOUT_GAIN:
            return (float)value / 100.0f;
        case UF4_ID_CFG_CURRENT_KP: case UF4_ID_CFG_CURRENT_KI: case UF4_ID_CFG_CURRENT_KD:
        case UF4_ID_CFG_VOLTAGE_KP: case UF4_ID_CFG_VOLTAGE_KI: case UF4_ID_CFG_VOLTAGE_KD:
            return (float)value / 10000.0f;
        default:
            return (float)value;
    }
}

static uint16_t G474_Remote_Encode(uint8_t id, float value)
{
    if(id == UF4_ID_FAN_SET_VALUE) return (uint16_t)(value * 10.0f);
    if(id == UF4_ID_CFG_CURRENT_KP || id == UF4_ID_CFG_CURRENT_KI || id == UF4_ID_CFG_CURRENT_KD ||
       id == UF4_ID_CFG_VOLTAGE_KP || id == UF4_ID_CFG_VOLTAGE_KI || id == UF4_ID_CFG_VOLTAGE_KD)
        return (uint16_t)(value * 10000.0f);
    if(id == UF4_ID_SET_VOLTAGE_LIMIT || id == UF4_ID_SET_CURRENT_LIMIT ||
       id == UF4_ID_OCP_SET_VALUE || id == UF4_ID_OTP_SET_VALUE ||
       id == UF4_ID_CFG_VIN_OVP || id == UF4_ID_CFG_VIN_UVP ||
       id == UF4_ID_CFG_VOUT_OVP || id == UF4_ID_CFG_ADC_VREF)
        return (uint16_t)(value * 100.0f);
    if(id == UF4_ID_CFG_CURRENT_OFFSET || id == UF4_ID_CFG_VIN_OFFSET ||
       id == UF4_ID_CFG_VOUT_OFFSET || id == UF4_ID_CFG_IIN_OFFSET ||
       id == UF4_ID_CFG_IOUT_OFFSET)
        return (uint16_t)(int16_t)(value * 100.0f);
    if(id == UF4_ID_CFG_GAIN_IN || id == UF4_ID_CFG_GAIN_OUT ||
       id == UF4_ID_CFG_VIN_GAIN || id == UF4_ID_CFG_VOUT_GAIN ||
       id == UF4_ID_CFG_IIN_GAIN || id == UF4_ID_CFG_IOUT_GAIN)
        return (uint16_t)(value * 100.0f);
    return (uint16_t)value;
}

static void G474_Remote_ApplyUi(uint8_t id, uint16_t value)
{
    const uint8_t dirty = UI_Params_IsDirty(id);

    for(uint16_t i = 0U; i < UI_PARAM_COUNT; ++i)
    {
        if(g_params[i].id == id && dirty == 0U)
        {
            const float scaled = G474_Remote_Scale(id, value);
            if(g_params[i].value != scaled)
            {
                g_params[i].value = scaled;
                UI_Params_NotifyRemoteUpdate(id);
            }
            if(id == UF4_ID_CFG_VIN_OFFSET)
            {
                printf("[F429][CFG] VIN_CAL_OFF raw=%04X value=%.2f\r\n",
                       (unsigned)value, (double)g_params[i].value);
            }
        }
    }

    if(id == UF4_ID_SET_VOLTAGE_LIMIT && dirty == 0U) g_ui.voltage_set = G474_Remote_Scale(id, value);
    if(id == UF4_ID_SET_CURRENT_LIMIT && dirty == 0U) g_ui.current_limit = G474_Remote_Scale(id, value);
    if(id == UF4_ID_OUTPUT_VOLTAGE) g_ui.output_voltage = G474_Remote_Scale(id, value);
    if(id == UF4_ID_OUTPUT_CURRENT) g_ui.output_current = G474_Remote_Scale(id, value);
    if(id == UF4_ID_OUTPUT_ENABLE && dirty == 0U)
    {
        g_ui.output_on = (value == 1U) ? 1U : 0U;
        printf("[F429][OUTPUT] G474 enable=%u\r\n", (unsigned)g_ui.output_on);
    }
}

static void G474_Remote_FrameRx(const uf4_frame_t *frame, void *user)
{
    (void)user;
    if(frame != NULL &&
       (frame->cmd == UF4_CMD_WRITE_RSP || frame->cmd == UF4_CMD_SAVE_RSP ||
        (frame->flags & UF4_FLAG_ERROR) != 0U))
    {
        printf("[F429][SAVE] RSP cmd=%02X seq=%u flags=%02X len=%u",
               (unsigned)frame->cmd, (unsigned)frame->seq,
               (unsigned)frame->flags, (unsigned)frame->len);
        for(uint8_t i = 0U; i < frame->len; ++i)
        {
            printf(" %02X", (unsigned)frame->data[i]);
        }
        printf(" state=%u\r\n", (unsigned)s_transaction_state);
    }
    if(frame == NULL ||
       (frame->cmd != UF4_CMD_READ_RSP && frame->cmd != UF4_CMD_WRITE_RSP &&
        frame->cmd != UF4_CMD_STREAM_DATA && frame->cmd != UF4_CMD_STREAM_START_RSP &&
        frame->cmd != UF4_CMD_STREAM_STOP_RSP && frame->cmd != UF4_CMD_SAVE_RSP) ||
       ((frame->cmd == UF4_CMD_READ_RSP || frame->cmd == UF4_CMD_WRITE_RSP ||
         frame->cmd == UF4_CMD_STREAM_DATA) && (frame->len % 3U) != 0U))
    {
        s_rx_rejected++;
        return;
    }

    s_rx_frames++;

    if((frame->flags & UF4_FLAG_ERROR) != 0U &&
       s_transaction_state != G474_TRANSACTION_IDLE &&
       s_transaction_state != G474_TRANSACTION_RESUME)
    {
        printf("[F429][SAVE] failed by G474 error cmd=%02X seq=%u\r\n",
               (unsigned)frame->cmd, (unsigned)frame->seq);
        s_save_result = G474_SAVE_RESULT_FAILED;
        s_transaction_state = G474_TRANSACTION_IDLE;
        return;
    }

    for(uint8_t i = 0U; i < frame->len; i += 3U)
    {
        const uint8_t id = frame->data[i];
        const uint16_t value = ((uint16_t)frame->data[i + 1U] << 8U) | frame->data[i + 2U];
        const int16_t index = G474_Remote_Find(id);
        if(index >= 0)
        {
            s_registers[index].value = value;
            s_registers[index].valid = 1U;
        }
        G474_Remote_ApplyUi(id, value);
        s_rx_values++;
    }
    if(frame->len != 0U)
    {
        s_ui_has_data = 1U;
    }
    if(frame->cmd == UF4_CMD_READ_RSP || frame->cmd == UF4_CMD_WRITE_RSP)
    {
        s_request_pending = 0U;
    }
    s_last_response_ms = HAL_GetTick();
    if(frame->cmd == UF4_CMD_READ_RSP && s_fast_poll_pending != 0U)
    {
        s_fast_poll_pending = 0U;
        s_stream_active = 1U;
        s_last_stream_data_ms = s_last_response_ms;
    }
    if(frame->cmd == UF4_CMD_WRITE_RSP &&
       s_transaction_state == G474_TRANSACTION_WRITE_WAIT)
    {
        (void)G474_Remote_SendSave();
        return;
    }
    if(frame->cmd == UF4_CMD_SAVE_RSP &&
       s_transaction_state == G474_TRANSACTION_SAVE_WAIT)
    {
        s_save_result = G474_SAVE_RESULT_SUCCESS;
        s_transaction_state = G474_TRANSACTION_IDLE;
        return;
    }
    if(frame->cmd == UF4_CMD_STREAM_START_RSP)
    {
        s_request_pending = 0U;
        s_stream_active = 1U;
        s_full_sync_active = 1U;
        s_full_sync_index = 0U;
        s_full_sync_batch_count = 0U;
        (void)G474_Remote_StartNextFullSyncRead();
    }
    else if(frame->cmd == UF4_CMD_READ_RSP && s_full_sync_active != 0U)
    {
        s_full_sync_index = (uint8_t)(s_full_sync_index + s_full_sync_batch_count);
        if(s_full_sync_index < G474_REG_COUNT)
        {
            (void)G474_Remote_StartNextFullSyncRead();
        }
        else
        {
            s_full_sync_active = 0U;
            s_poll_index = 0U;
        }
    }
    if(frame->cmd == UF4_CMD_STREAM_DATA)
    {
        s_last_stream_data_ms = s_last_response_ms;
    }
}

void G474_Remote_Init(void)
{
    for(uint8_t i = 0U; i < G474_REG_COUNT; ++i)
    {
        s_registers[i].id = s_poll_ids[i];
    }
    s_sequence = 0U;
    s_poll_index = 0U;
    s_request_pending = 0U;
    s_full_sync_active = 1U;
    s_full_sync_index = 0U;
    s_full_sync_batch_count = 0U;
    s_last_request_ms = 0U;
    s_last_poll_ms = HAL_GetTick() - G474_POLL_PERIOD_MS;
    s_last_response_ms = 0U;
    s_last_stream_request_ms = HAL_GetTick();
    s_last_stream_data_ms = 0U;
    s_last_output_command_ms = HAL_GetTick() - G474_OUTPUT_HEARTBEAT_MS;
    s_output_commanded = 0U;
    s_stream_requested = 0U;
    s_stream_active = 0U;
    s_fast_poll_pending = 0U;
    s_rx_frames = 0U;
    s_rx_rejected = 0U;
    s_rx_values = 0U;
    s_ack_done = 0U;
    s_ack_timeout = 0U;
    s_ack_dropped = 0U;
    s_tx_submit_failures = 0U;
    s_last_debug_ms = 0U;
    s_transaction_state = G474_TRANSACTION_IDLE;
    s_transaction_len = 0U;
    s_transaction_started_ms = 0U;
    s_save_result = G474_SAVE_RESULT_NONE;
    G474_Remote_ClearUiData();
    UF4_SetFrameRxCallback(G474_Remote_FrameRx, NULL);
    UF4_SetAckEventCallback(G474_Remote_AckEvent, NULL);
}

void G474_Remote_Process(void)
{
    const uint32_t now_ms = HAL_GetTick();

    G474_Remote_LogStatus(now_ms);

    if(s_transaction_state != G474_TRANSACTION_IDLE &&
       s_transaction_state != G474_TRANSACTION_RESUME &&
       (uint32_t)(now_ms - s_transaction_started_ms) >= G474_TRANSACTION_TIMEOUT_MS)
    {
        printf("[F429][SAVE] transaction timeout state=%u after=%lums\r\n",
               (unsigned)s_transaction_state,
               (unsigned long)(now_ms - s_transaction_started_ms));
        s_save_result = G474_SAVE_RESULT_FAILED;
        s_transaction_state = G474_TRANSACTION_IDLE;
    }

    if(s_stream_active != 0U && s_last_stream_data_ms != 0U &&
       (uint32_t)(now_ms - s_last_stream_data_ms) >= 500U)
    {
        s_stream_active = 0U;
        s_stream_requested = 0U;
    }

    if(G474_Remote_IsOnline() == 0U && s_ui_has_data != 0U)
    {
        G474_Remote_ClearUiData();
    }

    if(s_transaction_state != G474_TRANSACTION_IDLE)
    {
        return;
    }

    /* Assert the local safety intent before link setup and telemetry work.
       In particular, a rebooted F429 must revoke a previously enabled G474
       output as soon as the UART becomes available. */
    if(s_request_pending == 0U &&
       (uint32_t)(now_ms - s_last_output_command_ms) >= G474_OUTPUT_HEARTBEAT_MS)
    {
        (void)G474_Remote_SendOutputHeartbeat();
        return;
    }

    if(s_stream_requested == 0U)
    {
        if(s_request_pending == 0U &&
           (uint32_t)(now_ms - s_last_stream_request_ms) >= 100U)
        {
            (void)G474_Remote_StartStream();
        }
        return;
    }

    if(s_full_sync_active != 0U)
    {
        if(s_request_pending == 0U &&
           (uint32_t)(now_ms - s_last_request_ms) >= 20U)
        {
            (void)G474_Remote_StartNextFullSyncRead();
        }
        return;
    }

    /* G474 revokes its streaming session after one second without a master
       request.  A small valid read every 250 ms is the link keepalive; live
       telemetry itself remains the 20 Hz STREAM_DATA frame. */
    if(s_request_pending == 0U &&
       (uint32_t)(now_ms - s_last_poll_ms) >= G474_LINK_KEEPALIVE_MS)
    {
        (void)G474_Remote_SendKeepalive();
    }
}

uint8_t G474_Remote_RequestConnection(void)
{
    s_stream_requested = 0U;
    s_stream_active = 0U;
    s_last_stream_request_ms = HAL_GetTick() - 100U;
    s_full_sync_active = 1U;
    s_full_sync_index = 0U;
    s_full_sync_batch_count = 0U;
    return 1U;
}

uint8_t G474_Remote_Read(uint8_t id, uint16_t *value)
{
    const int16_t index = G474_Remote_Find(id);
    if(index < 0 || value == NULL || s_registers[index].valid == 0U) return 0U;
    *value = s_registers[index].value;
    return 1U;
}

uint8_t G474_Remote_ReadFloat(uint8_t id, float *value)
{
    uint16_t raw;

    if(value == NULL || G474_Remote_Read(id, &raw) == 0U)
    {
        return 0U;
    }

    *value = G474_Remote_Scale(id, raw);
    return 1U;
}

uint8_t G474_Remote_Write(uint8_t id, uint16_t value)
{
    const uint8_t data[3] = { id, (uint8_t)(value >> 8U), (uint8_t)value };
    if(id == UF4_ID_OUTPUT_ENABLE)
    {
        s_output_commanded = value == 1U ? 1U : 0U;
        s_last_output_command_ms = HAL_GetTick();
        printf("[F429][OUTPUT] WRITE_REQ seq=%u enable=%u\r\n",
               (unsigned)s_sequence, (unsigned)s_output_commanded);
    }
    if(UF4_SendFrame(s_sequence++, UF4_FLAG_ACK_REQ,
                       UF4_CMD_WRITE_REQ, data, sizeof(data)) == 0U) return 0U;
    return 1U;
}

uint8_t G474_Remote_WriteFloat(uint8_t id, float value)
{
    return G474_Remote_Write(id, G474_Remote_Encode(id, value));
}

uint8_t G474_Remote_GetOutputCommanded(void)
{
    return s_output_commanded;
}

uint8_t G474_Remote_SaveParameter(uint8_t id, float value)
{
    return G474_Remote_SaveParameters(&id, &value, 1U);
}

uint8_t G474_Remote_SaveParameters(const uint8_t *ids, const float *values, uint8_t count)
{
    if(s_transaction_state != G474_TRANSACTION_IDLE)
    {
        return 0U;
    }
    if(ids == NULL || values == NULL || count == 0U ||
       count > (uint8_t)(UF4_MAX_DATA_LEN / 3U))
    {
        return 0U;
    }

    for(uint8_t i = 0U; i < count; ++i)
    {
        const uint16_t encoded = G474_Remote_Encode(ids[i], values[i]);
        s_transaction_data[i * 3U] = ids[i];
        s_transaction_data[i * 3U + 1U] = (uint8_t)(encoded >> 8U);
        s_transaction_data[i * 3U + 2U] = (uint8_t)encoded;
    }
    s_transaction_len = (uint8_t)(count * 3U);
    s_transaction_started_ms = HAL_GetTick();
    s_save_result = G474_SAVE_RESULT_NONE;
    return G474_Remote_SendWrite();
}

uint8_t G474_Remote_IsOnline(void)
{
    return (s_last_response_ms != 0U &&
            (uint32_t)(HAL_GetTick() - s_last_response_ms) < 500U) ? 1U : 0U;
}

uint8_t G474_Remote_IsSaveInProgress(void)
{
    return (s_transaction_state != G474_TRANSACTION_IDLE) ? 1U : 0U;
}

uint8_t G474_Remote_ConsumeSaveResult(void)
{
    const uint8_t result = s_save_result;

    s_save_result = G474_SAVE_RESULT_NONE;
    return result;
}

uint8_t G474_Remote_IsStreamRequested(void)
{
    return s_stream_requested;
}

uint8_t G474_Remote_IsStreaming(void)
{
    return (s_stream_active != 0U && s_last_stream_data_ms != 0U &&
            (uint32_t)(HAL_GetTick() - s_last_stream_data_ms) < 500U) ? 1U : 0U;
}
