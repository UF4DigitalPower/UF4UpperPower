#ifndef BOARD_MODEL_H
#define BOARD_MODEL_H

#include "custom_internal.h"

#define UI_PARAM_COUNT PARAM_COUNT
#define UI_SETTING_COUNT SETTING_COUNT
#define UI_HOME_PRESET_COUNT 4U
#define UI_SETTING_ID_ESP32_START_MODE 31U
#define UI_ESP32_START_NONE 0U

typedef setting_t ui_setting_t;
typedef preset_t ui_home_preset_t;

void Board_Model_InitDefaults(void);
ui_setting_t *Board_Model_FindSetting(uint16_t id);
void ui_home_export_state(uint8_t *step_index,
                          ui_home_preset_t *presets, uint8_t *selected);
void ui_home_import_state(uint8_t step_index,
                          const ui_home_preset_t *presets, uint8_t selected);
void ui_apply_color_preset(uint8_t preset);
uint8_t UI_Params_IsDirty(uint8_t id);
void UI_Params_MarkDirty(uint8_t id);
uint8_t UI_Params_CollectDirty(uint8_t *ids, float *values);
void UI_Params_NotifyRemoteUpdate(uint8_t id);
void UI_Params_ClearDirty(void);

#endif
