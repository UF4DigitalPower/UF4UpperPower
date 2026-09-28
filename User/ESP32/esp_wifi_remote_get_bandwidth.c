#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_get_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t *bw)
{
  if (bw == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_GET_BANDWIDTH, &ifx, sizeof(ifx), bw, sizeof(*bw));
}
