#include "gui_internal.h"

#include <stdio.h>
#include <string.h>

static const GUI_ValueTile_t g_top_tiles[] =
{
    {{  4U, GUI_TOP_Y, 124U, GUI_TOP_H}, "VIN", "V"},
    {{132U, GUI_TOP_Y, 124U, GUI_TOP_H}, "IIN", "A"},
    {{260U, GUI_TOP_Y, 124U, GUI_TOP_H}, "PIN", "W"},
    {{388U, GUI_TOP_Y, 124U, GUI_TOP_H}, "EFF", "%"},
    {{512U, GUI_TOP_Y, 124U, GUI_TOP_H}, "FAN", "%"},
};

static const GUI_ValueTile_t g_main_tiles[] =
{
    {{GUI_LEFT_X, GUI_BODY_Y, GUI_LEFT_W, GUI_ROW_H}, "VO", "V"},
    {{GUI_LEFT_X, GUI_BODY_Y + GUI_ROW_H + GUI_ROW_GAP, GUI_LEFT_W, GUI_ROW_H}, "IO", "A"},
    {{GUI_LEFT_X, GUI_BODY_Y + 2U * (GUI_ROW_H + GUI_ROW_GAP), GUI_LEFT_W, GUI_ROW_H}, "PO", "W"},
};

static const GUI_ValueTile_t g_set_tiles[] =
{
    {{GUI_RIGHT_X, GUI_BODY_Y, GUI_RIGHT_COL_W, 63U}, "VSET", "V"},
    {{GUI_RIGHT_X, GUI_BODY_Y + 69U, GUI_RIGHT_COL_W, 63U}, "ISET", "A"},
};

static const GUI_LabelTile_t g_info_tiles[] =
{
    {{GUI_RIGHT_COL2_X, GUI_BODY_Y, GUI_RIGHT_COL2_W, GUI_SIDE_TILE_H}, "OTP"},
    {{GUI_RIGHT_COL2_X, GUI_BODY_Y + GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP, GUI_RIGHT_COL2_W, GUI_SIDE_TILE_H}, "OVP"},
    {{GUI_RIGHT_COL2_X, GUI_BODY_Y + 2U * (GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP), GUI_RIGHT_COL2_W, GUI_SIDE_TILE_H}, "OCP"},
    {{GUI_RIGHT_X, GUI_BODY_Y + GUI_ROW_H + GUI_ROW_GAP, GUI_RIGHT_COL_W, GUI_SIDE_TILE_H}, "OUT"},
    {{GUI_RIGHT_X, GUI_BODY_Y + GUI_ROW_H + GUI_ROW_GAP + GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP, GUI_RIGHT_COL_W, GUI_SIDE_TILE_H}, "TOPO"},
    {{GUI_RIGHT_X, GUI_BODY_Y + GUI_ROW_H + GUI_ROW_GAP + 2U * (GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP), GUI_RIGHT_COL_W, GUI_SIDE_TILE_H}, "MODE"},
    {{GUI_RIGHT_COL2_X, GUI_BODY_Y + GUI_ROW_H + GUI_ROW_GAP, GUI_RIGHT_COL2_W, GUI_SIDE_TILE_H}, "CMD"},
    {{GUI_RIGHT_COL2_X, GUI_BODY_Y + GUI_ROW_H + GUI_ROW_GAP + GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP, GUI_RIGHT_COL2_W, GUI_SIDE_TILE_H}, "STATE"},
    {{GUI_RIGHT_COL2_X, GUI_BODY_Y + GUI_ROW_H + GUI_ROW_GAP + 2U * (GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP), GUI_RIGHT_COL2_W, GUI_SIDE_TILE_H}, "FAULT"},
    {{GUI_RIGHT_COL2_X, GUI_BODY_Y + 2U * (GUI_ROW_H + GUI_ROW_GAP), GUI_RIGHT_COL2_W, GUI_SIDE_TILE_H}, "BLE"},
    {{GUI_RIGHT_COL2_X, GUI_BODY_Y + 2U * (GUI_ROW_H + GUI_ROW_GAP) + GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP, GUI_RIGHT_COL2_W, GUI_SIDE_TILE_H}, "SCOPE"},
};

