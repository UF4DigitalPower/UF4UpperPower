#include "esp32c6_wifi_remote_internal.h"

#include <string.h>

esp_err_t esp_wifi_remote_set_country_code(const char *country, bool ieee80211d_enabled)
{
  ESP32C6_WifiCountryCodePayload payload;

  if (country == NULL) {
    return ESP_FAIL;
  }
  memset(&payload, 0, sizeof(payload));
  memcpy(payload.country, country, 3U);
  payload.ieee80211d_enabled = ieee80211d_enabled ? 1U : 0U;
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_COUNTRY_CODE, &payload, sizeof(payload), NULL, 0U);
}
