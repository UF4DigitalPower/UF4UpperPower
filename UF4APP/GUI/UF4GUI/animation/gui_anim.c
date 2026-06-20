
#include "gui.h"

static GUI_Anim g_anims[GUI_MAX_ANIM_COUNT];

static int32_t gui_anim_ease(int32_t from, int32_t to, uint32_t elapsed, uint32_t duration)
{
    int32_t delta = to - from;
    uint32_t t;
    if (duration == 0U || elapsed >= duration) {
        return to;
    }
    t = (elapsed * 1024U) / duration;
    t = (t * (2048U - t)) / 1024U;
    return from + (int32_t)((delta * (int32_t)t) / 1024);
}

GUI_Anim *GUI_Anim_Start(GUI_AnimType type, GUI_Widget *target, int32_t from, int32_t to,
                         uint32_t duration_ms, void (*on_step)(GUI_Widget *, int32_t),
                         void (*on_done)(GUI_Widget *))
{
    uint8_t i;
    for (i = 0U; i < GUI_MAX_ANIM_COUNT; ++i) {
        if (g_anims[i].active == 0U) {
            g_anims[i].active = 1U;
            g_anims[i].type = type;
            g_anims[i].target = target;
            g_anims[i].from = from;
            g_anims[i].to = to;
            g_anims[i].value = from;
            g_anims[i].duration_ms = duration_ms;
            g_anims[i].elapsed_ms = 0U;
            g_anims[i].on_step = on_step;
            g_anims[i].on_done = on_done;
            if (on_step != 0) {
                on_step(target, from);
            }
            return &g_anims[i];
        }
    }
    return 0;
}

void GUI_Anim_Update(uint32_t elapsed_ms)
{
    uint8_t i;
    for (i = 0U; i < GUI_MAX_ANIM_COUNT; ++i) {
        if (g_anims[i].active != 0U) {
            g_anims[i].elapsed_ms += elapsed_ms;
            g_anims[i].value = gui_anim_ease(g_anims[i].from, g_anims[i].to,
                                             g_anims[i].elapsed_ms, g_anims[i].duration_ms);
            if (g_anims[i].on_step != 0) {
                g_anims[i].on_step(g_anims[i].target, g_anims[i].value);
            }
            if (g_anims[i].elapsed_ms >= g_anims[i].duration_ms) {
                g_anims[i].active = 0U;
                if (g_anims[i].on_done != 0) {
                    g_anims[i].on_done(g_anims[i].target);
                }
            }
        }
    }
}