static const GUI_ValueTile_t g_temp_tiles[] =
{
    {{GUI_RIGHT_X, GUI_BODY_Y + 2U * (GUI_ROW_H + GUI_ROW_GAP), GUI_RIGHT_COL_W, GUI_SIDE_TILE_H}, "CPU", "C"},
    {{GUI_RIGHT_X, GUI_BODY_Y + 2U * (GUI_ROW_H + GUI_ROW_GAP) + GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP, GUI_RIGHT_COL_W, GUI_SIDE_TILE_H}, "BUCK", "C"},
    {{GUI_RIGHT_X, GUI_BODY_Y + 2U * (GUI_ROW_H + GUI_ROW_GAP) + 2U * (GUI_SIDE_TILE_H + GUI_SIDE_TILE_GAP), GUI_RIGHT_COL_W, GUI_SIDE_TILE_H}, "BOOST", "C"},
};

static void GUI_DrawTopTiles(const GUI_Data_t *data, const GUI_Data_t *last_data);
static void GUI_DrawMainTiles(const GUI_Data_t *data, const GUI_Data_t *last_data);
static uint8_t GUI_MainValueTextChanged(float value, float last_value);

void GUI_DrawMainPage(const GUI_Data_t *data, const GUI_Data_t *last_data)
{
    char cmd_buf[8];
    uint8_t out_selected;
    uint8_t ble_selected;
    uint8_t scope_selected;
    uint8_t tx_flash = 0U;

    if (data == NULL)
    {
        return;
    }

    GUI_DrawTopTiles(data, last_data);

    GUI_DrawMainTiles(data, last_data);

    GUI_DrawSetTile(&g_set_tiles[0], data->vset, data->vset_digit);
    GUI_DrawSetTile(&g_set_tiles[1], data->iset, data->iset_digit);

    GUI_DrawProtectTile(&g_info_tiles[0], data->otp, "C", data->otp_enabled, (uint8_t)(data->panel_field == 0U));
    GUI_DrawProtectTile(&g_info_tiles[1], data->ovp, "V", data->ovp_enabled, (uint8_t)(data->panel_field == 1U));
    GUI_DrawProtectTile(&g_info_tiles[2], data->ocp, "A", data->ocp_enabled, (uint8_t)(data->panel_field == 2U));

    out_selected = (uint8_t)(data->panel_field == 3U);
    ble_selected = (uint8_t)(data->panel_field == 4U);
    scope_selected = (uint8_t)(data->panel_field == 5U);

    GUI_DrawStateTile(&g_info_tiles[3], data->output_enabled != 0U ? "ON" : "OFF", data->output_enabled != 0U ? GUI_OK_COLOR : GUI_PANEL_COLOR, out_selected);
    GUI_DrawStateTile(&g_info_tiles[4], GUI_TopoText(data), GUI_PANEL_COLOR, 0U);
    GUI_DrawStateTile(&g_info_tiles[5], (data->valid_flags & GUI_VALID_CC_CV_MODE) != 0U ? GUI_ModeText(data->regulation_mode) : "NA", GUI_PANEL_COLOR, 0U);

    if (last_data != NULL)
    {
        tx_flash = (uint8_t)(data->comm_last_tx_cmd != 0U &&
            (data->comm_tx_ok_count != last_data->comm_tx_ok_count ||
             data->comm_tx_fail_count != last_data->comm_tx_fail_count));
    }
    snprintf(cmd_buf, sizeof(cmd_buf), "%s", tx_flash != 0U ? "TX" : "--");
    GUI_DrawStateTile(&g_info_tiles[6], cmd_buf, tx_flash != 0U ? GUI_WARN_COLOR : GUI_PANEL_COLOR, 0U);

    GUI_DrawStateTile(&g_info_tiles[7], (data->valid_flags & GUI_VALID_STATE_MACHINE_FLAGS) != 0U ? GUI_StateText((uint8_t)data->state_machine_flags) : "NA", GUI_PANEL_COLOR, 0U);
    GUI_DrawStateTile(&g_info_tiles[8], (data->valid_flags & GUI_VALID_FAULT_STATE) != 0U ? GUI_FaultText(data->fault_state) : "NA", GUI_PANEL_COLOR, 0U);
    GUI_DrawStateTile(&g_info_tiles[9], data->ble_state != 0U ? "ON" : "OFF", GUI_PANEL_COLOR, ble_selected);
    GUI_DrawStateTile(&g_info_tiles[10], "OPEN", GUI_PANEL_COLOR, scope_selected);

    GUI_DrawTempTile(&g_temp_tiles[0], data->cpu_temp);
    GUI_DrawTempTile(&g_temp_tiles[1], data->buck_temp);
    GUI_DrawTempTile(&g_temp_tiles[2], data->boost_temp);
}

