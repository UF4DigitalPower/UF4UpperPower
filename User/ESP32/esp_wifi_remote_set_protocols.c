#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_set_protocols(wifi_interface_t ifx, wifi_protocols_t *protocols)
{
  ESP32C6_WifiProtocolsPayload payload;

  if (protocols == NULL) {
    return ESP_FAIL;
  }
  payload.interface = ifx;
  payload.protocols = *protocols;
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_PROTOCOLS, &payload, sizeof(payload), NULL, 0U);
}
