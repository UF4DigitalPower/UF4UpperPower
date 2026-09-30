#include "custom_internal.h"

static lv_obj_t *boot_overlay;
static lv_obj_t *boot_brand;
static lv_obj_t *boot_rule;
static lv_obj_t *boot_progress;
static lv_obj_t *boot_progress_text;
static lv_obj_t *boot_phase_text;
static int last_percent;

static lv_obj_t *boot_box(lv_obj_t *parent, int x, int y, int w, int h, uint16_t color)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_style_bg_color(obj, rgb(color), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(obj, 1, 0);
    lv_obj_set_style_border_color(obj, rgb(palette->border), 0);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    return obj;
}

static lv_obj_t *boot_bar(lv_obj_t *parent, int x, int y, int w, int h, uint16_t color)
{
    lv_obj_t *obj = boot_box(parent, x, y, w, h, color);
    lv_obj_set_style_border_width(obj, 0, 0);
    return obj;
}

static lv_obj_t *boot_label(lv_obj_t *parent, int x, int y, int w,
                            const char *value, const lv_font_t *font,
                            uint16_t color, lv_text_align_t align)
{
    lv_obj_t *obj = lv_label_create(parent);
    lv_obj_remove_style_all(obj);
    lv_label_set_text(obj, value);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, font->line_height);
    lv_obj_set_style_text_font(obj, font, 0);
    lv_obj_set_style_text_color(obj, rgb(color), 0);
    lv_obj_set_style_text_align(obj, align, 0);
    return obj;
}

static int smooth(int value, int start, int end)
{
    if(value <= start) return 0;
    if(value >= end) return 1000;
    int t = (value - start) * 1000 / (end - start);
    return t * t * (3000 - 2 * t) / 1000000;
}

static void boot_frame(void *unused, int32_t time)
{
    (void)unused;
    int intro = smooth(time, 35, 270);
    int reveal = smooth(time, 140, 370);
    int load = smooth(time, 240, 790);
    int exit = smooth(time, 820, 1000);
    int percent = load / 10;

    lv_obj_set_y(boot_brand, 211 + (1000 - intro) * 32 / 1000);
    lv_obj_set_style_opa(boot_brand, (lv_opa_t)(intro * 255 / 1000), 0);
    lv_obj_set_width(boot_rule, reveal * 448 / 1000);
    lv_obj_set_width(boot_progress, load * 448 / 1000);
    lv_obj_set_style_opa(boot_overlay, (lv_opa_t)(255 - exit * 255 / 1000), 0);

    if(percent != last_percent) {
        char value[8];
        snprintf(value, sizeof(value), "%d%%", percent);
        lv_label_set_text(boot_progress_text, value);
        last_percent = percent;
    }
    if(time >= 790 && time < 820 &&
       strcmp(lv_label_get_text(boot_phase_text), "READY") != 0)
        lv_label_set_text(boot_phase_text, "READY");
}

static void boot_complete(lv_anim_t *animation)
{
    (void)animation;
    lv_obj_delete(boot_overlay);
    boot_overlay = NULL;
    boot_brand = NULL;
    boot_rule = NULL;
    boot_progress = NULL;
    boot_progress_text = NULL;
    boot_phase_text = NULL;
}

