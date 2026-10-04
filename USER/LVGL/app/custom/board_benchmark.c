#include "board_benchmark.h"

#include "bsp_lcd.h"
#include "board_lvgl.h"
#include "custom_internal.h"
#include "lvgl/src/draw/dma2d/lv_draw_dma2d.h"

#define BENCH_PHASE_MS 3000U
#define BENCH_TICK_MS 16U
#define BENCH_SNAPSHOT_ADDR 0xD0700000UL
#define BENCH_PANEL_WIDTH 480U
#define BENCH_PANEL_HEIGHT 430U
#define BENCH_SNAPSHOT_BYTES (BENCH_PANEL_WIDTH * BENCH_PANEL_HEIGHT * LCD_BYTES_PER_PIXEL)

_Static_assert(BENCH_SNAPSHOT_ADDR >= LCD_PAGE_CACHE3_ADDR + LCD_PAGE_CACHE_BYTES,
               "benchmark snapshot overlaps LCD page cache");
_Static_assert(BENCH_SNAPSHOT_ADDR + BENCH_SNAPSHOT_BYTES <= 0xD0800000UL,
               "benchmark snapshot exceeds reserved SDRAM region");

static lv_obj_t *s_overlay;
static lv_obj_t *s_bench_screen;
static lv_obj_t *s_previous_screen;
static lv_obj_t *s_tile;
static lv_obj_t *s_panel;
static lv_obj_t *s_moving_panel;
static lv_draw_buf_t s_panel_snapshot;
static uint8_t s_panel_cached;
static lv_obj_t *s_phase_label;
static lv_obj_t *s_small_result;
static lv_obj_t *s_full_result;
static lv_timer_t *s_timer;
static lcd_performance_counters_t s_before;
static board_lvgl_profile_t s_profile_before;
static lv_draw_dma2d_profile_t s_dma_before;
static uint32_t s_phase_start;
static uint8_t s_phase;

static lv_obj_t *bench_box(lv_obj_t *parent, int32_t x, int32_t y,
                           int32_t w, int32_t h, uint16_t color)
{
    lv_obj_t *box = lv_obj_create(parent);
    lv_obj_remove_style_all(box);
    lv_obj_set_pos(box, x, y);
    lv_obj_set_size(box, w, h);
    lv_obj_set_style_bg_opa(box, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(box, rgb(color), 0);
    lv_obj_remove_flag(box, LV_OBJ_FLAG_SCROLLABLE);
    return box;
}

static lv_obj_t *bench_label(lv_obj_t *parent, int32_t x, int32_t y,
                             int32_t w, const char *value,
                             const lv_font_t *font, uint16_t color)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, value);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_width(label, w);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, rgb(color), 0);
    return label;
}

