#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_get_protocol(wifi_interface_t ifx, uint8_t *protocol_bitmap)
{
  if (protocol_bitmap == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_GET_PROTOCOL, &ifx, sizeof(ifx), protocol_bitmap, sizeof(*protocol_bitmap));
}