void ui_boot_start(lv_obj_t *canvas)
{
    boot_overlay = boot_box(canvas, 0, 0, UI_W, UI_H, palette->bg);
    lv_obj_add_flag(boot_overlay, LV_OBJ_FLAG_CLICKABLE);
    boot_label(boot_overlay, 16, 10, 330, "UF4 DIGITAL POWER",
               &lv_font_Teko_SemiBold_20, palette->muted, LV_TEXT_ALIGN_LEFT);
    boot_label(boot_overlay, 16, 35, 330, "DC POWER SUPPLY",
               &lv_font_Teko_SemiBold_28, palette->text, LV_TEXT_ALIGN_LEFT);
    boot_box(boot_overlay, 362, 0, 118, 76, palette->disabled);
    boot_label(boot_overlay, 362, 24, 118, "BOOT",
               &lv_font_Teko_SemiBold_24, palette->muted, LV_TEXT_ALIGN_CENTER);

    static const char *const status[] = {"UF4", "DISPLAY", "CONTROL", "OUTPUT", "G474"};
    for(int i = 0; i < 5; i++) {
        boot_box(boot_overlay, i * 96, 76, 96, 42, palette->panel);
        boot_label(boot_overlay, i * 96 + 2, 85, 92, status[i],
                   &lv_font_Teko_SemiBold_16, palette->muted, LV_TEXT_ALIGN_CENTER);
    }

    boot_box(boot_overlay, 0, 118, UI_W, 385, palette->ink);
    boot_label(boot_overlay, 16, 137, 260, "STARTUP / INTERFACE",
               &lv_font_Teko_SemiBold_20, palette->bg, LV_TEXT_ALIGN_LEFT);
    boot_label(boot_overlay, 315, 139, 148, "480 X 800",
               &lv_font_Teko_SemiBold_16, palette->bg, LV_TEXT_ALIGN_RIGHT);

    boot_brand = lv_obj_create(boot_overlay);
    lv_obj_remove_style_all(boot_brand);
    lv_obj_set_pos(boot_brand, 16, 243);
    lv_obj_set_size(boot_brand, 448, 166);
    lv_obj_remove_flag(boot_brand, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    boot_bar(boot_brand, 0, 6, 6, 69, palette->accent);
    boot_label(boot_brand, 24, 0, 300, "UF4",
               &lv_font_Teko_SemiBold_46, palette->bg, LV_TEXT_ALIGN_LEFT);
    boot_label(boot_brand, 24, 65, 400, "DIGITAL POWER SUPPLY",
               &lv_font_Teko_SemiBold_28, palette->bg, LV_TEXT_ALIGN_LEFT);
    boot_label(boot_brand, 24, 108, 400, "PRECISION / CONTROL / OUTPUT",
               &lv_font_Teko_SemiBold_16, palette->muted, LV_TEXT_ALIGN_LEFT);
    lv_obj_set_style_opa(boot_brand, LV_OPA_TRANSP, 0);

    boot_rule = boot_bar(boot_overlay, 16, 432, 0, 4, palette->accent);
    boot_label(boot_overlay, 16, 466, 350, "POWER CONTROL INTERFACE",
               &lv_font_Teko_SemiBold_16, palette->bg, LV_TEXT_ALIGN_LEFT);
    boot_label(boot_overlay, 365, 466, 99, "LVGL",
               &lv_font_Teko_SemiBold_16, palette->bg, LV_TEXT_ALIGN_RIGHT);

    boot_box(boot_overlay, 0, 503, UI_W, 110, palette->panel);
    boot_label(boot_overlay, 16, 514, 280, "INTERFACE STARTUP",
               &lv_font_Teko_SemiBold_16, palette->muted, LV_TEXT_ALIGN_LEFT);
    boot_progress_text = boot_label(boot_overlay, 16, 541, 260, "0%",
                                    &lv_font_Teko_SemiBold_46, palette->text, LV_TEXT_ALIGN_LEFT);
    boot_phase_text = boot_label(boot_overlay, 280, 559, 184, "INITIALIZING",
                                 &lv_font_Teko_SemiBold_20, palette->muted, LV_TEXT_ALIGN_RIGHT);

    boot_box(boot_overlay, 0, 613, UI_W, 135, palette->bg);
    boot_label(boot_overlay, 16, 634, 300, "LOADING INTERFACE",
               &lv_font_Teko_SemiBold_20, palette->text, LV_TEXT_ALIGN_LEFT);
    boot_label(boot_overlay, 345, 636, 119, "01 / 01",
               &lv_font_Teko_SemiBold_16, palette->muted, LV_TEXT_ALIGN_RIGHT);
    boot_bar(boot_overlay, 16, 686, 448, 14, palette->disabled);
    boot_progress = boot_bar(boot_overlay, 16, 686, 0, 14, palette->accent);
    boot_label(boot_overlay, 16, 711, 448, "UF4 / DIGITAL POWER SUPPLY",
               &lv_font_Teko_SemiBold_12, palette->muted, LV_TEXT_ALIGN_LEFT);

    static const char *const nav[] = {"HOME", "PARAMS", "SETTINGS", "LOG"};
    for(int i = 0; i < 4; i++) {
        boot_box(boot_overlay, i * 120, 748, 120, 52,
                 i == 0 ? palette->ink : palette->bg);
        boot_label(boot_overlay, i * 120 + 2, 759, 116, nav[i],
                   &lv_font_Teko_SemiBold_24,
                   i == 0 ? palette->bg : palette->muted, LV_TEXT_ALIGN_CENTER);
    }

    last_percent = 0;
    lv_anim_t animation;
    lv_anim_init(&animation);
    lv_anim_set_var(&animation, boot_overlay);
    lv_anim_set_values(&animation, 0, 1000);
    lv_anim_set_duration(&animation, 2400);
    lv_anim_set_path_cb(&animation, lv_anim_path_linear);
    lv_anim_set_exec_cb(&animation, boot_frame);
    lv_anim_set_completed_cb(&animation, boot_complete);
    lv_anim_start(&animation);
}
