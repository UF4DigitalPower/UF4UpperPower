#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_sta_get_ap_info(wifi_ap_record_t *ap_info)
{
  if (ap_info == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_STA_GET_AP_INFO, NULL, 0U, ap_info, sizeof(*ap_info));
}
