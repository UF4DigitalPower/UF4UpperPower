#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_set_channel(uint8_t primary, wifi_second_chan_t second)
{
  ESP32C6_WifiChannelPayload payload;

  payload.primary = primary;
  payload.second = second;
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_CHANNEL, &payload, sizeof(payload), NULL, 0U);
}
