#include "util.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int asprintf(char** out, const char* fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  int length = vsnprintf(NULL, 0, fmt, ap);
  va_end(ap);

  char *str = malloc(length + 1);

  va_start(ap, fmt);
  vsnprintf(str, length + 1, fmt, ap);
  va_end(ap);

  *out = str;
  return length;
}


inline char* strdup(const char* string)
{
  if(!string) return NULL;

  unsigned len = strlen(string) + 1;
  char* dup = malloc(len);

  memcpy(dup, string, len);
  return dup;
}
