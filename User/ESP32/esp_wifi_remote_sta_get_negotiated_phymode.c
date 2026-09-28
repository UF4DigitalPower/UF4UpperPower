#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_sta_get_negotiated_phymode(wifi_phy_mode_t *phymode)
{
  if (phymode == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_STA_GET_NEGOTIATED_PHYMODE, NULL, 0U, phymode, sizeof(*phymode));
}
