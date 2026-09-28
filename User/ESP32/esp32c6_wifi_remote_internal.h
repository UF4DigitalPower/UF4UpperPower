#ifndef __ESP32C6_WIFI_REMOTE_INTERNAL_H__
#define __ESP32C6_WIFI_REMOTE_INTERNAL_H__

#include "esp32c6_wifi_remote.h"

typedef struct __attribute__((packed))
{
  wifi_interface_t interface;
  wifi_config_t config;
} ESP32C6_WifiConfigPayload;

typedef struct __attribute__((packed))
{
  wifi_interface_t interface;
  uint8_t mac[6];
} ESP32C6_WifiMacPayload;

typedef struct __attribute__((packed))
{
  wifi_scan_config_t config;
  uint8_t block;
} ESP32C6_WifiScanStartPayload;

typedef struct __attribute__((packed))
{
  uint16_t number;
  wifi_ap_record_t records[0];
} ESP32C6_WifiApRecordsPayload;

typedef struct __attribute__((packed))
{
  wifi_interface_t interface;
  uint32_t value;
} ESP32C6_WifiInterfaceValuePayload;

typedef struct __attribute__((packed))
{
  uint8_t primary;
  wifi_second_chan_t second;
} ESP32C6_WifiChannelPayload;

typedef struct __attribute__((packed))
{
  char country[4];
  uint8_t ieee80211d_enabled;
} ESP32C6_WifiCountryCodePayload;

typedef struct __attribute__((packed))
{
  uint8_t mac[6];
  uint16_t aid;
} ESP32C6_WifiStaAidPayload;

typedef struct __attribute__((packed))
{
  wifi_interface_t interface;
  uint16_t seconds;
} ESP32C6_WifiInactiveTimePayload;

typedef struct __attribute__((packed))
{
  wifi_interface_t interface;
  wifi_protocols_t protocols;
} ESP32C6_WifiProtocolsPayload;

typedef struct __attribute__((packed))
{
  wifi_interface_t interface;
  wifi_bandwidths_t bandwidths;
} ESP32C6_WifiBandwidthsPayload;

esp_err_t esp32c6_wifi_remote_command(uint8_t command,
                                      const void *request,
                                      uint16_t request_len,
                                      void *response,
                                      uint16_t response_len);

#endif
