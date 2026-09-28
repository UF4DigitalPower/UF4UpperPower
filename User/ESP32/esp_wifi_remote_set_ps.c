#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_set_ps(wifi_ps_type_t type)
{
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_PS, &type, sizeof(type), NULL, 0U);
}
