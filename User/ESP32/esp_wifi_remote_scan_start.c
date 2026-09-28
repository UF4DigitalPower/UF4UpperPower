#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_scan_start(const wifi_scan_config_t *config, bool block)
{
  ESP32C6_WifiScanStartPayload payload;

  if (config == NULL) {
    return ESP_FAIL;
  }
  payload.config = *config;
  payload.block = block ? 1U : 0U;
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SCAN_START, &payload, sizeof(payload), NULL, 0U);
}
