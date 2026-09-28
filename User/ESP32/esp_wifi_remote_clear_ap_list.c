#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_clear_ap_list(void)
{
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_CLEAR_AP_LIST, NULL, 0U, NULL, 0U);
}
