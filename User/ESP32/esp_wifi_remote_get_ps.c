#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_get_ps(wifi_ps_type_t *type)
{
  if (type == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_GET_PS, NULL, 0U, type, sizeof(*type));
}
