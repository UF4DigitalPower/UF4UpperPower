#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_get_config(wifi_interface_t interface, wifi_config_t *conf)
{
  if (conf == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_GET_CONFIG, &interface, sizeof(interface), conf, sizeof(*conf));
}
