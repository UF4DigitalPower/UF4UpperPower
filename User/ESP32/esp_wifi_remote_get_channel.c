#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_get_channel(uint8_t *primary, wifi_second_chan_t *second)
{
  ESP32C6_WifiChannelPayload payload;
  esp_err_t ret;

  if ((primary == NULL) || (second == NULL)) {
    return ESP_FAIL;
  }
  ret = esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_GET_CHANNEL, NULL, 0U, &payload, sizeof(payload));
  if (ret == ESP_OK) {
    *primary = payload.primary;
    *second = payload.second;
  }
  return ret;
}
