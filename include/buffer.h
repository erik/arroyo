#pragma once

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdlib.h>
#include <string.h>

// Provides a dynamic string / byte buffer implementation. Buffer
// grows exponentially as needed.

// This should reduce the number of reallocations needed (but will
// grow massive fairly quickly)

typedef struct buffer {
  char* buf;
  unsigned pos, size;
} buffer;

// callocing instead of a straight realloc to initialize memory (and shut valgrind up)
#define buffer_grow(b        ) {                \
    void* _ptr = calloc((b->size*=2), 1);       \
    strcpy(_ptr, b->buf);                       \
    free(b->buf);                               \
    b->buf = _ptr;                              \
  }

#define buffer_puts(b, str   ) (buffer_putsn(b, str, strlen(str)))
#define buffer_reset(b       ) (memset(b->buf, 0, b->size), (b->pos = 0))
#define buffer_resize(b, sz  ) {                \
    void* _ptr = calloc(b->size=sz, 1);         \
    strcpy(_ptr, b->buf);                       \
    free(b->buf);                               \
    b->buf= _ptr;                               \
  }

void buffer_create  (buffer* b, unsigned sz);
void buffer_destroy (buffer* b);
void buffer_putc    (buffer* b, char c);
void buffer_putsn   (buffer* b, const char* s, unsigned sz);

#endif /* _BUFFER_H_ */
