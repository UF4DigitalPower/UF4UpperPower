#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/times.h>

extern int __io_putchar(int ch) __attribute__((weak));

int _getpid(void) { return 1; }
int _kill(int pid, int sig) { (void)pid; (void)sig; errno = EINVAL; return -1; }
void _exit(int status) { (void)status; for (;;) {} }

int _write(int file, char *ptr, int len)
{
  int i;
  (void)file;
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
