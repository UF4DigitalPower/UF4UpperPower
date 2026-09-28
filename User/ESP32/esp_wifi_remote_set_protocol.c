#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_set_protocol(wifi_interface_t ifx, uint8_t protocol_bitmap)
{
  ESP32C6_WifiInterfaceValuePayload payload;

  payload.interface = ifx;
  payload.value = protocol_bitmap;
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_PROTOCOL, &payload, sizeof(payload), NULL, 0U);
}
