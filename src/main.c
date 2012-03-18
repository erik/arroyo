#include "lex.h"
#include "reader.h"
#include "parse.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

const char* readline_reader (void* dummy, unsigned* size)
{
  (void)dummy;

  const char* input = readline (">> ");

  if (input == NULL) {
    *size = 0;
    return NULL;
  }
  add_history (input);

  static char* ret = NULL;

  if (ret != NULL) free (ret);

  ret = malloc (strlen (input) + 2);
  strcpy (ret, input);
  strcat (ret, "\n");

  *size = strlen (ret);
  return ret;
}

const char* string_reader (void* dummy, unsigned* size)
{
  (void)dummy;

  static int read = 0;
  if (read) {
    *size = 0;
    return NULL;
  }

  read = 1;
  const char* prgn =
    "fn main (x:integer, y, z) (\n"                               \
    "  longer_name123 <- 1.010 + 3\n"                             \
    "  a <- [y z 4]\n"                                            \
    "  b <- {adder : fn (v) v+1, b:2}\n"                          \
    "  -- this is a comment\n"                                    \
    "  c <- 1 = 1 and 2 < 3,\n"                                   \
    "  print (longer_name123))\n";

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

  parse (ps);

  lexer_destroy (ls);
  free (ls);
  free (ps);

  return 0;
}
