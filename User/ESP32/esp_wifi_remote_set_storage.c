#include "esp32c6_wifi_remote_internal.h"

esp_err_t esp_wifi_remote_set_storage(wifi_storage_t storage)
{
  return esp32c6_wifi_remote_command(ESP32C6_BUS_CMD_WIFI_SET_STORAGE, &storage, sizeof(storage), NULL, 0U);
}
