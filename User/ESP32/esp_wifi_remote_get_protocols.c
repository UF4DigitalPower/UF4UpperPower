#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_get_protocols(wifi_interface_t ifx, wifi_protocols_t *protocols)
{
  if (protocols == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_GET_PROTOCOLS, &ifx, sizeof(ifx), protocols, sizeof(*protocols));
}
