#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_deauth_sta(uint16_t aid)
{
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_DEAUTH_STA, &aid, sizeof(aid), NULL, 0U);
}
