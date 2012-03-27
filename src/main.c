#include "lex.h"
#include "reader.h"
#include "parse.h"
#include "ast.h"

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

  ret = malloc (strlen (input) + 3);
  strcpy (ret, input);
  strcat (ret, ",\n");

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
    "fn main (x:real y z) (\n"                                        \
    "    longer_name123 <- 1.010 + 3\n"                               \
    "    a <- [y z \"string\" 4]\n"                                   \
    "    -- b <- {adder : fn (v) v+1 b:2}\n"                          \
    "    -- this is a comment\n"                                      \
    "    c <- true and 2 < 3\n"                                       \
    "    --print (if 2 < 3 \"sane\" else \"insane\")\n"               \
    "    --print (longer_name123))\n)";

  puts (prgn);

  *size = strlen(prgn);
  return prgn;
}

int main (int argc, char** argv) {

  int dorepl = 0;

  for (int i = 1; i < argc; ++i)
    if (!strcmp (argv[i], "repl")) dorepl = 1;

  reader r;
  if (dorepl)
    reader_create (&r, readline_reader, NULL);
  else
    reader_create (&r, string_reader, NULL);

  lexer_state *ls = calloc (sizeof (lexer_state), 1);
  lexer_create (ls, &r);

  parser_state* ps = calloc (sizeof (parser_state), 1);
  ps->ls = ls;
  ps->die_on_error = 0;
  ps->error.max = 20;
  ps->t = lexer_next_token (ps->ls);

  while (ps->t.type != TK_EOS && ps->t.type != TK_ERROR) {
    expression_node* node = parse_expression (ps);

    string_node *str = expression_node_to_string_node (node);
    string_node *str2 = expression_node_to_string_node (expression_node_evaluate (node));

    printf ("==> %s\n===> %s\n", str->string, str2->string);

    string_node_destroy (str);
    expression_node_destroy (node);
  }

  lexer_destroy (ls);
  free (ls);
  free (ps);

  return 0;
}
