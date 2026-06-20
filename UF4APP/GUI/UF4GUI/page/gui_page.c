/**
  ******************************************************************************
  * @file    gui_page.c
  * @author  UF4
  * @date    26-6-20 下午6:22
  * @brief   UF4GUI static page registration and switching services.
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

static GUI_Page *g_pages[GUI_MAX_PAGE_COUNT];
static uint8_t g_page_count;
static GUI_Page *g_current_page;

/**
  * @brief  Registers a page in the static page table.
  * @param  page Pointer to page descriptor.
  * @retval 1 on success, 0 on failure.
  */
uint8_t GUI_Page_Register(GUI_Page *page)
{
    if ((page == 0) || (g_page_count >= GUI_MAX_PAGE_COUNT)) {
        return 0U;
    }
    g_pages[g_page_count++] = page;
    return 1U;
}

/**
  * @brief  Switches to a registered page by id.
  * @param  id Target page identifier.
  * @retval 1 on success, 0 if page is not found.
  */
uint8_t GUI_Page_Switch(uint8_t id)
{
    uint8_t i;

    for (i = 0U; i < g_page_count; ++i) {
        if ((g_pages[i] != 0) && (g_pages[i]->id == id)) {
            if ((g_current_page != 0) && (g_current_page->on_leave != 0)) {
                g_current_page->on_leave();
            }
            g_current_page = g_pages[i];
            if (g_current_page->on_enter != 0) {
                g_current_page->on_enter();
            }
            GUI_InvalidateRect(&(GUI_Rect){0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT});
            return 1U;
        }
    }
    return 0U;
}

/**
  * @brief  Updates the active page.
  * @param  elapsed_ms Elapsed time in milliseconds.
  * @retval None
  */
void GUI_Page_Update(uint32_t elapsed_ms)
{
    if ((g_current_page != 0) && (g_current_page->on_update != 0)) {
        g_current_page->on_update(elapsed_ms);
    }
}

/**
  * @brief  Draws the active page inside the current clip rectangle.
  * @param  clip Pointer to current clip rectangle.
  * @retval None
  */
void GUI_Page_Draw(const GUI_Rect *clip)
{
    if (g_current_page == 0) {
        return;
    }
    if (g_current_page->on_draw != 0) {
        g_current_page->on_draw(clip);
    }
    if (g_current_page->root != 0) {
        GUI_Widget_DrawTree(g_current_page->root, clip);
    }
}

/**
  * @brief  Gets the active page descriptor.
  * @retval Pointer to active page, or NULL if no page is active.
  */
GUI_Page *GUI_Page_Current(void)
{
    return g_current_page;
}
