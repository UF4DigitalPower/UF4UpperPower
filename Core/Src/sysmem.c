#include <errno.h>
#include <stddef.h>
#include <stdint.h>

static uint8_t *s_heap_end;

void *_sbrk(ptrdiff_t increment)
{
  extern uint8_t _end;
  extern uint8_t _estack;
  extern uint32_t _Min_Stack_Size;
  uint8_t *stack_limit = (uint8_t *)&_estack - (uint32_t)&_Min_Stack_Size;
  uint8_t *previous;

  if (s_heap_end == NULL) s_heap_end = &_end;
  if (increment < 0 || s_heap_end + increment > stack_limit)
  {
    errno = ENOMEM;
    return (void *)-1;
  }
  previous = s_heap_end;
  s_heap_end += increment;
  return previous;
}
