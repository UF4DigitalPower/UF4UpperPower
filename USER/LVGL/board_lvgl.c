#include "board_lvgl.h"

#include "bsp_lcd.h"
#include "ft5x16.h"
#include "gui_guider.h"
#include "gg_utils.h"
#include "custom_internal.h"
#include "lvgl.h"
#include "lvgl/src/draw/lv_draw_buf_private.h"
#include "main.h"
#include "g474_remote.h"
#include "board_model.h"

static lv_draw_buf_copy_cb_t s_draw_buf_copy;
static lv_indev_t *s_touch;
static volatile uint8_t s_touch_irq;
static uint8_t s_pressed;
static lv_point_t s_last_point;
static uint32_t s_last_ui_refresh_ms;
static board_lvgl_profile_t s_profile;
static uint32_t s_refresh_start_ms;
static uint32_t s_draw_start_ms;
static uint8_t s_drawing;
static uint8_t s_ref_rendered;

/* Keep input latency below one display refresh period.  The touch IRQ also
 * makes this timer ready immediately from the foreground loop. */
#define BOARD_TOUCH_READ_PERIOD_MS 5U

gg_ui_t guider_ui;

static void board_draw_buf_copy(lv_draw_buf_t *dest, const lv_area_t *dest_area,
                                const lv_draw_buf_t *src, const lv_area_t *src_area)
{
    if(dest_area != NULL && src_area != NULL &&
       dest->data == (uint8_t *)LCD_GetDrawBufferAddress() &&
       src->data == (uint8_t *)LCD_GetFrontBufferAddress() &&
       dest->header.cf == LV_COLOR_FORMAT_RGB565 &&
       src->header.cf == LV_COLOR_FORMAT_RGB565 &&
       dest->header.stride == LCD_WIDTH * LCD_BYTES_PER_PIXEL &&
       src->header.stride == LCD_WIDTH * LCD_BYTES_PER_PIXEL &&
       dest_area->x1 == src_area->x1 && dest_area->y1 == src_area->y1 &&
       dest_area->x2 == src_area->x2 && dest_area->y2 == src_area->y2) {
        uint32_t start_ms = HAL_GetTick();
        LCD_SyncRectFromFront((uint16_t)dest_area->x1, (uint16_t)dest_area->y1,
                              (uint16_t)lv_area_get_width(dest_area),
                              (uint16_t)lv_area_get_height(dest_area));
        s_profile.copy_ms += (uint32_t)(HAL_GetTick() - start_ms);
        s_profile.copied_pixels += (uint32_t)lv_area_get_size(dest_area);
        return;
    }

    s_draw_buf_copy(dest, dest_area, src, src_area);
}

static void board_flush(lv_display_t *display, const lv_area_t *area, uint8_t *pixels)
{
    (void)area;
    if(lv_display_flush_is_last(display)) {
        uint32_t start_ms = HAL_GetTick();
        LV_ASSERT((uint32_t)pixels == LCD_GetDrawBufferAddress());
        LCD_PresentBuffer((uint32_t)pixels);
        LCD_WaitForIdle();
        s_profile.flush_ms += (uint32_t)(HAL_GetTick() - start_ms);
    }
    lv_display_flush_ready(display);
}

static void board_display_profile_event(lv_event_t *event)
{
    uint32_t now_ms = HAL_GetTick();

    switch(lv_event_get_code(event)) {
        case LV_EVENT_REFR_START:
            s_refresh_start_ms = now_ms;
            s_ref_rendered = 0U;
            break;
        case LV_EVENT_RENDER_START:
            s_draw_start_ms = now_ms;
            s_drawing = 1U;
            s_ref_rendered = 1U;
            break;
        case LV_EVENT_FLUSH_START:
            if(s_drawing != 0U) {
                s_profile.draw_ms += (uint32_t)(now_ms - s_draw_start_ms);
                s_drawing = 0U;
            }
            break;
        case LV_EVENT_FLUSH_FINISH:
            s_draw_start_ms = now_ms;
            s_drawing = 1U;
            break;
        case LV_EVENT_RENDER_READY:
            if(s_drawing != 0U) {
                s_profile.draw_ms += (uint32_t)(now_ms - s_draw_start_ms);
                s_drawing = 0U;
            }
            break;
        case LV_EVENT_REFR_READY:
            if(s_ref_rendered != 0U) {
                s_profile.refresh_ms += (uint32_t)(now_ms - s_refresh_start_ms);
            }
            break;
        default:
            break;
    }
}

void Board_LVGL_GetProfile(board_lvgl_profile_t *profile)
{
    if(profile != NULL) *profile = s_profile;
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
    lv_draw_buf_handlers_t *handlers = lv_draw_buf_get_handlers();
    s_draw_buf_copy = handlers->buf_copy_cb;
    handlers->buf_copy_cb = board_draw_buf_copy;
    display = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(display, (void *)LCD_GetDrawBufferAddress(),
                           (void *)LCD_GetFrontBufferAddress(),
                           LCD_FRAMEBUFFER_BYTES, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(display, board_flush);
    lv_display_add_event_cb(display, board_display_profile_event, LV_EVENT_ALL, NULL);

    (void)FT5X16_Init();
    s_touch = lv_indev_create();
    lv_indev_set_type(s_touch, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(s_touch, board_touch_read);
    lv_indev_set_display(s_touch, display);
    lv_timer_set_period(lv_indev_get_read_timer(s_touch), BOARD_TOUCH_READ_PERIOD_MS);

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
    if(s_touch_irq != 0U) {
        /* The EXTI handler only records the edge.  Make the next foreground
         * pass sample it without waiting for the normal input timer phase. */
        lv_timer_ready(lv_indev_get_read_timer(s_touch));
    }
    lv_timer_handler();
}
