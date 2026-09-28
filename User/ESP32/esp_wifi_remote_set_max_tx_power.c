#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_set_max_tx_power(int8_t power)
{
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_MAX_TX_POWER, &power, sizeof(power), NULL, 0U);
}
