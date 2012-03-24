#include "buffer.h"

void buffer_create (buffer* b, unsigned sz)
{
  b->buf = calloc(sz, 1);
  b->size = sz; b->pos = 0;
}

void buffer_destroy (buffer* b)
{
  free (b->buf);
}

void buffer_putc (buffer* b, char c)
{
  if (++b->pos >= b->size) buffer_grow (b);

  b->buf [b->pos - 1] = c;
}

void buffer_putsn (buffer* b, const char* s, unsigned sz)
{
  if (b->pos + sz >= b->size) buffer_resize (b, sz + b->size);

  for (unsigned i = 0; i < sz; ++i)
    b->buf[b->pos++] = s[i];
}
