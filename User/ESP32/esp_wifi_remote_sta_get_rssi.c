#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_sta_get_rssi(int *rssi)
{
  if (rssi == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_STA_GET_RSSI, NULL, 0U, rssi, sizeof(*rssi));
}
