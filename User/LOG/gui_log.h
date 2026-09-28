#ifndef GUI_LOG_H
#define GUI_LOG_H

#include <stdint.h>

#define GUI_LOG_LINE_LEN  52U
#define GUI_LOG_MAX_LINES 24U

void GUI_LogInit(void);
void GUI_LogWrite(const char *text);
void GUI_LogPrintf(const char *format, ...);
uint16_t GUI_LogCount(void);
uint32_t GUI_LogGeneration(void);
uint8_t GUI_LogGetLine(uint16_t index_from_oldest, char *buffer, uint16_t length);

#endif
