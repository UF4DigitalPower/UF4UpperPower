#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_clear_fast_connect(void)
{
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_CLEAR_FAST_CONNECT, NULL, 0U, NULL, 0U);
}
