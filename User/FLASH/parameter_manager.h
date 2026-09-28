#ifndef PARAMETER_MANAGER_H
#define PARAMETER_MANAGER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void UF4_ParameterManager_Init(void);
void UF4_ParameterManager_Process(void);
void UF4_ParameterManager_OnWriteApply(void *user);
void UF4_ParameterManager_SaveNow(void);
void UF4_ParameterManager_LoadDefaults(void);
uint8_t UF4_ParameterManager_IsReady(void);
uint8_t UF4_ParameterManager_LastSaveOk(void);

#ifdef __cplusplus
}
#endif

#endif
