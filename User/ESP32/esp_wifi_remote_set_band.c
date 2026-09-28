#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_set_band(wifi_band_t band)
{
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_BAND, &band, sizeof(band), NULL, 0U);
}
