#pragma once

#ifndef _READER_H_
#define _READER_H_

#include <stdlib.h>

typedef const char* (*reader_fn)(void* data, unsigned* size);

// end of stream
#define EOS (-1)

// buffered IO
typedef struct reader {
  reader_fn fn;
  void* fn_data;

  unsigned available;
  const char* ptr;
} reader;

void reader_create(reader* r, reader_fn fn, void* data);
int reader_fillbuf(reader* r);

int reader_getchar(reader* r);

#endif /* _READER_H_ */
