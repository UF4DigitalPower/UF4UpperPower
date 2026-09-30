/*
 * Copyright 2026 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "custom_internal.h"

const palette_t *palette;
static lv_obj_t *root;
lv_obj_t *screen;
static lv_obj_t *content_layer;
static lv_obj_t *departing_layer;
static lv_obj_t *top_layer;
static lv_obj_t *nav_layer;
lv_obj_t *nav_indicator;
lv_obj_t *nav_labels[4];
static bool transition_active;
static bool render_pending;
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
    lv_obj_remove_style_all(content_layer);
    lv_obj_set_size(content_layer, UI_W, UI_H);
    lv_obj_remove_flag(content_layer, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
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
}

static void slide_nav(void *object, int32_t x)
{
    lv_obj_set_x((lv_obj_t *)object, x);
    for(int i = 0; i < 4; i++) {
        int center = i * 120 + 60;
        lv_obj_set_style_text_color(nav_labels[i],
                                    rgb(center >= x && center < x + 120 ? palette->bg : palette->muted), 0);
    }
}

static void slide_complete(lv_anim_t *animation)
{
    (void)animation;
    lv_obj_delete(departing_layer);
    departing_layer = NULL;
    transition_active = false;
}

static void animate_x(lv_obj_t *object, int32_t from, int32_t to,
                      lv_anim_exec_xcb_t callback, lv_anim_completed_cb_t completed)
{
    lv_anim_t animation;
    lv_anim_init(&animation);
    lv_anim_set_var(&animation, object);
    lv_anim_set_values(&animation, from, to);
    lv_anim_set_duration(&animation, 160);
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
    departing_layer = old_layer;
    state.page = page;
    palette = &palettes[settings[4].current];
    content_layer = lv_obj_create(root);
    lv_obj_remove_style_all(content_layer);
    lv_obj_set_size(content_layer, UI_W, UI_H);
    lv_obj_set_x(content_layer, direction * UI_W);
    lv_obj_remove_flag(content_layer, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
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
    transition_active = true;
    animate_x(old_layer, 0, -direction * UI_W, slide_content, NULL);
    animate_x(content_layer, direction * UI_W, 0, slide_content, slide_complete);
    animate_x(nav_indicator, previous * 120, page * 120, slide_nav, NULL);
}

static void render_async(void *unused)
{
    (void)unused;
    render_pending = false;
    render();
}

void ui_request_render(void)
{
    if(!render_pending) {
        render_pending = true;
        lv_async_call(render_async, NULL);
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
    render();
}
