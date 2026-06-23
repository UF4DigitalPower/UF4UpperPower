#include "uf4_power_client.h"

#include "uf4_mem.h"
#include "usart.h"
#include "uf4com.h"
#include "uf4com_parser.h"


#define UF4_POWER_CLIENT_TV_SIZE 3U
#define UF4_POWER_CLIENT_REG_COUNT 256U
#define UF4_POWER_CLIENT_MAX_TV_ITEMS 32U
#define UF4_POWER_CLIENT_UART_TX_TIMEOUT_MS 20U
#define UF4_POWER_CLIENT_RX_POLL_LIMIT 256U
#define UF4_POWER_CLIENT_RX_RING_SIZE 512U

static uf4_parser_t g_uf4_power_parser;
static uint8_t g_uf4_power_seq;
static volatile bool g_uf4_power_tx_enqueue_failed;
static volatile uint16_t g_uf4_power_regs[UF4_POWER_CLIENT_REG_COUNT];
static volatile uint8_t g_uf4_power_valid[UF4_POWER_CLIENT_REG_COUNT];
static volatile bool g_uf4_power_data_changed;
static volatile bool g_uf4_power_stream_enabled;
static volatile uint32_t g_uf4_power_tx_ok;
static volatile uint32_t g_uf4_power_tx_fail;
static volatile uint32_t g_uf4_power_rx_bytes;
static volatile uint32_t g_uf4_power_rx_frames;
static volatile uint32_t g_uf4_power_rx_errors;
static volatile uint32_t g_uf4_power_rx_overflows;
static volatile uint8_t g_uf4_power_last_tx_cmd;
static volatile uint8_t g_uf4_power_last_tx_len;
static volatile bool g_uf4_power_last_tx_ok;
static volatile uint16_t g_uf4_power_rx_head;
static volatile uint16_t g_uf4_power_rx_tail;
static uint8_t g_uf4_power_rx_ring[UF4_POWER_CLIENT_RX_RING_SIZE];

static void UF4PowerClient_HandleFrame(const uf4_frame_t *frame);

/**
  * @brief Transmit one assembled UF4 frame through USART1.
  */
static void UF4PowerClient_TxBytes(const uint8_t *data, uint16_t len, void *user)
{
  (void)user;

  if (data != NULL &&
      len != 0U &&
      HAL_UART_Transmit(&huart1, (uint8_t *)data, len, UF4_POWER_CLIENT_UART_TX_TIMEOUT_MS) == HAL_OK)
  {
    ++g_uf4_power_tx_ok;
  }
  else
  {
    g_uf4_power_tx_enqueue_failed = true;
    ++g_uf4_power_tx_fail;
  }
}

/**
  * @brief Feed one byte from the foreground RX ring into the UF4 parser.
  */
static void UF4_RAM_FUNC UF4PowerClient_ProcessRxByte(uint8_t byte)
{
  uf4_frame_t frame;

  ++g_uf4_power_rx_bytes;
  if (UF4_ParserInput(&g_uf4_power_parser, byte, &frame) != 0U)
  {
    UF4PowerClient_HandleFrame(&frame);
  }
}

/**
  * @brief Return the next index in the fixed-size RX ring.
  */
UF4_FORCE_INLINE uint16_t UF4PowerClient_RxRingNext(uint16_t pos)
{
  ++pos;
  if (pos >= UF4_POWER_CLIENT_RX_RING_SIZE)
  {
    pos = 0U;
  }
  return pos;
}

/**
  * @brief Push one byte into the ISR-safe RX ring.
  */
static void UF4_RAM_FUNC UF4PowerClient_RxRingPush(uint8_t byte)
{
  const uint16_t next_head = UF4PowerClient_RxRingNext(g_uf4_power_rx_head);

  if (next_head == g_uf4_power_rx_tail)
  {
    ++g_uf4_power_rx_overflows;
    return;
  }

  g_uf4_power_rx_ring[g_uf4_power_rx_head] = byte;
  g_uf4_power_rx_head = next_head;
}

/**
  * @brief Pop one byte from the foreground side of the RX ring.
  */
