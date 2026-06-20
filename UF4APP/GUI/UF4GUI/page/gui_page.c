#include "gui.h"

static GUI_Page *g_pages[GUI_MAX_PAGE_COUNT];
static uint8_t g_page_count;
static GUI_Page *g_current_page;

uint8_t GUI_Page_Register(GUI_Page *page)
{
    if ((page == 0) || (g_page_count >= GUI_MAX_PAGE_COUNT)) {
        return 0U;
    }
    g_pages[g_page_count++] = page;
    return 1U;
}

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

void GUI_Page_Update(uint32_t elapsed_ms)
{
    if ((g_current_page != 0) && (g_current_page->on_update != 0)) {
        g_current_page->on_update(elapsed_ms);
    }
}

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

GUI_Page *GUI_Page_Current(void)
{
    return g_current_page;
}
