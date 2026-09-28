#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_set_band_mode(wifi_band_mode_t band_mode)
{
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_BAND_MODE, &band_mode, sizeof(band_mode), NULL, 0U);
}
