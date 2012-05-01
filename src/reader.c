#include "reader.h"
#include <string.h>

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

// the reader's fn_data must be freed manually
void string_reader_create(reader* r, char* string)
{
  r->fn = string_reader_read;

  string_reader_data* dat = calloc(sizeof(string_reader_data), 1);
  dat->string = string;

  r->fn_data = dat;

  r->available = 0;
  r->ptr = NULL;
}

const char* string_reader_read(void* data, unsigned* size)
{
  string_reader_data* dat = data;

  if(dat->read) {
    *size = 0;
    return NULL;
  }

  dat->read = 1;
  *size = strlen(dat->string);
  return dat->string;
}

// the reader's fn_data must be freed manually
void file_reader_create(reader* r, FILE* fp)
{
  r->fn = file_reader_read;

  file_reader_data* dat = calloc(sizeof(file_reader_data), 1);
  dat->fp = fp;

  r->fn_data = dat;
  r->available = 0;
  r->ptr = NULL;
}

const char* file_reader_read(void* data, unsigned* size)
{
  struct file_reader_data* file = data;

  if(file->buf)
    free(file->buf);

  if(file->done) {
    *size = 0;
    return NULL;
  }

  file->buf = malloc(4096);

  *size = fread(file->buf, 1, 4096, file->fp);

  if(*size < 4096)
    file->done = 1;

  return file->buf;
}
