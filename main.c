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
    "main <- fn(x y z) longer_name123 <- 1.010002\n"            \
    "y <- -1,\n"                                                \
    "-- this is a comment\n"                                    \
    "z <- 1 = (1 and (2 < 3)),\n"                               \
    "print (longer_name123).\n";

  *size = strlen(prgn);
  return prgn;
}

int main (void) {

  reader r;
  reader_create (&r, string_reader, NULL);

  lexer_state *ls = calloc (sizeof (lexer_state), 1);
  lexer_create (ls, &r);

  parser_state* ps = calloc (sizeof (parser_state), 1);
  ps->ls = ls;

  // skip leading EOF token
  // FIXME: this shouldn't happen.
  lexer_next_token (ls);

  parse (ps);

  lexer_destroy (ls);
  free (ls);
  free (ps);

  return 0;
}
