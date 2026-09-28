#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_set_bandwidths(wifi_interface_t ifx, wifi_bandwidths_t *bw)
{
  ESP32C6_WifiBandwidthsPayload payload;

  if (bw == NULL) {
    return ESP_FAIL;
  }
  payload.interface = ifx;
  payload.bandwidths = *bw;
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_BANDWIDTHS, &payload, sizeof(payload), NULL, 0U);
}
