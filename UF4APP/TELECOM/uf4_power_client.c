#include "uf4_power_client.h"

#include "usart.h"
#include "uf4com.h"
#include "uf4com_parser.h"

#include <stddef.h>

#define UF4_POWER_CLIENT_UART_TIMEOUT_MS 20U
#define UF4_POWER_CLIENT_TV_SIZE 3U
#define UF4_POWER_CLIENT_REG_COUNT 256U
#define UF4_POWER_CLIENT_MAX_TV_ITEMS 32U

static uint8_t g_uf4_power_rx_byte;
static uf4_parser_t g_uf4_power_parser;
static uint8_t g_uf4_power_seq;
static volatile uint16_t g_uf4_power_regs[UF4_POWER_CLIENT_REG_COUNT];
static volatile uint8_t g_uf4_power_valid[UF4_POWER_CLIENT_REG_COUNT];
static volatile bool g_uf4_power_data_changed;
static volatile bool g_uf4_power_stream_enabled;
static volatile uint32_t g_uf4_power_tx_ok;
static volatile uint32_t g_uf4_power_tx_fail;
static volatile uint32_t g_uf4_power_rx_bytes;
static volatile uint32_t g_uf4_power_rx_frames;
static volatile uint32_t g_uf4_power_rx_errors;

static void UF4PowerClient_TxBytes(const uint8_t *data, uint16_t len, void *user)
{
  (void)user;
  if (data == NULL || len == 0U) {
    return;
  }

  if (HAL_UART_Transmit(&huart1, (uint8_t *)data, len, UF4_POWER_CLIENT_UART_TIMEOUT_MS) == HAL_OK) {
    ++g_uf4_power_tx_ok;
  } else {
    ++g_uf4_power_tx_fail;
  }
}

static uint16_t UF4PowerClient_ClampU16(uint32_t value)
{
  return (value > 0xFFFFU) ? 0xFFFFU : (uint16_t)value;
}

static bool UF4PowerClient_SendCommand(uint8_t cmd, const uint8_t *payload, uint8_t len)
{
  return UF4_SendFrame(g_uf4_power_seq++, UF4_FLAG_ACK_REQ, cmd, payload, len) != 0U;
}

static bool UF4PowerClient_SendTvCommand(uint8_t cmd, const uint8_t *ids, const uint16_t *values, uint8_t count)
{
  uint8_t payload[UF4_POWER_CLIENT_TV_SIZE * UF4_POWER_CLIENT_MAX_TV_ITEMS];
  uint8_t offset = 0U;

  if (ids == NULL || count == 0U || count > UF4_POWER_CLIENT_MAX_TV_ITEMS) {
    return false;
  }

  for (uint8_t i = 0U; i < count; ++i) {
    const uint16_t value = (values != NULL) ? values[i] : 0U;
    payload[offset++] = ids[i];
    payload[offset++] = (uint8_t)(value >> 8U);
    payload[offset++] = (uint8_t)(value & 0xFFU);
  }

  return UF4PowerClient_SendCommand(cmd, payload, offset);
}

static void UF4PowerClient_StoreTvPayload(const uf4_frame_t *frame)
{
  if (frame == NULL || (frame->len % UF4_POWER_CLIENT_TV_SIZE) != 0U) {
    ++g_uf4_power_rx_errors;
    return;
  }

  for (uint8_t offset = 0U; offset < frame->len; offset += UF4_POWER_CLIENT_TV_SIZE) {
    const uint8_t id = frame->data[offset];
    const uint16_t value = ((uint16_t)frame->data[offset + 1U] << 8U) | frame->data[offset + 2U];
    g_uf4_power_regs[id] = value;
    g_uf4_power_valid[id] = 1U;
  }
  g_uf4_power_data_changed = true;
}

