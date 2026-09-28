#ifndef FT5X16_H
#define FT5X16_H

#include "main.h"
#include <stdint.h>

#define FT5X16_MAX_POINTS 5U
#define FT5X16_EVENT_DOWN 0U
#define FT5X16_EVENT_UP 1U
#define FT5X16_EVENT_CONTACT 2U

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint8_t event;
    uint8_t id;
} FT5X16_Point_t;

HAL_StatusTypeDef FT5X16_Init(void);
HAL_StatusTypeDef FT5X16_ReadPoints(FT5X16_Point_t *points, uint8_t *count);
uint8_t FT5X16_IsTouched(void);

#endif /* FT5X16_H */
