#ifndef __ESP32C6_WIFI_REMOTE_H__
#define __ESP32C6_WIFI_REMOTE_H__

#include "esp32c6_bus.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t esp_err_t;

#ifndef ESP_OK
#define ESP_OK    0
#endif

#ifndef ESP_FAIL
#define ESP_FAIL -1
#endif

typedef uint32_t wifi_mode_t;
typedef uint32_t wifi_interface_t;
typedef uint32_t wifi_ps_type_t;
typedef uint32_t wifi_storage_t;
typedef uint32_t wifi_bandwidth_t;
typedef uint32_t wifi_second_chan_t;
typedef uint32_t wifi_phy_mode_t;
typedef uint32_t wifi_band_t;
typedef uint32_t wifi_band_mode_t;

typedef struct
{
  uint32_t magic;
} wifi_init_config_t;

typedef struct
{
  uint8_t ssid[32];
  uint8_t password[64];
  uint8_t channel;
  uint8_t authmode;
  uint8_t reserved[30];
} wifi_sta_config_t;

typedef struct
{
  uint8_t ssid[32];
  uint8_t password[64];
  uint8_t ssid_len;
  uint8_t channel;
  uint8_t authmode;
  uint8_t max_connection;
  uint8_t reserved[28];
} wifi_ap_config_t;

typedef union
{
  wifi_sta_config_t sta;
  wifi_ap_config_t ap;
} wifi_config_t;

typedef struct
{
  uint8_t ssid[32];
  uint8_t bssid[6];
  uint8_t channel;
  bool show_hidden;
  uint32_t scan_type;
  uint32_t scan_time_active_min;
  uint32_t scan_time_active_max;
  uint32_t scan_time_passive;
} wifi_scan_config_t;

typedef struct
{
  uint32_t scan_time_active_min;
  uint32_t scan_time_active_max;
  uint32_t scan_time_passive;
  uint32_t home_chan_dwell_time;
} wifi_scan_default_params_t;

typedef struct
{
  uint8_t bssid[6];
  uint8_t ssid[33];
  uint8_t primary;
  int8_t rssi;
  uint32_t authmode;
} wifi_ap_record_t;

typedef struct
{
  uint8_t mac[6];
  int8_t rssi;
  uint8_t phy_11b;
  uint8_t phy_11g;
  uint8_t phy_11n;
  uint8_t phy_lr;
} wifi_sta_info_t;

typedef struct
{
  uint32_t num;
  wifi_sta_info_t sta[10];
} wifi_sta_list_t;

typedef struct
{
  char cc[3];
  uint8_t schan;
  uint8_t nchan;
  int8_t max_tx_power;
  uint32_t policy;
} wifi_country_t;

typedef struct
{
  uint32_t protocol_bitmap;
} wifi_protocols_t;

typedef struct
{
  wifi_bandwidth_t bandwidth;
} wifi_bandwidths_t;