static bool UF4_RAM_FUNC UF4PowerClient_RxRingPop(uint8_t *byte)
{
  if (byte == NULL || g_uf4_power_rx_tail == g_uf4_power_rx_head)
  {
    return false;
  }

  *byte = g_uf4_power_rx_ring[g_uf4_power_rx_tail];
  g_uf4_power_rx_tail = UF4PowerClient_RxRingNext(g_uf4_power_rx_tail);
  return true;
}

/**
  * @brief Send one command frame and remember the latest TX status.
  */
static bool UF4PowerClient_SendCommand(uint8_t cmd, const uint8_t *payload, uint8_t len)
{
  bool ok;

  g_uf4_power_last_tx_cmd = cmd;
  g_uf4_power_last_tx_len = len;
  g_uf4_power_last_tx_ok = false;
  g_uf4_power_tx_enqueue_failed = false;
  if (UF4_SendFrame(g_uf4_power_seq++, UF4_FLAG_ACK_REQ, cmd, payload, len) == 0U)
  {
    return false;
  }

  ok = (g_uf4_power_tx_enqueue_failed == false);
  g_uf4_power_last_tx_ok = ok;
  return ok;
}

/**
  * @brief Build and send type-value payloads for read/write/stream commands.
  */
static bool UF4_RAM_FUNC UF4PowerClient_SendTvCommand(uint8_t cmd, const uint8_t *ids, const uint16_t *values, uint8_t count)
{
  uint8_t payload[UF4_POWER_CLIENT_TV_SIZE * UF4_POWER_CLIENT_MAX_TV_ITEMS];
  uint8_t offset = 0U;

  if (ids == NULL || count == 0U || count > UF4_POWER_CLIENT_MAX_TV_ITEMS)
  {
    return false;
  }

  for (uint8_t i = 0U; i < count; ++i)
  {
    const uint16_t value = (values != NULL) ? values[i] : 0U;
    payload[offset++] = ids[i];
    payload[offset++] = (uint8_t)(value >> 8U);
    payload[offset++] = (uint8_t)(value & 0xFFU);
  }

  return UF4PowerClient_SendCommand(cmd, payload, offset);
}

/**
  * @brief Store received type-value pairs into the local register cache.
  */
static void UF4_RAM_FUNC UF4PowerClient_StoreTvPayload(const uf4_frame_t *frame)
{
  if (frame == NULL || (frame->len % UF4_POWER_CLIENT_TV_SIZE) != 0U)
  {
    ++g_uf4_power_rx_errors;
    return;
  }

  for (uint8_t offset = 0U; offset < frame->len; offset += UF4_POWER_CLIENT_TV_SIZE)
  {
    const uint8_t id = frame->data[offset];
    const uint16_t value = ((uint16_t)frame->data[offset + 1U] << 8U) | frame->data[offset + 2U];
    g_uf4_power_regs[id] = value;
    g_uf4_power_valid[id] = 1U;
  }
  g_uf4_power_data_changed = true;
}

/**
  * @brief Dispatch a complete received UF4 frame into client state.
  */
static void UF4_RAM_FUNC UF4PowerClient_HandleFrame(const uf4_frame_t *frame)
{
  if (frame == NULL)
  {
    return;
  }

  ++g_uf4_power_rx_frames;
  if ((frame->flags & UF4_FLAG_ERROR) != 0U)
  {
    ++g_uf4_power_rx_errors;
    return;
  }

  switch (frame->cmd)
  {
    case UF4_CMD_READ_RSP:
    case UF4_CMD_WRITE_RSP:
      UF4PowerClient_StoreTvPayload(frame);
      break;
    case UF4_CMD_STREAM_DATA:
      g_uf4_power_stream_enabled = true;
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
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_ERR);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_PE);
}