static void UF4PowerClient_HandleFrame(const uf4_frame_t *frame)
{
  if (frame == NULL) {
    return;
  }

  ++g_uf4_power_rx_frames;
  if ((frame->flags & UF4_FLAG_ERROR) != 0U) {
    ++g_uf4_power_rx_errors;
    return;
  }

  switch (frame->cmd) {
    case UF4_CMD_READ_RSP:
    case UF4_CMD_WRITE_RSP:
    case UF4_CMD_STREAM_DATA:
      UF4PowerClient_StoreTvPayload(frame);
      break;
    case UF4_CMD_STREAM_START_RSP:
      g_uf4_power_stream_enabled = true;
      g_uf4_power_data_changed = true;
      break;
    case UF4_CMD_STREAM_STOP_RSP:
      g_uf4_power_stream_enabled = false;
      g_uf4_power_data_changed = true;
      break;
    default:
      break;
  }
}

void UF4PowerClient_Init(void)
{
  UF4_ParserInit(&g_uf4_power_parser);
  UF4_Init(UF4PowerClient_TxBytes, NULL);
  (void)HAL_UART_Receive_IT(&huart1, &g_uf4_power_rx_byte, 1U);
}

void UF4PowerClient_Tick(void)
{
}

bool UF4PowerClient_WriteU16Pairs(const uint8_t *ids, const uint16_t *values, uint8_t count)
{
  if (values == NULL) {
    return false;
  }

  return UF4PowerClient_SendTvCommand(UF4_CMD_WRITE_REQ, ids, values, count);
}

bool UF4PowerClient_WriteU16(uint8_t id, uint16_t value)
{
  return UF4PowerClient_WriteU16Pairs(&id, &value, 1U);
}

bool UF4PowerClient_ReadU16Pairs(const uint8_t *ids, uint8_t count)
{
  return UF4PowerClient_SendTvCommand(UF4_CMD_READ_REQ, ids, NULL, count);
}

bool UF4PowerClient_StartStreamAll(void)
{
  return UF4PowerClient_SendCommand(UF4_CMD_STREAM_START_REQ, NULL, 0U);
}

bool UF4PowerClient_StartStreamU16Pairs(const uint8_t *ids, uint8_t count)
{
  return UF4PowerClient_SendTvCommand(UF4_CMD_STREAM_START_REQ, ids, NULL, count);
}

bool UF4PowerClient_StopStream(void)
{
  return UF4PowerClient_SendCommand(UF4_CMD_STREAM_STOP_REQ, NULL, 0U);
}

bool UF4PowerClient_GetU16(uint8_t id, uint16_t *value)
{
  if (value == NULL || g_uf4_power_valid[id] == 0U) {
    return false;
  }

  *value = g_uf4_power_regs[id];
  return true;
}

bool UF4PowerClient_ConsumeDataChanged(void)
{
  const bool changed = g_uf4_power_data_changed;
  g_uf4_power_data_changed = false;
  return changed;
}

bool UF4PowerClient_IsStreamEnabled(void)
{
  return g_uf4_power_stream_enabled;
}

uint32_t UF4PowerClient_TxOkCount(void)
{
  return g_uf4_power_tx_ok;
}

uint32_t UF4PowerClient_TxFailCount(void)
{
  return g_uf4_power_tx_fail;
}

uint32_t UF4PowerClient_RxByteCount(void)
{
  return g_uf4_power_rx_bytes;
}

uint32_t UF4PowerClient_RxFrameCount(void)
{
  return g_uf4_power_rx_frames;
}

uint32_t UF4PowerClient_RxErrorCount(void)
{
  return g_uf4_power_rx_errors;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1) {
    uf4_frame_t frame;
    ++g_uf4_power_rx_bytes;
    if (UF4_ParserInput(&g_uf4_power_parser, g_uf4_power_rx_byte, &frame) != 0U) {
      UF4PowerClient_HandleFrame(&frame);
    }
    (void)HAL_UART_Receive_IT(&huart1, &g_uf4_power_rx_byte, 1U);
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1) {
    ++g_uf4_power_rx_errors;
    (void)HAL_UART_Receive_IT(&huart1, &g_uf4_power_rx_byte, 1U);
  }
}
