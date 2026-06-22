#ifndef GUI_INTERNAL_H
#define GUI_INTERNAL_H

#include "gui.h"

#include <stdint.h>

#define GUI_BG_COLOR            0x0000
#define GUI_PANEL_COLOR         0x1082
#define GUI_PANEL_DARK          0x0841
#define GUI_BORDER_COLOR        0x39E7
#define GUI_TEXT_COLOR          0xFFFF
#define GUI_MUTED_COLOR         0xBDF7
#define GUI_ACCENT_COLOR        0x07FF
#define GUI_OK_COLOR            0x05A0
#define GUI_WARN_COLOR          0xC300

#define GUI_FONT_TOP_LABEL      18U
#define GUI_FONT_TOP_VALUE      34U
#define GUI_FONT_TILE_TITLE     18U
#define GUI_FONT_TILE_VALUE     24U
#define GUI_FONT_SET_VALUE      34U
#define GUI_FONT_LABEL          24U
#define GUI_FONT_VALUE          144U

#define GUI_TOP_VALUE_CELL_W    16U
#define GUI_TILE_VALUE_CELL_W   12U
#define GUI_SET_CELL_W          16U
#define GUI_SET_TEXT_W          (5U * GUI_SET_CELL_W)
#define GUI_MAIN_VALUE_CELL_W   60U
#define GUI_MAIN_VALUE_W        300U
#define GUI_MAIN_VALUE_H        104U

#define GUI_TOP_Y               4U
#define GUI_TOP_H               54U
#define GUI_BODY_Y              66U
#define GUI_ROW_H               132U
#define GUI_ROW_GAP             6U
#define GUI_LEFT_X              4U
#define GUI_LEFT_W              360U
#define GUI_RIGHT_X             372U
#define GUI_RIGHT_COL_W         126U
#define GUI_RIGHT_GAP           6U
#define GUI_RIGHT_COL2_X        (GUI_RIGHT_X + GUI_RIGHT_COL_W + GUI_RIGHT_GAP)
#define GUI_RIGHT_COL2_W        132U
#define GUI_SIDE_TILE_H         42U
#define GUI_SIDE_TILE_GAP       3U
#define GUI_MAIN_VALUE_X_OFFSET 6U
#define GUI_MAIN_VALUE_Y_OFFSET 20U

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
} GUI_Rect_t;

typedef struct
{
    GUI_Rect_t rect;
    const char *title;
    const char *unit;
} GUI_ValueTile_t;

typedef struct
{
    GUI_Rect_t rect;
    const char *title;
} GUI_LabelTile_t;

void GUI_DrawPanel(const GUI_Rect_t *rect, uint16_t color);
void GUI_DrawPanelBorder(const GUI_Rect_t *rect, uint16_t color);
void GUI_DrawCenteredText(const GUI_Rect_t *rect, const char *text, uint16_t font, uint16_t color, uint16_t bg);
void GUI_DrawFixedSlotText(const GUI_Rect_t *rect, const char *text, uint16_t font, uint16_t cell_w, uint16_t color, uint16_t bg);
void GUI_FormatMainValue(char *buf, uint32_t size, float value);
void GUI_FormatFixed2(char *buf, uint32_t size, float value);
void GUI_DrawTopTile(const GUI_ValueTile_t *tile, float value);
void GUI_DrawMainTile(const GUI_ValueTile_t *tile, float value);
void GUI_DrawSetTile(const GUI_ValueTile_t *tile, float value, uint8_t digit);
void GUI_DrawProtectTile(const GUI_LabelTile_t *tile, float value, const char *unit, uint8_t enabled, uint8_t selected);
void GUI_DrawStateTile(const GUI_LabelTile_t *tile, const char *value, uint16_t fill_color, uint8_t selected);
void GUI_DrawTempTile(const GUI_ValueTile_t *tile, float value);

const char *GUI_ModeText(uint8_t mode);
const char *GUI_StateText(uint8_t state);
const char *GUI_FaultText(uint8_t fault);
const char *GUI_TopoText(const GUI_Data_t *data);

void GUI_DrawMainPage(const GUI_Data_t *data, const GUI_Data_t *last_data);
void GUI_DrawWavePage(void);

#endif /* GUI_INTERNAL_H */
