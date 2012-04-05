#include "reader.h"


void reader_create(reader* r, reader_fn fn, void* data)
{
  r->fn = fn;
  r->fn_data = data;

  r->available = 0;
  r->ptr = NULL;
}

int reader_fillbuf(reader* r)
{
  unsigned size;
  const char* buf = r->fn(r->fn_data, &size);

  if(buf == NULL || size == 0) {
    return EOS;
  }

  // size - 1 because last char is returned
  r->available = size - 1;
  r->ptr = buf;
  return *(r->ptr++);
}

int reader_getchar(reader* r)
{
  if(r->available-- > 0)
    return *(r->ptr++);

  return reader_fillbuf(r);
}
