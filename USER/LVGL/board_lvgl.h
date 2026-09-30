#ifndef BOARD_LVGL_H
#define BOARD_LVGL_H

#include <stdint.h>

typedef struct {
    uint32_t refresh_ms;
    uint32_t draw_ms;
    uint32_t copy_ms;
    uint32_t flush_ms;
    uint32_t copied_pixels;
} board_lvgl_profile_t;

void Board_LVGL_Init(void);
void Board_LVGL_Process(void);
void Board_LVGL_TouchIrqNotify(void);
void Board_LVGL_GetProfile(board_lvgl_profile_t *profile);

#endif
