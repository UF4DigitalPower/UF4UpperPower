#include "board_lvgl.h"

#include "bsp_lcd.h"
#include "ft5x16.h"
#include "gui_guider.h"
#include "gg_utils.h"
#include "custom_internal.h"
#include "lvgl.h"
#include "main.h"
#include "g474_remote.h"
#include "board_model.h"

#define LVGL_DRAW_LINES 32U

static uint16_t s_draw_buffer[LCD_WIDTH * LVGL_DRAW_LINES];
static lcd_rect_t s_dirty_rects[LCD_MAX_DIRTY_RECTS];
static uint8_t s_dirty_count;
static lv_indev_t *s_touch;
static volatile uint8_t s_touch_irq;
static uint8_t s_pressed;
static lv_point_t s_last_point;
static uint32_t s_last_ui_refresh_ms;

gg_ui_t guider_ui;

/* Join adjacent flush tiles before synchronizing the back framebuffer. */
static void board_add_dirty_area(const lv_area_t *area)
{
    lcd_rect_t dirty = {
        (uint16_t)area->x1, (uint16_t)area->y1,
        (uint16_t)(area->x2 - area->x1 + 1),
        (uint16_t)(area->y2 - area->y1 + 1)
    };

    if(s_dirty_count == 1U && s_dirty_rects[0].x == 0U &&
       s_dirty_rects[0].y == 0U && s_dirty_rects[0].w == LCD_WIDTH &&
       s_dirty_rects[0].h == LCD_HEIGHT) return;

    for(uint8_t i = 0U; i < s_dirty_count;) {
        lcd_rect_t *rect = &s_dirty_rects[i];
        uint32_t dirty_right = (uint32_t)dirty.x + dirty.w;
        uint32_t dirty_bottom = (uint32_t)dirty.y + dirty.h;
        uint32_t rect_right = (uint32_t)rect->x + rect->w;
        uint32_t rect_bottom = (uint32_t)rect->y + rect->h;

        if((dirty.x == rect->x && dirty.w == rect->w &&
            dirty.y <= rect_bottom && rect->y <= dirty_bottom) ||
           (dirty.y == rect->y && dirty.h == rect->h &&
            dirty.x <= rect_right && rect->x <= dirty_right)) {
            uint32_t right = dirty_right > rect_right ? dirty_right : rect_right;
            uint32_t bottom = dirty_bottom > rect_bottom ? dirty_bottom : rect_bottom;
            if(rect->x < dirty.x) dirty.x = rect->x;
            if(rect->y < dirty.y) dirty.y = rect->y;
            dirty.w = (uint16_t)(right - dirty.x);
            dirty.h = (uint16_t)(bottom - dirty.y);
            s_dirty_rects[i] = s_dirty_rects[--s_dirty_count];
            i = 0U;
        }
        else i++;
    }

    if(s_dirty_count < LCD_MAX_DIRTY_RECTS) s_dirty_rects[s_dirty_count++] = dirty;
    else {
        s_dirty_rects[0] = (lcd_rect_t){0U, 0U, LCD_WIDTH, LCD_HEIGHT};
        s_dirty_count = 1U;
    }
}

static void board_flush(lv_display_t *display, const lv_area_t *area, uint8_t *pixels)
{
    uint16_t width = (uint16_t)(area->x2 - area->x1 + 1);
    uint16_t height = (uint16_t)(area->y2 - area->y1 + 1);
    LCD_BlitRGB565((uint16_t)area->x1, (uint16_t)area->y1,
                   width, height, (const uint16_t *)pixels);
    board_add_dirty_area(area);
    if(lv_display_flush_is_last(display)) {
        LCD_CommitRectsFromDraw(s_dirty_rects, s_dirty_count);
        LCD_WaitForIdle();
        s_dirty_count = 0U;
    }
    lv_display_flush_ready(display);
}

static void board_touch_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    FT5X16_Point_t point[FT5X16_MAX_POINTS];
    uint8_t count = 0U;
    (void)indev;

    if(s_touch_irq != 0U || s_pressed != 0U || FT5X16_IsTouched() != 0U) {
        s_touch_irq = 0U;
        if(FT5X16_ReadPoints(point, &count) == HAL_OK &&
           count != 0U && point[0].event != FT5X16_EVENT_UP &&
           point[0].x < LCD_WIDTH && point[0].y < LCD_HEIGHT) {
            s_last_point.x = point[0].x;
            s_last_point.y = point[0].y;
            s_pressed = 1U;
        }
        else {
            s_pressed = 0U;
        }
    }

    data->state = s_pressed != 0U ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    data->point = s_last_point;
}

void Board_LVGL_TouchIrqNotify(void)
{
    s_touch_irq = 1U;
}

void Board_LVGL_Init(void)
{
    lv_display_t *display;

    lv_init();
    lv_tick_set_cb(HAL_GetTick);
    display = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(display, s_draw_buffer, NULL,
                           sizeof(s_draw_buffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, board_flush);

    (void)FT5X16_Init();
    s_touch = lv_indev_create();
    lv_indev_set_type(s_touch, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(s_touch, board_touch_read);
    lv_indev_set_display(s_touch, display);

    setup_ui(&guider_ui);
    custom_init(&guider_ui);
    lv_timer_handler();
}

void Board_LVGL_Process(void)
{
    uint32_t now_ms = HAL_GetTick();
    uint8_t save_result = G474_Remote_ConsumeSaveResult();
    if(save_result == 1U) {
        UI_Params_ClearDirty();
        log_event("PARAM SAVE OK");
    }
    else if(save_result != 0U) log_event("PARAM SAVE FAILED");
    if((uint32_t)(now_ms - s_last_ui_refresh_ms) >= 200U) {
        s_last_ui_refresh_ms = now_ms;
        board_ui_refresh();
    }
    lv_timer_handler();
}
