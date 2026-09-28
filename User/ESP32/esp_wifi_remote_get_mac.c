#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_get_mac(wifi_interface_t mode, uint8_t mac[6])
{
  if (mac == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_GET_MAC, &mode, sizeof(mode), mac, 6U);
}
