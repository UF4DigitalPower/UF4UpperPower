/*
 * Copyright 2026 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "custom_internal.h"
#include "bsp_lcd.h"

#define PAGE_SNAPSHOT_ADDR 0xD0800000UL
#define PAGE_SLIDE_MS 360U
#define PAGE_VIEW_Y 118
#define PAGE_VIEW_H 630

_Static_assert(PAGE_SNAPSHOT_ADDR >= LCD_PAGE_CACHE3_ADDR + LCD_PAGE_CACHE_BYTES,
               "page snapshots overlap reserved LCD SDRAM");
_Static_assert(PAGE_SNAPSHOT_ADDR + 2U * LCD_FRAMEBUFFER_BYTES <= 0xD2000000UL,
               "page snapshots exceed configured SDRAM geometry");

const palette_t *palette;
static lv_obj_t *root;
lv_obj_t *screen;
static lv_obj_t *content_layer;
static lv_obj_t *departing_layer;
static lv_obj_t *top_layer;
static lv_obj_t *nav_layer;
static lv_obj_t *departing_image;
static lv_obj_t *arriving_image;
static lv_obj_t *transition_screen;
static lv_obj_t *transition_view;
static lv_draw_buf_t departing_snapshot;
static lv_draw_buf_t arriving_snapshot;
lv_obj_t *nav_indicator;
lv_obj_t *nav_labels[4];
static bool transition_active;
static bool render_pending;
static int transition_direction;
static uint8_t nav_highlight_mask;
static void render_async(void *unused);

static void content_background(lv_obj_t *layer)
{
    lv_obj_remove_style_all(layer);
    lv_obj_set_size(layer, UI_W, UI_H);
    lv_obj_set_style_bg_opa(layer, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(layer, rgb(palette->bg), 0);
    lv_obj_remove_flag(layer, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
}

static bool prepare_page_snapshots(lv_obj_t *old_layer, lv_obj_t *new_layer)
{
    if(lv_draw_buf_init(&departing_snapshot, UI_W, UI_H, LV_COLOR_FORMAT_RGB565,
                        0, (void *)PAGE_SNAPSHOT_ADDR, LCD_FRAMEBUFFER_BYTES) != LV_RESULT_OK ||
       lv_draw_buf_init(&arriving_snapshot, UI_W, UI_H, LV_COLOR_FORMAT_RGB565,
                        0, (void *)(PAGE_SNAPSHOT_ADDR + LCD_FRAMEBUFFER_BYTES),
                        LCD_FRAMEBUFFER_BYTES) != LV_RESULT_OK) return false;

    lv_draw_buf_set_flag(&departing_snapshot, LV_IMAGE_FLAGS_MODIFIABLE);
    lv_draw_buf_set_flag(&arriving_snapshot, LV_IMAGE_FLAGS_MODIFIABLE);
    if(lv_snapshot_take_to_draw_buf(old_layer, LV_COLOR_FORMAT_RGB565,
                                    &departing_snapshot) != LV_RESULT_OK ||
       lv_snapshot_take_to_draw_buf(new_layer, LV_COLOR_FORMAT_RGB565,
                                    &arriving_snapshot) != LV_RESULT_OK) return false;

    transition_screen = lv_obj_create(NULL);
    if(transition_screen == NULL) return false;
    lv_obj_remove_style_all(transition_screen);
    lv_obj_remove_flag(transition_screen, LV_OBJ_FLAG_SCROLLABLE);

    transition_view = lv_obj_create(transition_screen);
    lv_obj_remove_style_all(transition_view);
    lv_obj_set_pos(transition_view, 0, PAGE_VIEW_Y);
    lv_obj_set_size(transition_view, UI_W, PAGE_VIEW_H);
    lv_obj_remove_flag(transition_view, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    departing_image = lv_image_create(transition_view);
    arriving_image = lv_image_create(transition_view);
    lv_image_set_src(departing_image, &departing_snapshot);
    lv_image_set_src(arriving_image, &arriving_snapshot);
    lv_obj_set_pos(departing_image, 0, -PAGE_VIEW_Y);
    lv_obj_set_pos(arriving_image, UI_W, -PAGE_VIEW_Y);
    lv_obj_add_flag(old_layer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(new_layer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_parent(nav_layer, transition_screen);
    lv_obj_set_parent(top_layer, transition_screen);
    lv_screen_load(transition_screen);
    return true;
}

static void render(void)
{
    palette = &palettes[settings[4].current];
    lv_obj_clean(root);
    lv_obj_remove_style_all(root);
    lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(root, rgb(palette->bg), 0);
    lv_obj_set_style_text_font(root, &lv_font_Teko_SemiBold_16, 0);
    lv_obj_remove_flag(root, LV_OBJ_FLAG_SCROLLABLE);
    content_layer = lv_obj_create(root);
    content_background(content_layer);
    screen = content_layer;
    switch(state.page) {
    case PAGE_HOME: draw_home(); break;
    case PAGE_PARAMS: draw_params(); break;
    case PAGE_SETTINGS: draw_settings(); break;
    case PAGE_LOG: draw_log(); break;
    }
    nav_layer = lv_obj_create(root);
    lv_obj_remove_style_all(nav_layer);
    lv_obj_set_size(nav_layer, UI_W, UI_H);
    lv_obj_remove_flag(nav_layer, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    screen = nav_layer;
    draw_nav();
    nav_highlight_mask = (uint8_t)(1U << state.page);
    top_layer = lv_obj_create(root);
    lv_obj_remove_style_all(top_layer);
    lv_obj_set_size(top_layer, UI_W, UI_H);
    lv_obj_remove_flag(top_layer, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    screen = top_layer;
    draw_header();
    draw_status();
    screen = root;
    board_ui_refresh();
}

static void slide_content(void *object, int32_t x)
{
    lv_obj_set_x((lv_obj_t *)object, x);
    lv_obj_set_x(departing_image != NULL ? arriving_image : content_layer,
                 x + transition_direction * UI_W);
}

static void slide_nav(void *object, int32_t x)
{
    uint8_t next_mask = 0U;
    lv_obj_set_x((lv_obj_t *)object, x);
    for(int i = 0; i < 4; i++) {
        int center = i * 120 + 60;
        if(center >= x && center < x + 120) next_mask |= (uint8_t)(1U << i);
    }
    uint8_t changed = nav_highlight_mask ^ next_mask;
    for(int i = 0; i < 4; i++) {
        if((changed & (1U << i)) != 0U) {
            lv_obj_set_style_text_color(nav_labels[i],
                                        rgb((next_mask & (1U << i)) != 0U ? palette->bg : palette->muted), 0);
        }
    }
    nav_highlight_mask = next_mask;
}

static void slide_complete(lv_anim_t *animation)
{
    (void)animation;
    if(departing_image != NULL) {
        lv_obj_t *finished_screen = transition_screen;
        lv_obj_set_parent(nav_layer, root);
        lv_obj_set_parent(top_layer, root);
        lv_obj_remove_flag(content_layer, LV_OBJ_FLAG_HIDDEN);
        lv_screen_load(root);
        lv_obj_delete_async(finished_screen);
        departing_image = NULL;
        arriving_image = NULL;
        transition_screen = NULL;
        transition_view = NULL;
    }
    lv_obj_delete(departing_layer);
    departing_layer = NULL;
    transition_active = false;
    board_ui_refresh();
    if(render_pending) lv_async_call(render_async, NULL);
}

static void animate_x(lv_obj_t *object, int32_t from, int32_t to,
                      lv_anim_exec_xcb_t callback, lv_anim_completed_cb_t completed)
{
    lv_anim_t animation;
    lv_anim_init(&animation);
    lv_anim_set_var(&animation, object);
    lv_anim_set_values(&animation, from, to);
    lv_anim_set_duration(&animation, PAGE_SLIDE_MS);
    lv_anim_set_path_cb(&animation, lv_anim_path_ease_in_out);
    lv_anim_set_exec_cb(&animation, callback);
    if(completed) lv_anim_set_completed_cb(&animation, completed);
    lv_anim_start(&animation);
}

void ui_transition_to(page_t page)
{
    page_t previous = state.page;
    int direction = page > previous ? 1 : -1;
    lv_obj_t *old_layer = content_layer;
    bool cached;
    departing_layer = old_layer;
    state.page = page;
    palette = &palettes[settings[4].current];
    content_layer = lv_obj_create(root);
    content_background(content_layer);
    lv_obj_move_to_index(content_layer, 1);
    screen = content_layer;
    switch(page) {
    case PAGE_HOME: draw_home(); break;
    case PAGE_PARAMS: draw_params(); break;
    case PAGE_SETTINGS: draw_settings(); break;
    case PAGE_LOG: draw_log(); break;
    }
    lv_obj_delete(top_layer);
    top_layer = lv_obj_create(root);
    lv_obj_remove_style_all(top_layer);
    lv_obj_set_size(top_layer, UI_W, UI_H);
    lv_obj_remove_flag(top_layer, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    screen = top_layer;
    draw_header();
    draw_status();
    screen = root;
    board_ui_refresh();
    cached = prepare_page_snapshots(old_layer, content_layer);
    transition_direction = direction;
    if(!cached) lv_obj_set_x(content_layer, direction * UI_W);
    else lv_obj_set_x(arriving_image, direction * UI_W);
    transition_active = true;
    animate_x(cached ? departing_image : old_layer,
              0, -direction * UI_W, slide_content, slide_complete);
    animate_x(nav_indicator, previous * 120, page * 120, slide_nav, NULL);
}

static void render_async(void *unused)
{
    (void)unused;
    if(transition_active) return;
    render_pending = false;
    render();
}

void ui_request_render(void)
{
    if(!render_pending) {
        render_pending = true;
        if(!transition_active) lv_async_call(render_async, NULL);
    }
}

bool ui_transition_active(void)
{
    return transition_active;
}

void ui_render_init(lv_obj_t *canvas)
{
    root = canvas;
    screen = root;
    render_pending = false;
    transition_active = false;
    departing_layer = NULL;
    departing_image = NULL;
    arriving_image = NULL;
    transition_screen = NULL;
    transition_view = NULL;
    render();
}
