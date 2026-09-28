#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_scan_get_ap_record(wifi_ap_record_t *ap_record)
{
  if (ap_record == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SCAN_GET_AP_RECORD, NULL, 0U, ap_record, sizeof(*ap_record));
}
