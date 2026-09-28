#ifndef G474_REMOTE_H
#define G474_REMOTE_H

#include <stdint.h>
#include "uf4com.h"

void G474_Remote_Init(void);
void G474_Remote_Process(void);
uint8_t G474_Remote_RequestConnection(void);
uint8_t G474_Remote_Read(uint8_t id, uint16_t *value);
uint8_t G474_Remote_ReadFloat(uint8_t id, float *value);
uint8_t G474_Remote_Write(uint8_t id, uint16_t value);
uint8_t G474_Remote_WriteFloat(uint8_t id, float value);
uint8_t G474_Remote_GetOutputCommanded(void);
uint8_t G474_Remote_SaveParameter(uint8_t id, float value);
uint8_t G474_Remote_SaveParameters(const uint8_t *ids, const float *values, uint8_t count);
uint8_t G474_Remote_IsSaveInProgress(void);
uint8_t G474_Remote_ConsumeSaveResult(void);
uint8_t G474_Remote_IsOnline(void);
uint8_t G474_Remote_IsStreamRequested(void);
uint8_t G474_Remote_IsStreaming(void);

#endif
