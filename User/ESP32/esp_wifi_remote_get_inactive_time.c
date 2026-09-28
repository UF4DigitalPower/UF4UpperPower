#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_get_inactive_time(wifi_interface_t ifx, uint16_t *sec)
{
  if (sec == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_GET_INACTIVE_TIME, &ifx, sizeof(ifx), sec, sizeof(*sec));
}
