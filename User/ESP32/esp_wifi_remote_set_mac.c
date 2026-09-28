#include "esp32c6_wifi_remote_internal.h"

#include <string.h>

esp_err_t esp_wifi_remote_set_mac(wifi_interface_t mode, const uint8_t mac[6])
{
  ESP32C6_WifiMacPayload payload;

  if (mac == NULL) {
    return ESP_FAIL;
  }
  payload.interface = mode;
  memcpy(payload.mac, mac, 6U);
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_MAC, &payload, sizeof(payload), NULL, 0U);
}
