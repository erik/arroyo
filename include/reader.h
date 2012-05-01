#pragma once

#ifndef _READER_H_
#define _READER_H_

#include <stdlib.h>
#include <stdio.h>

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

typedef struct string_reader_data {
  const char* string;
  int read;
} string_reader_data;

typedef struct file_reader_data {
  FILE* fp;
  char* buf;
  int done;
} file_reader_data;


void string_reader_create(reader* r, char* string);
const char* string_reader_read(void* data, unsigned* size);

void file_reader_create(reader* r, FILE* fp);
const char* file_reader_read(void* data, unsigned* size);

#endif /* _READER_H_ */
