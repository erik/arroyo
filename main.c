#include "lex.h"
#include "reader.h"
#include "parse.h"

#include <stdio.h>

const char* string_reader(void* dummy, unsigned* size)
{
  (void)dummy;

  static int read = 0;
  if(read) {
    *size = 0;
    return NULL;
  }

  read = 1;
  const char* prgn =
    //"main <- fn() longer_name123 <- 1.010002, y <- 1 + 2, print (longer_name123-1).";
    "main <- fn() longer_name123 <- 1.010002, y <- 1, 1 = 1 and 2 < 3 , print (longer_name123).";

  *size = strlen(prgn);
  return prgn;
}

int main() {

  reader r;
  reader_create(&r, string_reader, NULL);

  lexer_state *ls = calloc(sizeof(lexer_state), 1);
  lexer_create(ls, &r);

  parser_state* ps = calloc(sizeof (parser_state), 1);
  ps->ls = ls;

  // skip leading EOF token
  // FIXME: this shouldn't happen.
  lexer_next_token(ls);

  parse(ps);

  lexer_destroy(ls);
  free(ls);
  free(ps);

  return 0;
}
