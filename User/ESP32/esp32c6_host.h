#ifndef __ESP32C6_HOST_H__
#define __ESP32C6_HOST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "spi.h"

typedef struct
{
  SPI_HandleTypeDef *hspi;
  GPIO_TypeDef *cs_port;
  uint16_t cs_pin;
  GPIO_TypeDef *host_sig_port;
  uint16_t host_sig_pin;
  GPIO_TypeDef *data_ready_port;
  uint16_t data_ready_pin;
  volatile uint8_t host_sig_irq;
  volatile uint8_t data_ready_irq;
  uint32_t timeout_ms;
} ESP32C6_HostHandleTypeDef;

extern ESP32C6_HostHandleTypeDef hesp32c6;

HAL_StatusTypeDef ESP32C6_HostInit(SPI_HandleTypeDef *hspi);
void ESP32C6_HostDeInit(void);

HAL_StatusTypeDef ESP32C6_HostResetPulse(uint32_t low_ms, uint32_t high_ms);
HAL_StatusTypeDef ESP32C6_HostSetSignal(GPIO_PinState state);
GPIO_PinState ESP32C6_HostGetDataReady(void);
GPIO_PinState ESP32C6_HostGetHandshake(void);
void ESP32C6_HostClearDataReadyFlag(void);
void ESP32C6_HostClearHandshakeFlag(void);
HAL_StatusTypeDef ESP32C6_HostWaitDataReady(uint32_t timeout_ms);
HAL_StatusTypeDef ESP32C6_HostWaitHandshake(uint32_t timeout_ms);

HAL_StatusTypeDef ESP32C6_HostTransfer(const uint8_t *tx, uint8_t *rx,
                                       uint16_t len, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
