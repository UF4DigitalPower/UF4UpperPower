#include "gui_internal.h"

void GUI_DrawWavePage(void)
{
    GUI_Rect_t title_rect = {16U, 18U, 608U, 40U};
    GUI_Rect_t frame_rect = {16U, 72U, 608U, 320U};
    GUI_Rect_t hint_rect = {16U, 204U, 608U, 28U};
    GUI_LabelTile_t back_tile = {{240U, 420U, 160U, 42U}, "BACK"};

    GUI_DrawCenteredText(&title_rect, "SCOPE", GUI_FONT_SET_VALUE, GUI_TEXT_COLOR, GUI_BG_COLOR);
    GUI_DrawPanel(&frame_rect, GUI_PANEL_DARK);
    GUI_DrawCenteredText(&hint_rect, "Waveform View Reserved", GUI_FONT_TILE_VALUE, GUI_MUTED_COLOR, GUI_PANEL_DARK);
    GUI_DrawStateTile(&back_tile, "M", GUI_PANEL_COLOR, 1U);
}
