#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_sta_get_aid(uint16_t *aid)
{
  if (aid == NULL) {
    return ESP_FAIL;
  }
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_STA_GET_AID, NULL, 0U, aid, sizeof(*aid));
}
