#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp32c6_wifi_remote_command(uint8_t command,
                                      const void *request,
                                      uint16_t request_len,
                                      void *response,
                                      uint16_t response_len)
{
  if (ESP32C6_BusCommandExchange(command, request, request_len, response, response_len, NULL) != HAL_OK) {
    return ESP_FAIL;
  }

  return ESP_OK;
}
