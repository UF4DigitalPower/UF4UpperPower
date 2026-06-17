#ifndef UF4_POWER_CLIENT_H
#define UF4_POWER_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

void UF4PowerClient_Init(void);
void UF4PowerClient_Tick(void);
bool UF4PowerClient_WriteU16Pairs(const uint8_t *ids, const uint16_t *values, uint8_t count);
bool UF4PowerClient_WriteU16(uint8_t id, uint16_t value);
bool UF4PowerClient_ReadU16Pairs(const uint8_t *ids, uint8_t count);
bool UF4PowerClient_StartStreamAll(void);
bool UF4PowerClient_StartStreamU16Pairs(const uint8_t *ids, uint8_t count);
bool UF4PowerClient_StopStream(void);
bool UF4PowerClient_GetU16(uint8_t id, uint16_t *value);
bool UF4PowerClient_ConsumeDataChanged(void);
bool UF4PowerClient_IsStreamEnabled(void);
uint32_t UF4PowerClient_TxOkCount(void);
uint32_t UF4PowerClient_TxFailCount(void);
uint32_t UF4PowerClient_RxByteCount(void);
uint32_t UF4PowerClient_RxFrameCount(void);
uint32_t UF4PowerClient_RxErrorCount(void);

#ifdef __cplusplus
}
#endif

#endif /* UF4_POWER_CLIENT_H */
