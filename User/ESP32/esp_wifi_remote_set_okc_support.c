#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_set_okc_support(bool enable)
{
  uint8_t payload = enable ? 1U : 0U;

  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_OKC_SUPPORT, &payload, sizeof(payload), NULL, 0U);
}
