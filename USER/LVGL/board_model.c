#include "board_model.h"

#include "bsp_lcd.h"

static const uint16_t s_setting_ids[SETTING_COUNT] = {8U, 30U, 9U, 11U, 29U, 31U};
static uint32_t s_dirty_params;

void Board_Model_InitDefaults(void)
{
    s_dirty_params = 0U;
    memset(&state, 0, sizeof(state));
    state.voltage_set = 5.0f;
    state.current_limit = 2.0f;
    state.presets[0] = (preset_t){5.0f, 2.0f};
    state.presets[1] = (preset_t){12.0f, 2.0f};
    state.presets[2] = (preset_t){24.0f, 3.2f};
    state.presets[3] = (preset_t){48.0f, 5.0f};
}

ui_setting_t *Board_Model_FindSetting(uint16_t id)
{
    for(uint8_t i = 0U; i < SETTING_COUNT; i++) {
        if(s_setting_ids[i] == id) return &settings[i];
    }
    return NULL;
}

void ui_home_export_state(uint8_t *step_index,
                          ui_home_preset_t *presets, uint8_t *selected)
{
    if(step_index) *step_index = state.home_step;
    if(presets) memcpy(presets, state.presets, sizeof(state.presets));
    if(selected) *selected = state.preset_selected;
}

void ui_home_import_state(uint8_t step_index,
                          const ui_home_preset_t *presets, uint8_t selected)
{
    state.home_step = step_index < 3U ? step_index : 0U;
    state.preset_selected = selected < 4U ? selected : 0U;
    if(presets) memcpy(state.presets, presets, sizeof(state.presets));
}

void ui_apply_color_preset(uint8_t preset)
{
    settings[4].current = preset < 8U ? preset : 0U;
}

uint8_t UI_Params_IsDirty(uint8_t id)
{
    for(uint8_t i = 0U; i < PARAM_COUNT; ++i)
        if(params[i].id == id) return (s_dirty_params & (1UL << i)) != 0U;
    return 0U;
}

void UI_Params_MarkDirty(uint8_t id)
{
    for(uint8_t i = 0U; i < PARAM_COUNT; ++i)
        if(params[i].id == id) { s_dirty_params |= 1UL << i; return; }
}

uint8_t UI_Params_CollectDirty(uint8_t *ids, float *values)
{
    uint8_t count = 0U;
    if(ids == NULL || values == NULL) return 0U;
    for(uint8_t i = 0U; i < PARAM_COUNT; ++i) {
        if((s_dirty_params & (1UL << i)) != 0U) {
            ids[count] = params[i].id;
            values[count++] = params[i].value;
        }
    }
    return count;
}

void UI_Params_NotifyRemoteUpdate(uint8_t id)
{
    (void)id;
}

void UI_Params_ClearDirty(void)
{
    s_dirty_params = 0U;
}
