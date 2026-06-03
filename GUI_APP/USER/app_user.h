#ifndef APP_USER_H
#define APP_USER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void APP_Init(void);
void APP_Tick(void);
void APP_OnExti(uint16_t gpio_pin);

#ifdef __cplusplus
}
#endif

#endif /* APP_USER_H */
