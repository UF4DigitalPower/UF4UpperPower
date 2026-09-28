#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_set_config(wifi_interface_t interface, wifi_config_t *conf)
{
  ESP32C6_WifiConfigPayload payload;

  if (conf == NULL) {
    return ESP_FAIL;
  }
  payload.interface = interface;
  payload.config = *conf;
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_CONFIG, &payload, sizeof(payload), NULL, 0U);
}
