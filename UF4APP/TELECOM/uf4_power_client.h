#ifndef UF4_POWER_CLIENT_H
#define UF4_POWER_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/**
  * @brief Initialize UF4 protocol parser and USART1 RX interrupts.
  */
void UF4PowerClient_Init(void);

/**
  * @brief Poll USART/RX ring and process a bounded number of UF4 bytes.
  */
void UF4PowerClient_Tick(void);

/**
  * @brief Write several U16 UF4 registers in one frame.
  */
bool UF4PowerClient_WriteU16Pairs(const uint8_t *ids, const uint16_t *values, uint8_t count);

/**
  * @brief Write one U16 UF4 register.
  */
bool UF4PowerClient_WriteU16(uint8_t id, uint16_t value);

/**
  * @brief Request several U16 UF4 registers in one frame.
  */
bool UF4PowerClient_ReadU16Pairs(const uint8_t *ids, uint8_t count);

/**
  * @brief Start the default telemetry stream used by the GUI.
  */
bool UF4PowerClient_StartStreamAll(void);

/**
  * @brief Start a telemetry stream for caller-provided register IDs.
  */
bool UF4PowerClient_StartStreamU16Pairs(const uint8_t *ids, uint8_t count);

/**
  * @brief Request remote telemetry streaming to stop.
  */
bool UF4PowerClient_StopStream(void);

/**
  * @brief Read a cached U16 value; returns false until that ID has been received.
  */
bool UF4PowerClient_GetU16(uint8_t id, uint16_t *value);

/**
  * @brief Consume and clear the data-changed flag.
  */
bool UF4PowerClient_ConsumeDataChanged(void);

/**
  * @brief Return whether stream data or start response has been seen.
  */
bool UF4PowerClient_IsStreamEnabled(void);

uint32_t UF4PowerClient_TxOkCount(void);
uint32_t UF4PowerClient_TxFailCount(void);
uint32_t UF4PowerClient_RxByteCount(void);
uint32_t UF4PowerClient_RxFrameCount(void);
uint32_t UF4PowerClient_RxErrorCount(void);
uint32_t UF4PowerClient_RxOverflowCount(void);
uint8_t UF4PowerClient_LastTxCmd(void);
uint8_t UF4PowerClient_LastTxLen(void);
bool UF4PowerClient_LastTxOk(void);

/**
  * @brief Lightweight USART1 IRQ handler that only drains bytes into the RX ring.
  */
void UF4PowerClient_UartIrqHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* UF4_POWER_CLIENT_H */
