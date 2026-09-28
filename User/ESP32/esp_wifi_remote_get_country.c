#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_get_country(wifi_country_t *country)
{
  if (country == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_GET_COUNTRY, NULL, 0U, country, sizeof(*country));
}