void UF4_RAM_FUNC UF4PowerClient_Tick(void)
{
  uint32_t rx_count = 0U;
  uint8_t byte;

  if ((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_ORE) != RESET) ||
      (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_FE) != RESET) ||
      (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_NE) != RESET) ||
      (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_PE) != RESET))
  {
    __HAL_UART_CLEAR_OREFLAG(&huart1);
    __HAL_UART_CLEAR_FEFLAG(&huart1);
    __HAL_UART_CLEAR_NEFLAG(&huart1);
    __HAL_UART_CLEAR_PEFLAG(&huart1);
    ++g_uf4_power_rx_errors;
  }

  while ((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET) &&
         (rx_count < UF4_POWER_CLIENT_RX_POLL_LIMIT))
  {
    UF4PowerClient_RxRingPush((uint8_t)(huart1.Instance->RDR & 0xFFU));
    ++rx_count;
  }

  rx_count = 0U;
  while (UF4PowerClient_RxRingPop(&byte) && (rx_count < UF4_POWER_CLIENT_RX_POLL_LIMIT))
  {
    UF4PowerClient_ProcessRxByte(byte);
    ++rx_count;
  }
}

bool UF4PowerClient_WriteU16Pairs(const uint8_t *ids, const uint16_t *values, uint8_t count)
{
  if (values == NULL)
  {
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
  static const uint8_t ids[] UF4_FAST_CONST =
  {
    UF4_ID_INPUT_VOLTAGE,
    UF4_ID_INPUT_CURRENT,
    UF4_ID_OUTPUT_VOLTAGE,
    UF4_ID_OUTPUT_CURRENT,
    UF4_ID_CORE_TEMPERATURE,
    UF4_ID_TEMP1_TEMPERATURE,
    UF4_ID_TEMP2_TEMPERATURE,
    UF4_ID_SET_VOLTAGE_LIMIT,
    UF4_ID_SET_CURRENT_LIMIT,
    UF4_ID_CC_CV_MODE,
    UF4_ID_POWER_STATE,
    UF4_ID_FAULT_STATE,
    UF4_ID_STATE_MACHINE_FLAG_BITS,
    UF4_ID_STATE_MACHINE_STATE,
    UF4_ID_INPUT_VOLTAGE_RAW,
    UF4_ID_INPUT_CURRENT_RAW,
    UF4_ID_OUTPUT_VOLTAGE_RAW,
    UF4_ID_OUTPUT_CURRENT_RAW,
    UF4_ID_OTP_VALUE,
    UF4_ID_OTP_SET_VALUE,
    UF4_ID_OVP_VALUE,
    UF4_ID_OVP_SET_VALUE,
    UF4_ID_OCP_VALUE,
    UF4_ID_OCP_SET_VALUE,
    UF4_ID_DUTY_CMD,
    UF4_ID_PWM_A_COMPARE,
    UF4_ID_PWM_D_COMPARE,
    UF4_ID_FAN_SPEED,
    UF4_ID_FAN_SET_VALUE,
    UF4_ID_LOOP_CURRENT_FEEDBACK,
    UF4_ID_LOOP_CURRENT_REFERENCE,
    UF4_ID_VOLTAGE_LOOP_CURRENT_REFERENCE
  };

  return UF4PowerClient_StartStreamU16Pairs(ids, (uint8_t)(sizeof(ids) / sizeof(ids[0])));
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
  if (value == NULL || g_uf4_power_valid[id] == 0U)
  {
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

uint32_t UF4PowerClient_RxOverflowCount(void)
{
  return g_uf4_power_rx_overflows;
}

uint8_t UF4PowerClient_LastTxCmd(void)
{
  return g_uf4_power_last_tx_cmd;
}

uint8_t UF4PowerClient_LastTxLen(void)
{
  return g_uf4_power_last_tx_len;
}

bool UF4PowerClient_LastTxOk(void)
{
  return g_uf4_power_last_tx_ok;
}

void UF4_RAM_FUNC UF4PowerClient_UartIrqHandler(void)
{
  uint32_t isr = huart1.Instance->ISR;

  if ((isr & (USART_ISR_ORE | USART_ISR_FE | USART_ISR_NE | USART_ISR_PE)) != 0U)
  {
    __HAL_UART_CLEAR_FLAG(&huart1, UART_CLEAR_OREF | UART_CLEAR_FEF | UART_CLEAR_NEF | UART_CLEAR_PEF);
    ++g_uf4_power_rx_errors;
  }

  while ((huart1.Instance->ISR & USART_ISR_RXNE_RXFNE) != 0U)
  {
    UF4PowerClient_RxRingPush((uint8_t)(huart1.Instance->RDR & 0xFFU));
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    ++g_uf4_power_rx_errors;
  }
}
