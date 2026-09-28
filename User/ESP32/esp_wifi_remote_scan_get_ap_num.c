#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_scan_get_ap_num(uint16_t *number)
{
  if (number == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SCAN_GET_AP_NUM, NULL, 0U, number, sizeof(*number));
}
