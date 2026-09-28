#include "gui_log.h"
#include "main.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static char g_log_lines[GUI_LOG_MAX_LINES][GUI_LOG_LINE_LEN];
static uint16_t g_log_head;
static uint16_t g_log_count;
static uint32_t g_log_generation;

void GUI_LogInit(void)
{
  memset(g_log_lines, 0, sizeof(g_log_lines));
  g_log_head = 0U;
  g_log_count = 0U;
  g_log_generation = 0U;
}

void GUI_LogWrite(const char *text)
{
  char *line;
  uint16_t index;

  if (text == 0 || text[0] == '\0') { return; }
  index = g_log_head;
  line = g_log_lines[index];
  (void)snprintf(line, GUI_LOG_LINE_LEN, "[%06lu ms] %s",
                 (unsigned long)HAL_GetTick(), text);
  line[GUI_LOG_LINE_LEN - 1U] = '\0';
  g_log_head = (uint16_t)((g_log_head + 1U) % GUI_LOG_MAX_LINES);
  if (g_log_count < GUI_LOG_MAX_LINES) { ++g_log_count; }
  ++g_log_generation;
}

void GUI_LogPrintf(const char *format, ...)
{
  char text[GUI_LOG_LINE_LEN - 16U];
  va_list args;

  if (format == 0) { return; }
  va_start(args, format);
  (void)vsnprintf(text, sizeof(text), format, args);
  va_end(args);
  text[sizeof(text) - 1U] = '\0';
  GUI_LogWrite(text);
}

uint16_t GUI_LogCount(void) { return g_log_count; }
uint32_t GUI_LogGeneration(void) { return g_log_generation; }

uint8_t GUI_LogGetLine(uint16_t index_from_oldest, char *buffer, uint16_t length)
{
  uint16_t first;
  uint16_t index;

  if (buffer == 0 || length == 0U || index_from_oldest >= g_log_count) { return 0U; }
  first = (g_log_count < GUI_LOG_MAX_LINES) ? 0U : g_log_head;
  index = (uint16_t)((first + index_from_oldest) % GUI_LOG_MAX_LINES);
  (void)snprintf(buffer, length, "%s", g_log_lines[index]);
  return 1U;
}
