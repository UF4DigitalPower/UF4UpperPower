/**
  ******************************************************************************
  * @file    gui_widget.c
  * @author  UF4
  * @date    26-6-20 下午6:22
  * @brief   UF4GUI base widget tree, controls and event dispatch services.
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

static uint8_t gui_point_in_rect(const GUI_Rect *r, int16_t x, int16_t y)
{
    return (x >= r->x) && (y >= r->y) && (x < (r->x + r->w)) && (y < (r->y + r->h));
}

static int16_t gui_map_value(int16_t value, int16_t in_min, int16_t in_max, int16_t out_min, int16_t out_max)
{
    int32_t den = (int32_t)in_max - in_min;

    if (den == 0) {
        return out_min;
    }
    return (int16_t)(out_min + (((int32_t)value - in_min) * ((int32_t)out_max - out_min)) / den);
}

static void gui_draw_label(GUI_Widget *w, const GUI_Rect *clip)
{
    (void)clip;
    GUI_DrawString(w->obj.rect.x, w->obj.rect.y, w->data.label.text, w->data.label.color, w->data.label.scale);
}

static void gui_draw_button(GUI_Widget *w, const GUI_Rect *clip)
{
    const GUI_Theme *t = GUI_GetTheme();
    GUI_Rect r = w->obj.rect;
    GUI_Color bg = w->data.button.pressed ? GUI_Blend565(t->accent, t->panel, 170U) : w->data.button.bg;

    (void)clip;
    GUI_FillRect(&r, bg);
    GUI_DrawRect(&r, t->border);
    GUI_DrawString((int16_t)(r.x + 10), (int16_t)(r.y + (r.h / 2) - 4),
                   w->data.button.text, w->data.button.fg, 2U);
}

static void gui_event_button(GUI_Widget *w, const GUI_Event *event)
{
    if ((event->type == GUI_EVENT_TOUCH_DOWN) && gui_point_in_rect(&w->obj.rect, event->x, event->y)) {
        w->data.button.pressed = 1U;
        GUI_InvalidateWidget(w);
    } else if (event->type == GUI_EVENT_TOUCH_UP) {
        uint8_t was_pressed = w->data.button.pressed;
        w->data.button.pressed = 0U;
        if (was_pressed != 0U) {
            GUI_InvalidateWidget(w);
            if (gui_point_in_rect(&w->obj.rect, event->x, event->y) && (w->data.button.on_click != 0)) {
                w->data.button.on_click(w);
            }
        }
    }
}

static void gui_draw_switch(GUI_Widget *w, const GUI_Rect *clip)
{
    const GUI_Theme *t = GUI_GetTheme();
    GUI_Rect r = w->obj.rect;
    GUI_Rect knob;

    (void)clip;
    GUI_FillRect(&r, w->data.sw.checked ? t->success : t->muted);
    GUI_DrawRect(&r, t->border);
    knob.w = (int16_t)(r.h - 6);
    knob.h = (int16_t)(r.h - 6);
    knob.y = (int16_t)(r.y + 3);
    knob.x = (int16_t)(w->data.sw.checked ? (r.x + r.w - knob.w - 3) : (r.x + 3));
    GUI_FillRect(&knob, t->text);
}

static void gui_event_switch(GUI_Widget *w, const GUI_Event *event)
{
    if ((event->type == GUI_EVENT_TOUCH_UP) && gui_point_in_rect(&w->obj.rect, event->x, event->y)) {
        w->data.sw.checked = (uint8_t)!w->data.sw.checked;
        GUI_InvalidateWidget(w);
        if (w->data.sw.on_change != 0) {
            w->data.sw.on_change(w, w->data.sw.checked);
        }
    }
}

static void gui_draw_slider(GUI_Widget *w, const GUI_Rect *clip)
{
    const GUI_Theme *t = GUI_GetTheme();
    GUI_Rect r = w->obj.rect;
    GUI_Rect track = {(int16_t)(r.x + 8), (int16_t)(r.y + r.h / 2 - 2), (int16_t)(r.w - 16), 4};
    int16_t kx = gui_map_value(w->data.slider.value, w->data.slider.min, w->data.slider.max,
                               track.x, (int16_t)(track.x + track.w));
    GUI_Rect fill = {track.x, track.y, (int16_t)(kx - track.x), track.h};
    GUI_Rect knob = {(int16_t)(kx - 5), (int16_t)(r.y + 4), 10, (int16_t)(r.h - 8)};

    (void)clip;
    GUI_FillRect(&track, t->border);
    GUI_FillRect(&fill, t->accent);
    GUI_FillRect(&knob, t->text);
}

static void gui_event_slider(GUI_Widget *w, const GUI_Event *event)
{
    GUI_Rect r = w->obj.rect;

    if ((event->type == GUI_EVENT_TOUCH_DOWN) && gui_point_in_rect(&r, event->x, event->y)) {
        w->data.slider.dragging = 1U;
    }
    if (((event->type == GUI_EVENT_TOUCH_MOVE) || (event->type == GUI_EVENT_TOUCH_DOWN)) &&
        (w->data.slider.dragging != 0U)) {
        int16_t v = gui_map_value(event->x, (int16_t)(r.x + 8), (int16_t)(r.x + r.w - 8),
                                  w->data.slider.min, w->data.slider.max);
        if (v < w->data.slider.min) {
            v = w->data.slider.min;
        }
        if (v > w->data.slider.max) {
            v = w->data.slider.max;
        }
        if (v != w->data.slider.value) {
            w->data.slider.value = v;
            GUI_InvalidateWidget(w);
            if (w->data.slider.on_change != 0) {
                w->data.slider.on_change(w, v);
            }
        }
    }
    if (event->type == GUI_EVENT_TOUCH_UP) {
        w->data.slider.dragging = 0U;
    }
}

static void gui_draw_progress(GUI_Widget *w, const GUI_Rect *clip)
{
    const GUI_Theme *t = GUI_GetTheme();
    GUI_Rect r = w->obj.rect;
    int16_t fw = gui_map_value(w->data.progress.value, w->data.progress.min, w->data.progress.max, 0, r.w);
    GUI_Rect fill = {r.x, r.y, fw, r.h};

    (void)clip;
    GUI_FillRect(&r, t->border);
    GUI_FillRect(&fill, w->data.progress.bar_color);
    GUI_DrawRect(&r, t->text);
}

static void gui_draw_menu(GUI_Widget *w, const GUI_Rect *clip)
{
    const GUI_Theme *t = GUI_GetTheme();
    uint8_t i;
    int16_t item_h = (w->data.menu.count == 0U) ? 1 : (int16_t)(w->obj.rect.h / w->data.menu.count);

    (void)clip;
    GUI_FillRect(&w->obj.rect, t->panel);
    GUI_DrawRect(&w->obj.rect, t->border);
    for (i = 0U; i < w->data.menu.count; ++i) {
        GUI_Rect ir = {w->obj.rect.x, (int16_t)(w->obj.rect.y + i * item_h), w->obj.rect.w, item_h};
        if (i == w->data.menu.selected) {
            GUI_FillRect(&ir, t->accent);
        }
        GUI_DrawString((int16_t)(ir.x + 8), (int16_t)(ir.y + 8), w->data.menu.items[i], t->text, 1U);
    }
}

static void gui_event_menu(GUI_Widget *w, const GUI_Event *event)
{
    if ((event->type == GUI_EVENT_TOUCH_UP) && gui_point_in_rect(&w->obj.rect, event->x, event->y) &&
        (w->data.menu.count != 0U)) {
        uint8_t index = (uint8_t)((event->y - w->obj.rect.y) / (w->obj.rect.h / w->data.menu.count));
        if (index >= w->data.menu.count) {
            index = (uint8_t)(w->data.menu.count - 1U);
        }
        w->data.menu.selected = index;
        GUI_InvalidateWidget(w);
        if (w->data.menu.on_select != 0) {
            w->data.menu.on_select(w, index);
        }
    }
}

static void gui_draw_window(GUI_Widget *w, const GUI_Rect *clip)
{
    const GUI_Theme *t = GUI_GetTheme();
    GUI_Rect r = w->obj.rect;
    GUI_Rect title = {r.x, r.y, r.w, 28};

    (void)clip;
    GUI_FillRect(&r, w->data.window.bg);
    GUI_FillRect(&title, t->panel);
    GUI_DrawRect(&r, t->border);
    GUI_DrawString((int16_t)(r.x + 8), (int16_t)(r.y + 8), w->data.window.title, t->text, 1U);
}

/**
  * @brief  Initializes a widget object with common default state.
  * @param  widget Pointer to widget storage.
  * @param  type Widget type.
  * @param  rect Pointer to widget rectangle.
  * @retval None
  */
