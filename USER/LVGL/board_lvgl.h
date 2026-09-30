#ifndef BOARD_LVGL_H
#define BOARD_LVGL_H

#include <stdint.h>

void Board_LVGL_Init(void);
void Board_LVGL_Process(void);
void Board_LVGL_TouchIrqNotify(void);

#endif
