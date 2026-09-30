#include "esp32c6_host.h"
#include "board_lvgl.h"

#include <string.h>

ESP32C6_HostHandleTypeDef hesp32c6;

static HAL_StatusTypeDef esp32c6_validate(void)
{
  return (hesp32c6.hspi != NULL) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef ESP32C6_HostInit(SPI_HandleTypeDef *hspi)
{
  if (hspi == NULL) {
    return HAL_ERROR;
  }

  memset(&hesp32c6, 0, sizeof(hesp32c6));
  hesp32c6.hspi = hspi;
  hesp32c6.cs_port = WIFI_NSS_GPIO_Port;
  hesp32c6.cs_pin = WIFI_NSS_Pin;
  hesp32c6.host_sig_port = WIFI_HS_GPIO_Port;
  hesp32c6.host_sig_pin = WIFI_HS_Pin;
  hesp32c6.data_ready_port = WIFI_DR_GPIO_Port;
  hesp32c6.data_ready_pin = WIFI_DR_Pin;
  hesp32c6.timeout_ms = 100U;

  HAL_GPIO_WritePin(hesp32c6.cs_port, hesp32c6.cs_pin, GPIO_PIN_SET);
  hesp32c6.host_sig_irq = 0U;
  hesp32c6.data_ready_irq = 0U;

  return HAL_OK;
}

void ESP32C6_HostDeInit(void)
{
  memset(&hesp32c6, 0, sizeof(hesp32c6));
}

HAL_StatusTypeDef ESP32C6_HostResetPulse(uint32_t low_ms, uint32_t high_ms)
{
  if (esp32c6_validate() != HAL_OK) {
    return HAL_ERROR;
  }

  (void)low_ms;
  (void)high_ms;
  return HAL_OK;
}

HAL_StatusTypeDef ESP32C6_HostSetSignal(GPIO_PinState state)
{
  if (esp32c6_validate() != HAL_OK) {
    return HAL_ERROR;
  }

  (void)state;
  return HAL_OK;
}

GPIO_PinState ESP32C6_HostGetDataReady(void)
{
  if (esp32c6_validate() != HAL_OK) {
    return GPIO_PIN_RESET;
  }

  if (hesp32c6.data_ready_irq) {
    return GPIO_PIN_SET;
  }

  return HAL_GPIO_ReadPin(hesp32c6.data_ready_port, hesp32c6.data_ready_pin);
}

GPIO_PinState ESP32C6_HostGetHandshake(void)
{
  if (esp32c6_validate() != HAL_OK) {
    return GPIO_PIN_RESET;
  }

  if (hesp32c6.host_sig_irq) {
    return GPIO_PIN_SET;
  }

  return HAL_GPIO_ReadPin(hesp32c6.host_sig_port, hesp32c6.host_sig_pin);
}

void ESP32C6_HostClearDataReadyFlag(void)
{
  hesp32c6.data_ready_irq = 0U;
}

void ESP32C6_HostClearHandshakeFlag(void)
{
  hesp32c6.host_sig_irq = 0U;
}

HAL_StatusTypeDef ESP32C6_HostWaitDataReady(uint32_t timeout_ms)
{
  uint32_t start;

  if (esp32c6_validate() != HAL_OK) {
    return HAL_ERROR;
  }

  start = HAL_GetTick();
  while (ESP32C6_HostGetDataReady() == GPIO_PIN_RESET) {
    if ((HAL_GetTick() - start) >= timeout_ms) {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

HAL_StatusTypeDef ESP32C6_HostWaitHandshake(uint32_t timeout_ms)
{
  uint32_t start;

  if (esp32c6_validate() != HAL_OK) {
    return HAL_ERROR;
  }

  start = HAL_GetTick();
  while (ESP32C6_HostGetHandshake() == GPIO_PIN_RESET) {
    if ((HAL_GetTick() - start) >= timeout_ms) {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

HAL_StatusTypeDef ESP32C6_HostTransfer(const uint8_t *tx, uint8_t *rx,
                                       uint16_t len, uint32_t timeout_ms)
{
  uint8_t dummy_tx[32];
  uint8_t dummy_rx[32];
  uint16_t remain = len;
  uint16_t offset = 0U;
  HAL_StatusTypeDef status = HAL_OK;
  const uint8_t *tx_ptr = tx;
  uint8_t *rx_ptr = rx;

  if (esp32c6_validate() != HAL_OK) {
    return HAL_ERROR;
  }

  if (len == 0U) {
    return HAL_OK;
  }

  if (ESP32C6_HostWaitHandshake(timeout_ms) != HAL_OK) {
    return HAL_TIMEOUT;
  }
  ESP32C6_HostClearHandshakeFlag();

  HAL_GPIO_WritePin(hesp32c6.cs_port, hesp32c6.cs_pin, GPIO_PIN_RESET);

  while (remain != 0U) {
    uint16_t chunk = (remain > sizeof(dummy_tx)) ? (uint16_t)sizeof(dummy_tx) : remain;
    const uint8_t *cur_tx = tx_ptr;
    uint8_t *cur_rx = rx_ptr;

    if (cur_tx == NULL) {
      memset(dummy_tx, 0, chunk);
      cur_tx = dummy_tx;
    }

    if (cur_rx == NULL) {
      cur_rx = dummy_rx;
    }

    status = HAL_SPI_TransmitReceive(hesp32c6.hspi,
                                     (uint8_t *)cur_tx,
                                     cur_rx,
                                     chunk,
                                     timeout_ms);
    if (status != HAL_OK) {
      break;
    }

    if (tx_ptr != NULL) {
      tx_ptr += chunk;
    }
    if (rx_ptr != NULL) {
      rx_ptr += chunk;
    }
    offset = (uint16_t)(offset + chunk);
    remain = (uint16_t)(len - offset);
  }

  HAL_GPIO_WritePin(hesp32c6.cs_port, hesp32c6.cs_pin, GPIO_PIN_SET);
  return status;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == TOUCH_INT_Pin) {
    Board_LVGL_TouchIrqNotify();
  }
  if (GPIO_Pin == WIFI_HS_Pin) {
    hesp32c6.host_sig_irq = 1U;
  }
  if (GPIO_Pin == WIFI_DR_Pin) {
    hesp32c6.data_ready_irq = 1U;
  }
}
