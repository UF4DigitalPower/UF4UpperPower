#ifndef __ESP32C6_BUS_H__
#define __ESP32C6_BUS_H__

#include "esp32c6_host.h"
#include "esp32c6_bus_proto.h"

#ifdef __cplusplus
extern "C" {
#endif

HAL_StatusTypeDef ESP32C6_BusInit(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef ESP32C6_BusPoll(uint32_t timeout_ms);
HAL_StatusTypeDef ESP32C6_BusWaitReady(uint32_t timeout_ms);
HAL_StatusTypeDef ESP32C6_BusSetServiceMode(uint8_t mode);
typedef void (*ESP32C6_SerialRxCallback)(const uint8_t *data, uint16_t len, void *user);
void ESP32C6_BusSetSerialRxCallback(ESP32C6_SerialRxCallback callback, void *user);
HAL_StatusTypeDef ESP32C6_BusSendSerial(const uint8_t *data, uint16_t len);
const ESP32C6_HostedInitInfo *ESP32C6_BusGetHostedInitInfo(void);
const ESP32C6_HostedPacket *ESP32C6_BusGetLastPacket(void);

typedef struct
{
  uint32_t rx_frames;
  uint32_t parse_errors;
  uint32_t idle_frames;
  uint32_t init_frames;
  ESP32C6_HostedPacket last_error;
} ESP32C6_BusDiagnostics;

const ESP32C6_BusDiagnostics *ESP32C6_BusGetDiagnostics(void);
HAL_StatusTypeDef ESP32C6_BusCommandExchange(uint8_t command,
                                             const void *request,
                                             uint16_t request_len,
                                             void *response,
                                             uint16_t response_len,
                                             uint16_t *response_used);
HAL_StatusTypeDef ESP32C6_BusPing(void);
HAL_StatusTypeDef ESP32C6_BusGetInfo(ESP32C6_BusInfo *info);
HAL_StatusTypeDef ESP32C6_BusReset(void);
HAL_StatusTypeDef ESP32C6_BusWriteGpio(uint8_t pin, GPIO_PinState level);
HAL_StatusTypeDef ESP32C6_BusReadGpio(uint8_t pin, GPIO_PinState *level);

#ifdef __cplusplus
}
#endif

#endif