esp_err_t esp_wifi_remote_init(const wifi_init_config_t *arg);
esp_err_t esp_wifi_remote_deinit(void);
esp_err_t esp_wifi_remote_set_mode(wifi_mode_t mode);
esp_err_t esp_wifi_remote_get_mode(wifi_mode_t *mode);
esp_err_t esp_wifi_remote_start(void);
esp_err_t esp_wifi_remote_stop(void);
esp_err_t esp_wifi_remote_connect(void);
esp_err_t esp_wifi_remote_disconnect(void);
esp_err_t esp_wifi_remote_set_config(wifi_interface_t interface, wifi_config_t *conf);
esp_err_t esp_wifi_remote_get_config(wifi_interface_t interface, wifi_config_t *conf);
esp_err_t esp_wifi_remote_get_mac(wifi_interface_t mode, uint8_t mac[6]);
esp_err_t esp_wifi_remote_set_mac(wifi_interface_t mode, const uint8_t mac[6]);
esp_err_t esp_wifi_remote_scan_start(const wifi_scan_config_t *config, bool block);
esp_err_t esp_wifi_remote_set_scan_parameters(const wifi_scan_default_params_t *config);
esp_err_t esp_wifi_remote_get_scan_parameters(wifi_scan_default_params_t *config);
esp_err_t esp_wifi_remote_scan_stop(void);
esp_err_t esp_wifi_remote_scan_get_ap_num(uint16_t *number);
esp_err_t esp_wifi_remote_scan_get_ap_record(wifi_ap_record_t *ap_record);
esp_err_t esp_wifi_remote_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records);
esp_err_t esp_wifi_remote_clear_ap_list(void);
esp_err_t esp_wifi_remote_restore(void);
esp_err_t esp_wifi_remote_clear_fast_connect(void);
esp_err_t esp_wifi_remote_deauth_sta(uint16_t aid);
esp_err_t esp_wifi_remote_sta_get_ap_info(wifi_ap_record_t *ap_info);
esp_err_t esp_wifi_remote_set_ps(wifi_ps_type_t type);
esp_err_t esp_wifi_remote_get_ps(wifi_ps_type_t *type);
esp_err_t esp_wifi_remote_set_storage(wifi_storage_t storage);
esp_err_t esp_wifi_remote_set_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t bw);
esp_err_t esp_wifi_remote_get_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t *bw);
esp_err_t esp_wifi_remote_set_channel(uint8_t primary, wifi_second_chan_t second);
esp_err_t esp_wifi_remote_get_channel(uint8_t *primary, wifi_second_chan_t *second);
esp_err_t esp_wifi_remote_set_country_code(const char *country, bool ieee80211d_enabled);
esp_err_t esp_wifi_remote_get_country_code(char *country);
esp_err_t esp_wifi_remote_set_country(const wifi_country_t *country);
esp_err_t esp_wifi_remote_get_country(wifi_country_t *country);
esp_err_t esp_wifi_remote_ap_get_sta_list(wifi_sta_list_t *sta);
esp_err_t esp_wifi_remote_ap_get_sta_aid(const uint8_t mac[6], uint16_t *aid);
esp_err_t esp_wifi_remote_sta_get_rssi(int *rssi);
esp_err_t esp_wifi_remote_set_protocol(wifi_interface_t ifx, uint8_t protocol_bitmap);
esp_err_t esp_wifi_remote_get_protocol(wifi_interface_t ifx, uint8_t *protocol_bitmap);
esp_err_t esp_wifi_remote_set_max_tx_power(int8_t power);
esp_err_t esp_wifi_remote_get_max_tx_power(int8_t *power);
esp_err_t esp_wifi_remote_sta_get_negotiated_phymode(wifi_phy_mode_t *phymode);
esp_err_t esp_wifi_remote_sta_get_aid(uint16_t *aid);
esp_err_t esp_wifi_remote_set_inactive_time(wifi_interface_t ifx, uint16_t sec);
esp_err_t esp_wifi_remote_get_inactive_time(wifi_interface_t ifx, uint16_t *sec);
esp_err_t esp_wifi_remote_set_band(wifi_band_t band);
esp_err_t esp_wifi_remote_get_band(wifi_band_t *band);
esp_err_t esp_wifi_remote_set_band_mode(wifi_band_mode_t band_mode);
esp_err_t esp_wifi_remote_get_band_mode(wifi_band_mode_t *band_mode);
esp_err_t esp_wifi_remote_set_protocols(wifi_interface_t ifx, wifi_protocols_t *protocols);
esp_err_t esp_wifi_remote_get_protocols(wifi_interface_t ifx, wifi_protocols_t *protocols);
esp_err_t esp_wifi_remote_set_bandwidths(wifi_interface_t ifx, wifi_bandwidths_t *bw);
esp_err_t esp_wifi_remote_get_bandwidths(wifi_interface_t ifx, wifi_bandwidths_t *bw);
esp_err_t esp_wifi_remote_sta_enterprise_enable(void);
esp_err_t esp_wifi_remote_sta_enterprise_disable(void);
esp_err_t esp_wifi_remote_set_okc_support(bool enable);

#ifdef __cplusplus
}
#endif

#endif
