#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_set_country(const wifi_country_t *country)
{
  if (country == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_COUNTRY, country, sizeof(*country), NULL, 0U);
}
