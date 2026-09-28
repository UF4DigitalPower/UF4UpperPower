#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_set_scan_parameters(const wifi_scan_default_params_t *config)
{
  if (config == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_SCAN_PARAMETERS, config, sizeof(*config), NULL, 0U);
}