void GUI_Widget_Init(GUI_Widget *widget, GUI_WidgetType type, const GUI_Rect *rect)
{
    uint8_t i;

    widget->obj.rect = *rect;
    widget->obj.visible = 1U;
    widget->obj.enabled = 1U;
    widget->obj.invalid = 1U;
    widget->obj.user_data = 0;
    widget->type = type;
    widget->draw = 0;
    widget->event = 0;
    widget->child_count = 0U;
    widget->parent = 0;
    for (i = 0U; i < GUI_MAX_CHILDREN; ++i) {
        widget->children[i] = 0;
    }
}

/**
  * @brief  Adds a child widget to a parent widget.
  * @param  parent Pointer to parent widget.
  * @param  child Pointer to child widget.
  * @retval None
  */
void GUI_Widget_AddChild(GUI_Widget *parent, GUI_Widget *child)
{
    if ((parent == 0) || (child == 0) || (parent->child_count >= GUI_MAX_CHILDREN)) {
        return;
    }
    parent->children[parent->child_count++] = child;
    child->parent = parent;
}

/**
  * @brief  Draws a widget tree recursively.
  * @param  widget Pointer to root widget.
  * @param  clip Pointer to current clip rectangle.
  * @retval None
  */
void GUI_Widget_DrawTree(GUI_Widget *widget, const GUI_Rect *clip)
{
    uint8_t i;

    if ((widget == 0) || (widget->obj.visible == 0U)) {
        return;
    }
    if (widget->draw != 0) {
        widget->draw(widget, clip);
    }
    for (i = 0U; i < widget->child_count; ++i) {
        GUI_Widget_DrawTree(widget->children[i], clip);
    }
    widget->obj.invalid = 0U;
}

