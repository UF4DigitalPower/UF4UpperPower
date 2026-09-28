#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_get_mode(wifi_mode_t *mode)
{
  if (mode == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_GET_MODE, NULL, 0U, mode, sizeof(*mode));
}
