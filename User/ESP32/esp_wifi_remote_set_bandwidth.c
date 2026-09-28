#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_set_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t bw)
{
  ESP32C6_WifiInterfaceValuePayload payload;

  payload.interface = ifx;
  payload.value = bw;
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_BANDWIDTH, &payload, sizeof(payload), NULL, 0U);
}
