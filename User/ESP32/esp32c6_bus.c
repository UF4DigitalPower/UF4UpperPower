#include "esp32c6_bus.h"

#include <string.h>

#define ESP32C6_HOSTED_HEADER_SIZE       ((uint16_t)sizeof(ESP32C6_PayloadHeader))
#define ESP32C6_HOSTED_PRIV_EVENT_HEAD   2U
#define ESP32C6_HOSTED_TX_INIT_TLV_BYTES 18U

typedef struct __attribute__((packed))
{
  uint8_t event_type;
  uint8_t event_len;
  uint8_t data[0];
} ESP32C6_PrivEvent;

static uint8_t s_tx_frame[ESP32C6_HOSTED_SPI_FRAME];
static uint8_t s_rx_frame[ESP32C6_HOSTED_SPI_FRAME];
static uint16_t s_sequence;
static ESP32C6_HostedInitInfo s_init_info;
static ESP32C6_HostedPacket s_last_packet;
static ESP32C6_BusDiagnostics s_diagnostics;
static ESP32C6_SerialRxCallback s_serial_rx_callback;
static void *s_serial_rx_user;

static uint16_t esp32c6_get_le16(const uint8_t *p)
{
  return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t esp32c6_get_le32(const uint8_t *p)
{
  return (uint32_t)p[0] |
         ((uint32_t)p[1] << 8) |
         ((uint32_t)p[2] << 16) |
         ((uint32_t)p[3] << 24);
}

static uint16_t esp32c6_checksum(const uint8_t *buf, uint16_t len)
{
  uint16_t checksum = 0U;

  while (len != 0U) {
    checksum = (uint16_t)(checksum + *buf);
    buf++;
    len--;
  }

  return checksum;
}

static uint8_t esp32c6_parse_init_tlv(const uint8_t *data, uint16_t len)
{
  uint16_t pos = 0U;
  ESP32C6_HostedInitInfo info;

  memset(&info, 0, sizeof(info));

  while ((uint16_t)(pos + 2U) <= len) {
    const uint8_t tag = data[pos];
    const uint8_t tag_len = data[pos + 1U];
    const uint8_t *value = &data[pos + 2U];

    if ((uint16_t)(pos + 2U + tag_len) > len) {
      return 0U;
    }

    switch (tag) {
      case ESP32C6_HOSTED_TLV_CHIP_ID:
        if (tag_len >= 1U) {
          info.chip_id = value[0];
        }
        break;

      case ESP32C6_HOSTED_TLV_CAPABILITY:
        if (tag_len >= 1U) {
          info.capabilities = value[0];
        }
        break;

      case ESP32C6_HOSTED_TLV_CAP_EXT:
        if (tag_len >= 4U) {
          info.extended_capabilities = esp32c6_get_le32(value);
        }
        break;

      case ESP32C6_HOSTED_TLV_RAW_TP:
        if (tag_len >= 1U) {
          info.raw_tp = value[0];
        }
        break;

      case ESP32C6_HOSTED_TLV_RX_Q_SIZE:
        if (tag_len >= 1U) {
          info.rx_queue_size = value[0];
        }
        break;

      case ESP32C6_HOSTED_TLV_TX_Q_SIZE:
        if (tag_len >= 1U) {
          info.tx_queue_size = value[0];
        }
        break;

      case ESP32C6_HOSTED_TLV_FW_VERSION:
        if (tag_len >= 4U) {
          info.firmware_version = esp32c6_get_le32(value);
        }
        break;

      default:
        break;
    }

    pos = (uint16_t)(pos + 2U + tag_len);
  }

  info.valid = 1U;
  s_init_info = info;
  return 1U;
}

static HAL_StatusTypeDef esp32c6_parse_hosted_frame(const uint8_t *frame,
                                                    uint16_t frame_len,
                                                    ESP32C6_HostedPacket *packet)
{
  const uint8_t *payload;
  uint16_t payload_len;
  uint16_t offset;
  uint16_t checksum;

  if (packet == NULL || frame == NULL || frame_len < ESP32C6_HOSTED_HEADER_SIZE) {
    return HAL_ERROR;
  }

  memset(packet, 0, sizeof(*packet));
  packet->type = ESP32C6_HOSTED_PACKET_BAD_HEADER;
  packet->if_type = frame[0] & 0x0fU;
  packet->if_num = (frame[0] >> 4) & 0x0fU;
  packet->flags = frame[1];
  packet->payload_len = esp32c6_get_le16(&frame[2]);
  packet->payload_offset = esp32c6_get_le16(&frame[4]);
  checksum = esp32c6_get_le16(&frame[6]);
  packet->seq_num = esp32c6_get_le16(&frame[8]);
  packet->throttle_cmd = frame[10] & 0x03U;
  packet->subtype = frame[11];

  payload_len = packet->payload_len;
  offset = packet->payload_offset;

  if (packet->if_type == ESP32C6_HOSTED_IF_MAX && payload_len == 0U) {
    packet->type = ESP32C6_HOSTED_PACKET_IDLE;
    return HAL_OK;
  }

  if (offset < ESP32C6_HOSTED_HEADER_SIZE ||
      offset > frame_len ||
      payload_len > (uint16_t)(frame_len - offset)) {
    return HAL_ERROR;
  }

  if (checksum != 0U) {
    uint8_t checksum_frame[ESP32C6_HOSTED_SPI_FRAME];

    memcpy(checksum_frame, frame, frame_len);
    checksum_frame[6] = 0U;
    checksum_frame[7] = 0U;
    if (esp32c6_checksum(checksum_frame, (uint16_t)(offset + payload_len)) != checksum) {
      return HAL_ERROR;
    }
  }

  payload = &frame[offset];
  packet->payload = payload;

  switch (packet->if_type) {
    case ESP32C6_HOSTED_IF_PRIV:
      packet->type = ESP32C6_HOSTED_PACKET_PRIV_EVENT;
      if (packet->subtype == ESP32C6_HOSTED_PACKET_EVENT &&
          payload_len >= ESP32C6_HOSTED_PRIV_EVENT_HEAD) {
        const ESP32C6_PrivEvent *event = (const ESP32C6_PrivEvent *)payload;

        packet->subtype = event->event_type;
        if ((uint16_t)(ESP32C6_HOSTED_PRIV_EVENT_HEAD + event->event_len) > payload_len) {
          return HAL_ERROR;
        }
        if (event->event_type == ESP32C6_HOSTED_PRIV_INIT) {
          if (esp32c6_parse_init_tlv(event->data, event->event_len) == 0U) {
            return HAL_ERROR;
          }
          packet->type = ESP32C6_HOSTED_PACKET_PRIV_INIT;
        }
      }
      break;

    case ESP32C6_HOSTED_IF_SERIAL:
      packet->type = ESP32C6_HOSTED_PACKET_SERIAL;
      break;

    case ESP32C6_HOSTED_IF_STA:
    case ESP32C6_HOSTED_IF_AP:
    case ESP32C6_HOSTED_IF_ETH:
      packet->type = ESP32C6_HOSTED_PACKET_WIFI;
      break;

    case ESP32C6_HOSTED_IF_HCI:
      packet->type = ESP32C6_HOSTED_PACKET_HCI;
      break;

    default:
      packet->type = ESP32C6_HOSTED_PACKET_UNKNOWN;
      break;
  }

  return HAL_OK;
}

static uint16_t esp32c6_build_host_config(uint8_t *frame, uint16_t frame_len,
                                           uint8_t service_mode)
{
  ESP32C6_PayloadHeader *header;
  ESP32C6_PrivEvent *event;
  uint8_t *pos;
  uint8_t chip_id = s_init_info.valid ? s_init_info.chip_id : 0U;

  if (service_mode > ESP32C6_HOSTED_SERVICE_BLE) {
    service_mode = ESP32C6_HOSTED_SERVICE_NONE;
  }

  if (frame == NULL ||
      frame_len < (uint16_t)(ESP32C6_HOSTED_HEADER_SIZE +
                             ESP32C6_HOSTED_PRIV_EVENT_HEAD +
                             ESP32C6_HOSTED_TX_INIT_TLV_BYTES)) {
    return 0U;
  }

  memset(frame, 0, frame_len);

  header = (ESP32C6_PayloadHeader *)frame;
  header->if_type = ESP32C6_HOSTED_IF_PRIV;
  header->if_num = 0U;
  header->flags = 0U;
  header->len = (uint16_t)(ESP32C6_HOSTED_PRIV_EVENT_HEAD + ESP32C6_HOSTED_TX_INIT_TLV_BYTES);
  header->offset = ESP32C6_HOSTED_HEADER_SIZE;
  header->checksum = 0U;
  header->seq_num = ++s_sequence;
  header->throttle_cmd = 0U;
  header->reserved2 = 0U;
  header->priv_pkt_type = ESP32C6_HOSTED_PACKET_EVENT;

  event = (ESP32C6_PrivEvent *)&frame[ESP32C6_HOSTED_HEADER_SIZE];
  event->event_type = ESP32C6_HOSTED_PRIV_INIT;
  event->event_len = ESP32C6_HOSTED_TX_INIT_TLV_BYTES;
  pos = event->data;

  *pos++ = ESP32C6_HOSTED_HOST_CAPABILITIES;
  *pos++ = 1U;
  *pos++ = ESP32C6_HOSTED_CAP_WLAN_SPI;

  *pos++ = ESP32C6_HOSTED_RCVD_CHIP_ID;
  *pos++ = 1U;
  *pos++ = chip_id;

  *pos++ = ESP32C6_HOSTED_CFG_RAW_TP;
  *pos++ = 1U;
  *pos++ = 0U;

  *pos++ = ESP32C6_HOSTED_CFG_THROTTLE_HIGH;
  *pos++ = 1U;
  *pos++ = 80U;

  *pos++ = ESP32C6_HOSTED_CFG_THROTTLE_LOW;
  *pos++ = 1U;
  *pos++ = 60U;

  *pos++ = ESP32C6_HOSTED_CFG_SERVICE_MODE;
  *pos++ = 1U;
  *pos++ = service_mode;

  const uint16_t total_len = (uint16_t)(ESP32C6_HOSTED_HEADER_SIZE +
                                        ESP32C6_HOSTED_PRIV_EVENT_HEAD +
                                        ESP32C6_HOSTED_TX_INIT_TLV_BYTES);
  header->checksum = esp32c6_checksum(frame, total_len);
  return total_len;
}

static HAL_StatusTypeDef esp32c6_exchange_frame(const uint8_t *tx,
                                                ESP32C6_HostedPacket *packet,
                                                uint32_t timeout_ms)
{
  memset(s_rx_frame, 0, sizeof(s_rx_frame));

  if (tx == NULL) {
    memset(s_tx_frame, 0, sizeof(s_tx_frame));
    tx = s_tx_frame;
  }

  if (ESP32C6_HostTransfer(tx, s_rx_frame, ESP32C6_HOSTED_SPI_FRAME, timeout_ms) != HAL_OK) {
    return HAL_ERROR;
  }

  s_diagnostics.rx_frames++;

  if (esp32c6_parse_hosted_frame(s_rx_frame, ESP32C6_HOSTED_SPI_FRAME, &s_last_packet) != HAL_OK) {
    s_diagnostics.parse_errors++;
    s_diagnostics.last_error = s_last_packet;
    return HAL_ERROR;
  }

  if (s_last_packet.type == ESP32C6_HOSTED_PACKET_IDLE) {
    s_diagnostics.idle_frames++;
  } else if (s_last_packet.type == ESP32C6_HOSTED_PACKET_PRIV_INIT) {
    s_diagnostics.init_frames++;
  }

  if (s_last_packet.type == ESP32C6_HOSTED_PACKET_SERIAL &&
      s_serial_rx_callback != NULL && s_last_packet.payload != NULL) {
    s_serial_rx_callback(s_last_packet.payload, s_last_packet.payload_len, s_serial_rx_user);
  }

  if (packet != NULL) {
    *packet = s_last_packet;
  }

  return HAL_OK;
}

void ESP32C6_BusSetSerialRxCallback(ESP32C6_SerialRxCallback callback, void *user)
{
  s_serial_rx_callback = callback;
  s_serial_rx_user = user;
}

HAL_StatusTypeDef ESP32C6_BusSendSerial(const uint8_t *data, uint16_t len)
{
  ESP32C6_PayloadHeader *header;
  uint16_t total_len;

  if (data == NULL || len == 0U ||
      (uint32_t)len + ESP32C6_HOSTED_HEADER_SIZE > sizeof(s_tx_frame)) {
    return HAL_ERROR;
  }

  memset(s_tx_frame, 0, sizeof(s_tx_frame));
  header = (ESP32C6_PayloadHeader *)s_tx_frame;
  header->if_type = ESP32C6_HOSTED_IF_SERIAL;
  header->if_num = 0U;
  header->flags = 0U;
  header->len = len;
  header->offset = ESP32C6_HOSTED_HEADER_SIZE;
  header->seq_num = ++s_sequence;
  memcpy(&s_tx_frame[ESP32C6_HOSTED_HEADER_SIZE], data, len);
  total_len = (uint16_t)(ESP32C6_HOSTED_HEADER_SIZE + len);
  header->checksum = esp32c6_checksum(s_tx_frame, total_len);
  return esp32c6_exchange_frame(s_tx_frame, NULL, hesp32c6.timeout_ms);
}

static HAL_StatusTypeDef esp32c6_send_hci_command(uint16_t opcode,
                                                   const uint8_t *params,
                                                   uint8_t params_len)
{
  ESP32C6_PayloadHeader *header;
  uint8_t *payload;
  uint16_t total_len;

  if ((uint16_t)(ESP32C6_HOSTED_HEADER_SIZE + 3U + params_len) > sizeof(s_tx_frame)) {
    return HAL_ERROR;
  }

  memset(s_tx_frame, 0, sizeof(s_tx_frame));
  header = (ESP32C6_PayloadHeader *)s_tx_frame;
  header->if_type = ESP32C6_HOSTED_IF_HCI;
  header->if_num = 0U;
  header->len = (uint16_t)(3U + params_len);
  header->offset = ESP32C6_HOSTED_HEADER_SIZE;
  header->seq_num = ++s_sequence;
  header->hci_pkt_type = 0x01U; /* HCI command packet */

  payload = &s_tx_frame[ESP32C6_HOSTED_HEADER_SIZE];
  payload[0] = (uint8_t)(opcode & 0xFFU);
  payload[1] = (uint8_t)(opcode >> 8U);
  payload[2] = params_len;
  if (params_len != 0U && params != NULL) {
    memcpy(&payload[3], params, params_len);
  }

  total_len = (uint16_t)(ESP32C6_HOSTED_HEADER_SIZE + header->len);
  header->checksum = esp32c6_checksum(s_tx_frame, total_len);
  return esp32c6_exchange_frame(s_tx_frame, NULL, hesp32c6.timeout_ms);
}

static HAL_StatusTypeDef esp32c6_configure_ble_advertising(void)
{
  static const char name[] = "UF4DigitalPower";
	const uint8_t no_params = 0U;
  uint8_t local_name[248] = { 0U };
  uint8_t advertising_params[15] =
  {
    0xA0U, 0x00U, 0xA0U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
    0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x07U, 0x00U
  };
  uint8_t advertising_data[32] = { 0U };
  uint8_t advertising_enable = 1U;
  const uint8_t name_len = (uint8_t)(sizeof(name) - 1U);

	/* The controller is newly enabled when this function is called. Reset it
	 * first and let each LE command complete before issuing the next command. */
	if (esp32c6_send_hci_command(0x0C03U, &no_params, 0U) != HAL_OK) {
	  return HAL_ERROR;
	}
	HAL_Delay(100U);

  memcpy(local_name, name, name_len);
  if (esp32c6_send_hci_command(0x0C13U, local_name, sizeof(local_name)) != HAL_OK ||
      esp32c6_send_hci_command(0x2006U, advertising_params, sizeof(advertising_params)) != HAL_OK) {
    return HAL_ERROR;
  }
	HAL_Delay(20U);

  advertising_data[0] = (uint8_t)(3U + name_len);
  advertising_data[1] = 2U;
  advertising_data[2] = 0x01U;
  advertising_data[3] = 0x06U;
  advertising_data[4] = (uint8_t)(name_len + 1U);
  advertising_data[5] = 0x09U;
  memcpy(&advertising_data[6], name, name_len);
  if (esp32c6_send_hci_command(0x2008U, advertising_data, sizeof(advertising_data)) != HAL_OK) {
    return HAL_ERROR;
  }
	HAL_Delay(20U);

  return esp32c6_send_hci_command(0x200AU, &advertising_enable, 1U);
}

const ESP32C6_HostedInitInfo *ESP32C6_BusGetHostedInitInfo(void)
{
  return &s_init_info;
}

const ESP32C6_HostedPacket *ESP32C6_BusGetLastPacket(void)
{
  return &s_last_packet;
}

const ESP32C6_BusDiagnostics *ESP32C6_BusGetDiagnostics(void)
{
  return &s_diagnostics;
}

HAL_StatusTypeDef ESP32C6_BusPoll(uint32_t timeout_ms)
{
  if (ESP32C6_HostGetHandshake() == GPIO_PIN_RESET &&
      ESP32C6_HostGetDataReady() == GPIO_PIN_RESET) {
    if (timeout_ms == 0U) {
      return HAL_TIMEOUT;
    }
    if (ESP32C6_HostWaitHandshake(timeout_ms) != HAL_OK &&
        ESP32C6_HostWaitDataReady(timeout_ms) != HAL_OK) {
      return HAL_TIMEOUT;
    }
  }

  ESP32C6_HostClearDataReadyFlag();
  return esp32c6_exchange_frame(NULL, NULL, hesp32c6.timeout_ms);
}

HAL_StatusTypeDef ESP32C6_BusWaitReady(uint32_t timeout_ms)
{
  const uint32_t start = HAL_GetTick();
  uint8_t host_config_sent = 0U;

  while ((uint32_t)(HAL_GetTick() - start) < timeout_ms) {
    HAL_StatusTypeDef status = ESP32C6_BusPoll(50U);

    if (status == HAL_OK && s_init_info.valid != 0U) {
      if (host_config_sent == 0U) {
        (void)esp32c6_build_host_config(s_tx_frame, sizeof(s_tx_frame),
                                        ESP32C6_HOSTED_SERVICE_NONE);
        (void)esp32c6_exchange_frame(s_tx_frame, NULL, hesp32c6.timeout_ms);
        host_config_sent = 1U;
      }
      return HAL_OK;
    }
  }

  return HAL_TIMEOUT;
}

HAL_StatusTypeDef ESP32C6_BusSetServiceMode(uint8_t mode)
{
  HAL_StatusTypeDef status;

  if (s_init_info.valid == 0U || mode > ESP32C6_HOSTED_SERVICE_BLE) {
    return HAL_ERROR;
  }

  if (esp32c6_build_host_config(s_tx_frame, sizeof(s_tx_frame), mode) == 0U) {
    return HAL_ERROR;
  }

  status = esp32c6_exchange_frame(s_tx_frame, NULL, hesp32c6.timeout_ms);
  if (status != HAL_OK || mode != ESP32C6_HOSTED_SERVICE_BLE) {
    return status;
  }

  HAL_Delay(250U);
  return esp32c6_configure_ble_advertising();
}

HAL_StatusTypeDef ESP32C6_BusCommandExchange(uint8_t command,
                                             const void *request,
                                             uint16_t request_len,
                                             void *response,
                                             uint16_t response_len,
                                             uint16_t *response_used)
{
  (void)command;
  (void)request;
  (void)request_len;
  (void)response;
  (void)response_len;

  if (response_used != NULL) {
    *response_used = 0U;
  }

  return HAL_ERROR;
}

HAL_StatusTypeDef ESP32C6_BusInit(SPI_HandleTypeDef *hspi)
{
  memset(&s_init_info, 0, sizeof(s_init_info));
  memset(&s_last_packet, 0, sizeof(s_last_packet));
  memset(&s_diagnostics, 0, sizeof(s_diagnostics));
  s_sequence = 0U;
  s_serial_rx_callback = NULL;
  s_serial_rx_user = NULL;
  return ESP32C6_HostInit(hspi);
}

HAL_StatusTypeDef ESP32C6_BusPing(void)
{
  return ESP32C6_BusWaitReady(1000U);
}

HAL_StatusTypeDef ESP32C6_BusGetInfo(ESP32C6_BusInfo *info)
{
  if (info == NULL || s_init_info.valid == 0U) {
    return HAL_ERROR;
  }

  memset(info, 0, sizeof(*info));
  info->chip_id = s_init_info.chip_id;
  info->fw_version = s_init_info.firmware_version;
  (void)memcpy(info->target, "esp32s3", 7U);
  (void)memcpy(info->idf_version, "esp-hosted", 10U);
  return HAL_OK;
}

HAL_StatusTypeDef ESP32C6_BusReset(void)
{
  return HAL_ERROR;
}

HAL_StatusTypeDef ESP32C6_BusWriteGpio(uint8_t pin, GPIO_PinState level)
{
  (void)pin;
  (void)level;
  return HAL_ERROR;
}

HAL_StatusTypeDef ESP32C6_BusReadGpio(uint8_t pin, GPIO_PinState *level)
{
  (void)pin;
  if (level != NULL) {
    *level = GPIO_PIN_RESET;
  }
  return HAL_ERROR;
}
