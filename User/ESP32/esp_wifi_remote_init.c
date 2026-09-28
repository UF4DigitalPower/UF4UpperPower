#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_init(const wifi_init_config_t *arg)
{
  if (arg == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_INIT, arg, sizeof(*arg), NULL, 0U);
}
