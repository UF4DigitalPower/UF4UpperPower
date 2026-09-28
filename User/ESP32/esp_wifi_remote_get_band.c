#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_get_band(wifi_band_t *band)
{
  if (band == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_GET_BAND, NULL, 0U, band, sizeof(*band));
}
