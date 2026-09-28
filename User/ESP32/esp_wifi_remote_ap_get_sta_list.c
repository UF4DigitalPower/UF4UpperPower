#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_ap_get_sta_list(wifi_sta_list_t *sta)
{
  if (sta == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_AP_GET_STA_LIST, NULL, 0U, sta, sizeof(*sta));
}
