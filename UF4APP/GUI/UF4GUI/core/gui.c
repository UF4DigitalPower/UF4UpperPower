/**
  ******************************************************************************
  * @file    gui.c
  * @author  UF4
  * @date    26-6-20 下午6:22
  * @brief   UF4GUI core initialization, theme, event and dirty refresh services.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 UF4.
  * All rights reserved.
  *
  * This software is provided "as is", without warranty of any kind.
  *
  ******************************************************************************
  */
#include "gui.h"
#include "bsp_lcd.h"

static GUI_Rect g_dirty[GUI_MAX_DIRTY_RECTS];
static uint8_t g_dirty_count;
static const GUI_Theme *g_theme;

static const GUI_Theme g_themes[] = {
    {0x1082, 0x2104, 0xEF7D, 0x8C71, 0x39E7, 0xFD20, 0x07FF, 0xF800, 0xFFE0, 0x07E0},
    {0xFFFF, 0xF7BE, 0x1082, 0x7BEF, 0xC618, 0xE3A0, 0x001F, 0xD000, 0xE5A0, 0x0560},
    {0x0841, 0x18E3, 0xE71C, 0x8410, 0x4208, 0xFDE0, 0x07A4, 0xF980, 0xFE60, 0x0660}
};

static GUI_Rect gui_rect_clip(GUI_Rect r)
{
    if (r.x < 0) {
        r.w = (int16_t)(r.w + r.x);
        r.x = 0;
    }
    if (r.y < 0) {
        r.h = (int16_t)(r.h + r.y);
        r.y = 0;
    }
    if ((int32_t)r.x + r.w > (int32_t)GUI_SCREEN_WIDTH) {
        r.w = (int16_t)(GUI_SCREEN_WIDTH - (uint16_t)r.x);
    }
    if ((int32_t)r.y + r.h > (int32_t)GUI_SCREEN_HEIGHT) {
        r.h = (int16_t)(GUI_SCREEN_HEIGHT - (uint16_t)r.y);
    }
    if (r.w < 0) {
        r.w = 0;
    }
    if (r.h < 0) {
        r.h = 0;
    }
    return r;
}

static uint8_t gui_rect_intersects(const GUI_Rect *a, const GUI_Rect *b)
{
    return (a->x < b->x + b->w) && (b->x < a->x + a->w) &&
           (a->y < b->y + b->h) && (b->y < a->y + a->h);
}

static GUI_Rect gui_rect_union(const GUI_Rect *a, const GUI_Rect *b)
{
    int16_t x1 = (a->x < b->x) ? a->x : b->x;
    int16_t y1 = (a->y < b->y) ? a->y : b->y;
    int16_t x2 = ((a->x + a->w) > (b->x + b->w)) ? (int16_t)(a->x + a->w) : (int16_t)(b->x + b->w);
    int16_t y2 = ((a->y + a->h) > (b->y + b->h)) ? (int16_t)(a->y + a->h) : (int16_t)(b->y + b->h);
    GUI_Rect r = {x1, y1, (int16_t)(x2 - x1), (int16_t)(y2 - y1)};
    return r;
}

/**
  * @brief  Initializes UF4GUI core state, default theme and full-screen invalidation.
  * @retval None
  */
void GUI_Init(void)
{
    g_dirty_count = 0U;
    g_theme = &g_themes[GUI_THEME_INDUSTRIAL];
    LCD_SetDisplayDir(1);
    LCD_Clear(g_theme->bg);
    GUI_InvalidateRect(&(GUI_Rect){0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT});
}

/**
  * @brief  Updates animation and current page timing.
  * @param  elapsed_ms Elapsed time in milliseconds since last GUI tick.
  * @retval None
  */
void GUI_Tick(uint32_t elapsed_ms)
{
    GUI_Anim_Update(elapsed_ms);
    GUI_Page_Update(elapsed_ms);
}

/**
  * @brief  Dispatches an input event to the current page widget tree.
  * @param  event Pointer to the GUI event descriptor.
  * @retval None
  */
void GUI_DispatchEvent(const GUI_Event *event)
{
    GUI_Page *page = GUI_Page_Current();

    if ((page != 0) && (page->root != 0)) {
        GUI_Widget_DispatchTree(page->root, event);
    }
}

/**
  * @brief  Adds a rectangle to the dirty refresh list.
  * @param  rect Pointer to the rectangle that needs redraw.
  * @retval None
  */
void GUI_InvalidateRect(const GUI_Rect *rect)
{
    GUI_Rect r = gui_rect_clip(*rect);
    uint8_t i;

    if ((r.w <= 0) || (r.h <= 0)) {
        return;
    }

    for (i = 0U; i < g_dirty_count; ++i) {
        if (gui_rect_intersects(&g_dirty[i], &r)) {
            g_dirty[i] = gui_rect_union(&g_dirty[i], &r);
            return;
        }
    }

    if (g_dirty_count < GUI_MAX_DIRTY_RECTS) {
        g_dirty[g_dirty_count++] = r;
        return;
    }

    g_dirty[0] = gui_rect_union(&g_dirty[0], &r);
}

/**
  * @brief  Marks a widget rectangle as dirty.
  * @param  widget Pointer to target widget.
  * @retval None
  */
void GUI_InvalidateWidget(GUI_Widget *widget)
{
    if (widget == 0) {
        return;
    }
    widget->obj.invalid = 1U;
    GUI_InvalidateRect(&widget->obj.rect);
}

/**
  * @brief  Redraws dirty rectangles and presents the draw buffer.
  * @retval None
  */
void GUI_Refresh(void)
{
    uint8_t i;

    if (g_dirty_count == 0U) {
        return;
    }

    for (i = 0U; i < g_dirty_count; ++i) {
        LCD_CopyRectFromFrontToDraw((uint16_t)g_dirty[i].x, (uint16_t)g_dirty[i].y,
                                    (uint16_t)g_dirty[i].w, (uint16_t)g_dirty[i].h);
        GUI_DrawSetClip(&g_dirty[i]);
        GUI_Page_Draw(&g_dirty[i]);
    }

    GUI_DrawResetClip();
    LCD_Present();
    g_dirty_count = 0U;
}

/**
  * @brief  Selects the active GUI theme.
  * @param  id Theme identifier.
  * @retval None
  */
void GUI_SetTheme(GUI_ThemeId id)
{
    if ((uint8_t)id >= (uint8_t)(sizeof(g_themes) / sizeof(g_themes[0]))) {
        id = GUI_THEME_INDUSTRIAL;
    }
    g_theme = &g_themes[id];
    GUI_InvalidateRect(&(GUI_Rect){0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT});
}

/**
  * @brief  Gets the active GUI theme.
  * @retval Pointer to the active theme.
  */
const GUI_Theme *GUI_GetTheme(void)
{
    return g_theme;
}
