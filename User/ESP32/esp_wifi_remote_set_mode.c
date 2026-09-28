#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_set_mode(wifi_mode_t mode)
{
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_MODE, &mode, sizeof(mode), NULL, 0U);
}
