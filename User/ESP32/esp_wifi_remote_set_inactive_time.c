#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_set_inactive_time(wifi_interface_t ifx, uint16_t sec)
{
  ESP32C6_WifiInactiveTimePayload payload;

  payload.interface = ifx;
  payload.seconds = sec;
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_INACTIVE_TIME, &payload, sizeof(payload), NULL, 0U);
}
