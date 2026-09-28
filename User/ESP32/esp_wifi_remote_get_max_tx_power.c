#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_get_max_tx_power(int8_t *power)
{
  if (power == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_GET_MAX_TX_POWER, NULL, 0U, power, sizeof(*power));
}