/**
  * @brief Draw top status tiles without repainting static labels on every refresh.
  */
static void GUI_DrawTopTiles(const GUI_Data_t *data, const GUI_Data_t *last_data)
{
    if (last_data == NULL)
    {
        GUI_DrawTopTile(&g_top_tiles[0], data->vin);
        GUI_DrawTopTile(&g_top_tiles[1], data->iin);
        GUI_DrawTopTile(&g_top_tiles[2], data->pin);
        GUI_DrawTopTile(&g_top_tiles[3], data->efficiency);
        GUI_DrawTopTile(&g_top_tiles[4], data->fan);
        return;
    }

    if (GUI_MainValueTextChanged(data->vin, last_data->vin) != 0U)
    {
        GUI_DrawTopTileValue(&g_top_tiles[0], data->vin);
    }
    if (GUI_MainValueTextChanged(data->iin, last_data->iin) != 0U)
    {
        GUI_DrawTopTileValue(&g_top_tiles[1], data->iin);
    }
    if (GUI_MainValueTextChanged(data->pin, last_data->pin) != 0U)
    {
        GUI_DrawTopTileValue(&g_top_tiles[2], data->pin);
    }
    if (GUI_MainValueTextChanged(data->efficiency, last_data->efficiency) != 0U)
    {
        GUI_DrawTopTileValue(&g_top_tiles[3], data->efficiency);
    }
    if (GUI_MainValueTextChanged(data->fan, last_data->fan) != 0U)
    {
        GUI_DrawTopTileValue(&g_top_tiles[4], data->fan);
    }
}

/**
  * @brief Draw VO/IO/PO without repainting the whole black panel each tick.
  */
static void GUI_DrawMainTiles(const GUI_Data_t *data, const GUI_Data_t *last_data)
{
    if (last_data == NULL)
    {
        GUI_DrawMainTile(&g_main_tiles[0], data->vout);
        GUI_DrawMainTile(&g_main_tiles[1], data->iout);
        GUI_DrawMainTile(&g_main_tiles[2], data->pout);
        return;
    }

    if (GUI_MainValueTextChanged(data->vout, last_data->vout) != 0U)
    {
        GUI_DrawMainTileValue(&g_main_tiles[0], data->vout);
    }
    if (GUI_MainValueTextChanged(data->iout, last_data->iout) != 0U)
    {
        GUI_DrawMainTileValue(&g_main_tiles[1], data->iout);
    }
    if (GUI_MainValueTextChanged(data->pout, last_data->pout) != 0U)
    {
        GUI_DrawMainTileValue(&g_main_tiles[2], data->pout);
    }
}

/**
  * @brief Compare the text that will be displayed, not raw float jitter.
  */
static uint8_t GUI_MainValueTextChanged(float value, float last_value)
{
    char value_text[16];
    char last_text[16];

    GUI_FormatMainValue(value_text, sizeof(value_text), value);
    GUI_FormatMainValue(last_text, sizeof(last_text), last_value);

    return (uint8_t)(strcmp(value_text, last_text) != 0);
}
