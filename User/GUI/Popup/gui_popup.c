/**   ******************************************************************************   * @file    gui_popup.c   * @brief   UF4 GUI module.   ******************************************************************************   */
#include "gui_internal.h"

#define UI_POPUP_X              12U
#define UI_POPUP_Y              12U
#define UI_POPUP_W              236U
#define UI_POPUP_H               84U
#define UI_POPUP_DEFAULT_MS     3000U

typedef struct
{
  uint8_t active;
  uint8_t rendered;
  uint8_t background_saved;
  char title[28];
  char message[48];
  uint32_t start_tick;
  uint32_t hold_ms;
} ui_popup_t;

static ui_popup_t g_popup;

static void ui_popup_draw_box(uint16_t x, uint16_t y)
{
  draw_cell(x, y, UI_POPUP_W, UI_POPUP_H, UI_COLOR_PANEL);
  fill(x, y, 6U, UI_POPUP_H, UI_COLOR_ACCENT);
  draw_text((uint16_t)(x + 16U), (uint16_t)(y + 10U), g_popup.title, 2U, UI_COLOR_INK);
  draw_text((uint16_t)(x + 16U), (uint16_t)(y + 42U), g_popup.message, 1U, UI_COLOR_MUTED);
}

void GUI_PopupShow(const char *title, const char *message, uint32_t duration_ms)
{
  uint8_t was_active = g_popup.active;
  if (title == 0)
  {
    title = "";
  }
  if (message == 0)
  {
    message = "";
  }
  if (duration_ms == 0U)
  {
    duration_ms = UI_POPUP_DEFAULT_MS;
  }

  (void)snprintf(g_popup.title, sizeof(g_popup.title), "%s", title);
  (void)snprintf(g_popup.message, sizeof(g_popup.message), "%s", message);
  g_popup.start_tick = HAL_GetTick();
  g_popup.hold_ms = duration_ms;
  g_popup.rendered = 0U;
  if (was_active == 0U)
  {
    g_popup.background_saved = 0U;
  }
  g_popup.active = 1U;
}

void GUI_PopupHide(void)
{
  if (g_popup.active == 0U)
  {
    return;
  }

  g_popup.active = 0U;
  g_popup.rendered = 0U;
  if (g_popup.background_saved != 0U)
  {
    LCD_BlitRGB565FromAddr(UI_POPUP_X, UI_POPUP_Y, UI_POPUP_W, UI_POPUP_H,
                           LCD_POPUP_BACKUP_ADDR, UI_POPUP_W);
    LCD_CommitRectFromDraw(UI_POPUP_X, UI_POPUP_Y, UI_POPUP_W, UI_POPUP_H);
    g_popup.background_saved = 0U;
  }
}

void ui_popup_tick(uint32_t now)
{
  uint32_t elapsed;

  if (g_popup.active == 0U)
  {
    return;
  }

  elapsed = (uint32_t)(now - g_popup.start_tick);

  if (elapsed >= g_popup.hold_ms)
  {
    g_popup.active = 0U;
    g_popup.rendered = 0U;
    if (g_popup.background_saved != 0U)
    {
      LCD_BlitRGB565FromAddr(UI_POPUP_X, UI_POPUP_Y, UI_POPUP_W, UI_POPUP_H,
                             LCD_POPUP_BACKUP_ADDR, UI_POPUP_W);
      LCD_CommitRectFromDraw(UI_POPUP_X, UI_POPUP_Y, UI_POPUP_W, UI_POPUP_H);
      g_popup.background_saved = 0U;
    }
    return;
  }

  if (g_popup.rendered == 0U)
  {
    if (g_popup.background_saved == 0U)
    {
      LCD_CopyRectFromDrawToAddr(UI_POPUP_X, UI_POPUP_Y, UI_POPUP_W, UI_POPUP_H,
                                 LCD_POPUP_BACKUP_ADDR, UI_POPUP_W);
      g_popup.background_saved = 1U;
    }
    ui_popup_draw_box(UI_POPUP_X, UI_POPUP_Y);
    LCD_CommitRectFromDraw(UI_POPUP_X, UI_POPUP_Y, UI_POPUP_W, UI_POPUP_H);
    g_popup.rendered = 1U;
  }
}

uint8_t ui_popup_is_active(void)
{
  return g_popup.active;
}
