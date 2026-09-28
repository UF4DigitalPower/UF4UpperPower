#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_get_band_mode(wifi_band_mode_t *band_mode)
{
  if (band_mode == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_GET_BAND_MODE, NULL, 0U, band_mode, sizeof(*band_mode));
}
