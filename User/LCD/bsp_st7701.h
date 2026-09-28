#ifndef __BSP_ST7701_H__
#define __BSP_ST7701_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../Core/Inc/main.h"
#include <stdint.h>

#define ST7701_CS_Pin              LCD_NSS_Pin
#define ST7701_CS_GPIO_Port        LCD_NSS_GPIO_Port
#define ST7701_SCK_Pin             LCD_SCK_Pin
#define ST7701_SCK_GPIO_Port       LCD_SCK_GPIO_Port
#define ST7701_SDA_Pin             LCD_SDA_Pin
#define ST7701_SDA_GPIO_Port       LCD_SDA_GPIO_Port
#define ST7701_RST_Pin             LCD_RST_Pin
#define ST7701_RST_GPIO_Port       LCD_RST_GPIO_Port

HAL_StatusTypeDef ST7701Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_ST7701_H__ */
