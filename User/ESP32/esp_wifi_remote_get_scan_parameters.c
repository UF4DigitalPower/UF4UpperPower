#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_get_scan_parameters(wifi_scan_default_params_t *config)
{
  if (config == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_GET_SCAN_PARAMETERS, NULL, 0U, config, sizeof(*config));
}
