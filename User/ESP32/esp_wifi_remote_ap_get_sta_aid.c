#include "esp32c6_wifi_remote_internal.h"

#include <string.h>

esp_err_t esp_wifi_remote_ap_get_sta_aid(const uint8_t mac[6], uint16_t *aid)
{
  ESP32C6_WifiStaAidPayload payload;
  esp_err_t ret;

  if ((mac == NULL) || (aid == NULL)) {
    return ESP_FAIL;
  }
  memcpy(payload.mac, mac, 6U);
  payload.aid = 0U;
  ret = esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_AP_GET_STA_AID, &payload, sizeof(payload), &payload, sizeof(payload));
  if (ret == ESP_OK) {
    *aid = payload.aid;
  }
  return ret;
}