static void bench_begin_phase(uint8_t phase)
{
    s_phase = phase;
    s_phase_start = lv_tick_get();
    LCD_GetPerformanceCounters(&s_before);
    Board_LVGL_GetProfile(&s_profile_before);
    lv_draw_dma2d_get_profile(&s_dma_before);
    if(phase == 0U) {
        lv_obj_remove_flag(s_tile, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s_panel, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(s_phase_label, "SMALL REGION / 3 S");
    }
    else {
        lv_obj_add_flag(s_tile, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(s_moving_panel, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(s_phase_label, s_panel_cached ?
                          "LARGE CACHED / 3 S" : "LARGE PANEL / 3 S");
    }
}

static void bench_finish_phase(uint32_t elapsed)
{
    lcd_performance_counters_t after;
    board_lvgl_profile_t profile_after;
    lv_draw_dma2d_profile_t dma_after;
    lv_obj_t *result = s_phase == 0U ? s_small_result : s_full_result;
    const char *name = s_phase == 0U ? "SMALL" : "LARGE";
    uint32_t frames;
    uint32_t fps10;
    char value[192];

    LCD_GetPerformanceCounters(&after);
    Board_LVGL_GetProfile(&profile_after);
    lv_draw_dma2d_get_profile(&dma_after);
    frames = after.frames_presented - s_before.frames_presented;
    fps10 = elapsed != 0U ? frames * 10000U / elapsed : 0U;
    if(s_phase == 0U) {
        snprintf(value, sizeof(value),
                 "%s %lu.%lu FPS (%lu FRAMES)\nFIFO %lu LTDC %lu DMA %lu WAIT %lu",
                 name, (unsigned long)(fps10 / 10U), (unsigned long)(fps10 % 10U),
                 (unsigned long)frames,
                 (unsigned long)(after.ltdc_fifo_underruns - s_before.ltdc_fifo_underruns),
                 (unsigned long)(after.ltdc_transfer_errors - s_before.ltdc_transfer_errors),
                 (unsigned long)(after.dma2d_transfer_errors - s_before.dma2d_transfer_errors),
                 (unsigned long)(after.pipeline_timeouts - s_before.pipeline_timeouts));
    }
    else {
        snprintf(value, sizeof(value),
                 "%s %lu.%lu FPS (%lu FRAMES)\n"
                 "REF %lu DRAW %lu COPY %lu/%luk FLUSH %lu ms\n"
                 "DMA IMG %lu FILL %lu ms (%lu/%lu tasks)\n"
                 "FIFO %lu LTDC %lu DMA %lu WAIT %lu",
                 name, (unsigned long)(fps10 / 10U), (unsigned long)(fps10 % 10U),
                 (unsigned long)frames,
                 (unsigned long)(frames != 0U ? (profile_after.refresh_ms - s_profile_before.refresh_ms + frames / 2U) / frames : 0U),
                 (unsigned long)(frames != 0U ? (profile_after.draw_ms - s_profile_before.draw_ms + frames / 2U) / frames : 0U),
                 (unsigned long)(frames != 0U ? (profile_after.copy_ms - s_profile_before.copy_ms + frames / 2U) / frames : 0U),
                 (unsigned long)(frames != 0U ? (profile_after.copied_pixels - s_profile_before.copied_pixels + frames * 500U) / (frames * 1000U) : 0U),
                 (unsigned long)(frames != 0U ? (profile_after.flush_ms - s_profile_before.flush_ms + frames / 2U) / frames : 0U),
                 (unsigned long)(frames != 0U ? (dma_after.image_ms - s_dma_before.image_ms + frames / 2U) / frames : 0U),
                 (unsigned long)(frames != 0U ? (dma_after.fill_ms - s_dma_before.fill_ms + frames / 2U) / frames : 0U),
                 (unsigned long)(frames != 0U ? (dma_after.image_tasks - s_dma_before.image_tasks + frames / 2U) / frames : 0U),
                 (unsigned long)(frames != 0U ? (dma_after.fill_tasks - s_dma_before.fill_tasks + frames / 2U) / frames : 0U),
                 (unsigned long)(after.ltdc_fifo_underruns - s_before.ltdc_fifo_underruns),
                 (unsigned long)(after.ltdc_transfer_errors - s_before.ltdc_transfer_errors),
                 (unsigned long)(after.dma2d_transfer_errors - s_before.dma2d_transfer_errors),
                 (unsigned long)(after.pipeline_timeouts - s_before.pipeline_timeouts));
    }
    lv_label_set_text(result, value);
}

static void bench_tick(lv_timer_t *timer)
{
    uint32_t elapsed = lv_tick_elaps(s_phase_start);
    uint32_t t;
    (void)timer;

    if(elapsed >= BENCH_PHASE_MS) {
        bench_finish_phase(elapsed);
        if(s_phase == 0U) bench_begin_phase(1U);
        else {
            lv_obj_add_flag(s_moving_panel, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text(s_phase_label, "COMPLETE");
            lv_timer_delete(s_timer);
            s_timer = NULL;
        }
        return;
    }

    t = elapsed % 1200U;
    if(t > 600U) t = 1200U - t;
    if(s_phase == 0U) {
        lv_obj_set_x(s_tile, 20 + (int32_t)(t * 330U / 600U));
        lv_obj_set_y(s_tile, 230 + (int32_t)(t * 150U / 600U));
    }
    else lv_obj_set_x(s_moving_panel, -240 + (int32_t)(t * 480U / 600U));
}

static void bench_restart(lv_event_t *event)
{
    (void)event;
    if(s_timer != NULL) lv_timer_delete(s_timer);
    lv_label_set_text(s_small_result, "SMALL  --");
    lv_label_set_text(s_full_result, "LARGE  --");
    bench_begin_phase(0U);
    s_timer = lv_timer_create(bench_tick, BENCH_TICK_MS, NULL);
}

static void bench_close(lv_event_t *event)
{
    lv_obj_t *screen_to_delete = s_bench_screen;
    (void)event;
    if(s_timer != NULL) {
        lv_timer_delete(s_timer);
        s_timer = NULL;
    }
    s_overlay = NULL;
    s_bench_screen = NULL;
    lv_screen_load(s_previous_screen);
    s_previous_screen = NULL;
    lv_obj_delete_async(screen_to_delete);
}

void Board_Benchmark_Start(void)
{
    lv_obj_t *button;

    if(s_overlay != NULL) return;
    s_previous_screen = lv_screen_active();
    s_bench_screen = lv_obj_create(NULL);
    if(s_bench_screen == NULL) return;
    lv_screen_load(s_bench_screen);
    s_overlay = bench_box(s_bench_screen, 0, 0, LCD_WIDTH, LCD_HEIGHT, palette->bg);
    lv_obj_add_flag(s_overlay, LV_OBJ_FLAG_CLICKABLE);
    bench_label(s_overlay, 16, 14, 376, "DISPLAY BENCHMARK",
                &lv_font_Teko_SemiBold_28, palette->text);
    button = bench_box(s_overlay, 416, 0, 64, 56, palette->ink);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
    bench_label(button, 24, 12, 40, "X", &lv_font_Teko_SemiBold_28, palette->bg);
    lv_obj_add_event_cb(button, bench_close, LV_EVENT_CLICKED, NULL);

    s_phase_label = bench_label(s_overlay, 16, 78, 448, "SMALL REGION / 3 S",
                                &lv_font_Teko_SemiBold_20, palette->muted);
    bench_box(s_overlay, 0, 132, 480, 450, palette->ink);
    s_tile = bench_box(s_overlay, 20, 230, 112, 112, palette->accent);
    s_panel = bench_box(s_overlay, -240, 142, 480, 430, palette->panel);
    for(int i = 0; i < 8; i++) {
        int y = 18 + i * 50;
        bench_box(s_panel, 16, y, 448, 32,
                  (i & 1) != 0 ? palette->accent : palette->disabled);
        bench_label(s_panel, 32, y + 3, 400, "UF4 / DISPLAY TEST",
                    &lv_font_Teko_SemiBold_20, palette->ink);
    }
    s_moving_panel = s_panel;
    s_panel_cached = 0U;
    lv_obj_set_x(s_panel, 0);
    if(lv_draw_buf_init(&s_panel_snapshot, BENCH_PANEL_WIDTH, BENCH_PANEL_HEIGHT,
                        LV_COLOR_FORMAT_RGB565, 0, (void *)BENCH_SNAPSHOT_ADDR,
                        BENCH_SNAPSHOT_BYTES) == LV_RESULT_OK) {
        lv_draw_buf_set_flag(&s_panel_snapshot, LV_IMAGE_FLAGS_MODIFIABLE);
        if(lv_snapshot_take_to_draw_buf(s_panel, LV_COLOR_FORMAT_RGB565,
                                        &s_panel_snapshot) == LV_RESULT_OK) {
            s_moving_panel = lv_image_create(s_overlay);
            lv_image_set_src(s_moving_panel, &s_panel_snapshot);
            lv_obj_set_pos(s_moving_panel, -240, 142);
            s_panel_cached = 1U;
        }
    }
    lv_obj_set_x(s_panel, -240);
    lv_obj_add_flag(s_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s_moving_panel, LV_OBJ_FLAG_HIDDEN);

    s_small_result = bench_label(s_overlay, 16, 590, 448, "SMALL  --",
                                 &lv_font_Teko_SemiBold_16, palette->text);
    s_full_result = bench_label(s_overlay, 16, 645, 448, "LARGE  --",
                                &lv_font_Teko_SemiBold_16, palette->text);
    button = bench_box(s_overlay, 0, 744, 200, 56, palette->accent);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
    bench_label(button, 18, 756, 170, "RUN AGAIN",
                &lv_font_Teko_SemiBold_24, palette->ink);
    lv_obj_add_event_cb(button, bench_restart, LV_EVENT_CLICKED, NULL);

    bench_begin_phase(0U);
    s_timer = lv_timer_create(bench_tick, BENCH_TICK_MS, NULL);
}