/**
  * @brief  Dispatches an event through a widget tree.
  * @param  widget Pointer to root widget.
  * @param  event Pointer to event descriptor.
  * @retval None
  */
void GUI_Widget_DispatchTree(GUI_Widget *widget, const GUI_Event *event)
{
    int8_t i;

    if ((widget == 0) || (widget->obj.visible == 0U) || (widget->obj.enabled == 0U)) {
        return;
    }
    for (i = (int8_t)widget->child_count - 1; i >= 0; --i) {
        GUI_Widget_DispatchTree(widget->children[(uint8_t)i], event);
    }
    if (widget->event != 0) {
        widget->event(widget, event);
    }
}

/**
  * @brief  Creates a label widget in caller-provided storage.
  * @param  w Pointer to widget storage.
  * @param  r Pointer to widget rectangle.
  * @param  text Pointer to static or caller-owned text buffer.
  * @retval None
  */
void GUI_Label_Create(GUI_Widget *w, const GUI_Rect *r, const char *text)
{
    GUI_Widget_Init(w, GUI_WIDGET_LABEL, r);
    w->draw = gui_draw_label;
    w->data.label.text = text;
    w->data.label.color = GUI_GetTheme()->text;
    w->data.label.scale = 1U;
}

/**
  * @brief  Creates a button widget in caller-provided storage.
  * @param  w Pointer to widget storage.
  * @param  r Pointer to widget rectangle.
  * @param  text Button text.
  * @param  on_click Optional click callback.
  * @retval None
  */
