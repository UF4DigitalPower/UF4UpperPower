/*
 * Copyright 2026 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "custom_internal.h"
#include "g474_remote.h"
#include "parameter_manager.h"
#include "esp32c6_bus.h"
#include "bsp_lcd.h"
#include "uf4com.h"
#include "board_model.h"

void clicked(lv_event_t *event)
{
    if(ui_transition_active()) return;
    uintptr_t data = (uintptr_t)lv_event_get_user_data(event);
    action_t action = (action_t)(data >> 8);
    uint8_t arg = (uint8_t)data;
    char message[48];
    switch(action) {
    case ACT_OUTPUT:
        if(settings[5].current != 0U) { log_event("OUTPUT LOCKED"); break; }
        if(G474_Remote_Write(UF4_ID_OUTPUT_ENABLE, !state.output_on) != 0U) {
            state.output_on = !state.output_on;
            log_event(state.output_on ? "OUTPUT ON" : "OUTPUT OFF");
        } else log_event("OUTPUT WRITE FAILED");
        break;
    case ACT_NAV:
        if(state.page != (page_t)arg) {
            snprintf(message, sizeof(message), "PAGE %s",
                     arg == PAGE_HOME ? "HOME" : arg == PAGE_PARAMS ? "PARAMS" :
                     arg == PAGE_SETTINGS ? "SETTINGS" : "LOG");
            log_event(message);
            ui_transition_to((page_t)arg);
            return;
        }
        break;
    case ACT_HOME_ADJUST: {
        float steps[] = {0.02f, 0.2f, 2.0f};
        float *value = arg < 2 ? &state.voltage_set : &state.current_limit;
        float max = arg < 2 ? 75.0f : 10.0f;
        if(settings[5].current != 0U) { log_event("SETPOINT LOCKED"); break; }
        float previous = *value;
        *value += (arg % 2 == 0 ? -1.0f : 1.0f) * steps[state.home_step];
        if(*value < 0.0f) *value = 0.0f;
        if(*value > max) *value = max;
        if(G474_Remote_WriteFloat(arg < 2 ? UF4_ID_SET_VOLTAGE_LIMIT :
                                  UF4_ID_SET_CURRENT_LIMIT, *value) == 0U) {
            *value = previous;
            log_event("SETPOINT WRITE FAILED");
        }
        break;
    }
    case ACT_HOME_STEP: state.home_step = arg; break;
    case ACT_PRESET_SELECT: state.preset_selected = (state.preset_selected + 1) % 4; break;
    case ACT_PRESET_SAVE:
        state.presets[state.preset_selected] = (preset_t){state.voltage_set, state.current_limit};
        snprintf(message, sizeof(message), "PRESET P%u SAVED", (unsigned)(state.preset_selected + 1));
        UF4_ParameterManager_SaveNow();
        log_event(message);
        break;
    case ACT_PRESET_APPLY:
        if(settings[5].current != 0U) { log_event("PRESET LOCKED"); break; }
        state.voltage_set = state.presets[state.preset_selected].voltage;
        state.current_limit = state.presets[state.preset_selected].current;
        if(G474_Remote_WriteFloat(UF4_ID_SET_VOLTAGE_LIMIT, state.voltage_set) == 0U ||
           G474_Remote_WriteFloat(UF4_ID_SET_CURRENT_LIMIT, state.current_limit) == 0U) {
            log_event("PRESET WRITE FAILED");
            break;
        }
        snprintf(message, sizeof(message), "PRESET P%u APPLIED", (unsigned)(state.preset_selected + 1));
        log_event(message);
        break;
    case ACT_PARAM_PAGE:
        if(arg == 0 && state.param_page > 0) state.param_page--;
        if(arg == 1 && state.param_page < (PARAM_COUNT - 1) / PARAMS_PER_PAGE) state.param_page++;
        state.param_selected = 0;
        break;
    case ACT_PARAM_SELECT: state.param_selected = arg; break;
    case ACT_PARAM_ADJUST: {
        param_t *param = &params[state.param_page * PARAMS_PER_PAGE + state.param_selected];
        float step = param_step(param, state.param_step != 0);
        if(settings[5].current != 0U) { log_event("PARAM LOCKED"); break; }
        if(G474_Remote_IsSaveInProgress()) { log_event("PARAM SAVE BUSY"); break; }
        param->value += arg == 0 ? -step : step;
        if(param->value < param->min) param->value = param->min;
        if(param->value > param->max) param->value = param->max;
        UI_Params_MarkDirty(param->id);
        break;
    }
    case ACT_PARAM_STEP: state.param_step = arg; break;
    case ACT_PARAM_SAVE: {
        uint8_t ids[PARAM_COUNT];
        float values[PARAM_COUNT];
        uint8_t count = UI_Params_CollectDirty(ids, values);
        if(settings[5].current != 0U) log_event("PARAM LOCKED");
        else if(count == 0U) log_event("PARAM UNCHANGED");
        else log_event(G474_Remote_SaveParameters(ids, values, count) != 0U ?
                       "PARAM SAVE REQUEST" : "PARAM SAVE BUSY");
        break;
    }
    case ACT_SETTING_SELECT: state.setting_selected = arg; break;
    case ACT_SETTING_NEXT: {
        setting_t *setting = &settings[state.setting_selected];
        if(state.setting_selected == 1) log_event("POPUP TEST");
        else {
            setting->current = (setting->current + 1) % setting->option_count;
            if(state.setting_selected == 2U) LCD_SetBacklightLevel(setting->current);
            if(state.setting_selected == 5U &&
               ESP32C6_BusSetServiceMode(setting->current) != HAL_OK) {
                setting->current = (setting->current + setting->option_count - 1U) % setting->option_count;
                log_event("ESP32 MODE FAILED");
                break;
            }
            UF4_ParameterManager_OnWriteApply(NULL);
            snprintf(message, sizeof(message), "%s = %s", setting->name,
                     setting->options[setting->current]);
            log_event(message);
        }
        break;
    }
    }
    ui_request_render();
}

void custom_init(gg_ui_t *ui)
{
    if(!ui || !ui->screen.screen) return;
    log_count = 0;
    log_event("SYSTEM READY");
    ui_render_init(ui->screen.screen);
    ui_boot_start(ui->screen.screen);
}
