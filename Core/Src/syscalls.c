#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/times.h>
#include "usart.h"

extern int __io_putchar(int ch) __attribute__((weak));

int _getpid(void) { return 1; }
int _kill(int pid, int sig) { (void)pid; (void)sig; errno = EINVAL; return -1; }
void _exit(int status) { (void)status; for (;;) {} }

int _write(int file, char *ptr, int len)
{
  int i;
  (void)file;
  if (__io_putchar == NULL)
  {
    if (len <= 0) return 0;
    if (HAL_UART_Transmit(&huart1, (uint8_t *)ptr, (uint16_t)len, 100U) != HAL_OK)
    {
      errno = EIO;
      return -1;
    }
    return len;
  }
  for (i = 0; i < len; ++i)
  {
    if (__io_putchar != NULL) (void)__io_putchar((unsigned char)ptr[i]);
  }
  return len;
}

int _read(int file, char *ptr, int len) { (void)file; (void)ptr; (void)len; return 0; }
int _close(int file) { (void)file; return -1; }
int _fstat(int file, struct stat *st) { (void)file; st->st_mode = S_IFCHR; return 0; }
int _isatty(int file) { (void)file; return 1; }
int _lseek(int file, int ptr, int dir) { (void)file; (void)ptr; (void)dir; return 0; }
int _open(char *path, int flags, ...) { (void)path; (void)flags; return -1; }
int _times(struct tms *buf) { (void)buf; return -1; }