void GUI_Button_Create(GUI_Widget *w, const GUI_Rect *r, const char *text, void (*on_click)(GUI_Widget *))
{
    GUI_Widget_Init(w, GUI_WIDGET_BUTTON, r);
    w->draw = gui_draw_button;
    w->event = gui_event_button;
    w->data.button.text = text;
    w->data.button.bg = GUI_GetTheme()->panel;
    w->data.button.fg = GUI_GetTheme()->text;
    w->data.button.pressed = 0U;
    w->data.button.on_click = on_click;
}

/**
  * @brief  Creates a switch widget in caller-provided storage.
  * @param  w Pointer to widget storage.
  * @param  r Pointer to widget rectangle.
  * @param  checked Initial checked state.
  * @retval None
  */
void GUI_Switch_Create(GUI_Widget *w, const GUI_Rect *r, uint8_t checked)
{
    GUI_Widget_Init(w, GUI_WIDGET_SWITCH, r);
    w->draw = gui_draw_switch;
    w->event = gui_event_switch;
    w->data.sw.checked = checked;
    w->data.sw.on_change = 0;
}

/**
  * @brief  Creates a slider widget in caller-provided storage.
  * @param  w Pointer to widget storage.
  * @param  r Pointer to widget rectangle.
  * @param  min Minimum slider value.
  * @param  max Maximum slider value.
  * @param  value Initial slider value.
  * @retval None
  */
void GUI_Slider_Create(GUI_Widget *w, const GUI_Rect *r, int16_t min, int16_t max, int16_t value)
{
    GUI_Widget_Init(w, GUI_WIDGET_SLIDER, r);
    w->draw = gui_draw_slider;
    w->event = gui_event_slider;
    w->data.slider.min = min;
    w->data.slider.max = max;
    w->data.slider.value = value;
    w->data.slider.dragging = 0U;
    w->data.slider.on_change = 0;
}

/**
  * @brief  Creates a progress bar widget in caller-provided storage.
  * @param  w Pointer to widget storage.
  * @param  r Pointer to widget rectangle.
  * @param  min Minimum progress value.
  * @param  max Maximum progress value.
  * @param  value Initial progress value.
  * @retval None
  */
void GUI_Progress_Create(GUI_Widget *w, const GUI_Rect *r, int16_t min, int16_t max, int16_t value)
{
    GUI_Widget_Init(w, GUI_WIDGET_PROGRESS, r);
    w->draw = gui_draw_progress;
    w->data.progress.min = min;
    w->data.progress.max = max;
    w->data.progress.value = value;
    w->data.progress.bar_color = GUI_GetTheme()->accent;
}

/**
  * @brief  Creates a menu widget in caller-provided storage.
  * @param  w Pointer to widget storage.
  * @param  r Pointer to widget rectangle.
  * @param  items Pointer to menu item string array.
  * @param  count Number of menu items.
  * @retval None
  */
void GUI_Menu_Create(GUI_Widget *w, const GUI_Rect *r, const char **items, uint8_t count)
{
    uint8_t i;

    GUI_Widget_Init(w, GUI_WIDGET_MENU, r);
    w->draw = gui_draw_menu;
    w->event = gui_event_menu;
    w->data.menu.count = (count > GUI_MENU_MAX_ITEMS) ? GUI_MENU_MAX_ITEMS : count;
    w->data.menu.selected = 0U;
    w->data.menu.on_select = 0;
    for (i = 0U; i < w->data.menu.count; ++i) {
        w->data.menu.items[i] = items[i];
    }
}

/**
  * @brief  Creates a window widget in caller-provided storage.
  * @param  w Pointer to widget storage.
  * @param  r Pointer to widget rectangle.
  * @param  title Window title text.
  * @retval None
  */
void GUI_Window_Create(GUI_Widget *w, const GUI_Rect *r, const char *title)
{
    GUI_Widget_Init(w, GUI_WIDGET_WINDOW, r);
    w->draw = gui_draw_window;
    w->data.window.title = title;
    w->data.window.bg = GUI_GetTheme()->bg;
}
