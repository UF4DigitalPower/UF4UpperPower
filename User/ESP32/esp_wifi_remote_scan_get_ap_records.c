#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records)
{
  if ((number == NULL) || (ap_records == NULL)) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SCAN_GET_AP_RECORDS, number, sizeof(*number), ap_records, (uint16_t)(sizeof(wifi_ap_record_t) * (*number)));
}
