// for strdup
#define _BSD_SOURCE 1

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "lex.h"

#define lisalpha(c) (isalpha(c) || c == '_')
#define lisalnum(c) (isalnum(c) || c == '_')
#define lisdigit(c) (isdigit(c))
#define lisspace(c) (isspace(c) || c == ',')

#define next(ls)           (ls->current = reader_getchar(ls->r))
#define save(ls, c)        (buffer_putc(ls->buf, c))
#define save_and_next(ls)  (save(ls, ls->current), next(ls))

#define isnewline(ls)      (ls->current == '\n' || ls->current == '\r')

static void lexer_error (lexer_state* ls, const char* fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);

  // make sure buffer doesn't have any garbage
  buffer_putc (ls->buf, '\0');

  fprintf (stderr, "Syntax error on line %d at '%s': ", ls->linenum, ls->buf->buf);
  vfprintf (stderr, fmt, ap);
  fprintf (stderr, "\n");
  va_end (ap);

  longjmp (ls->error.buf, 1);
}

static void inc_line (lexer_state *ls)
{
  char first = ls->current;
  next(ls); // skip \r or \n

  // handle \r\n or \n\r
  if (isnewline(ls) && ls->current != first) next(ls);

  ls->linenum++;
}

static void read_string (lexer_state *ls, token_info *info)
{
  // skip leading "
  next(ls);

  int cont = 1;

  while(cont) {
    switch (ls->current) {
    case EOS:
      lexer_error (ls, "unexpected eof in string");
      return;
    case '\n': case '\r':
      lexer_error (ls, "unexpected new line in string");
      return;
    case '"': cont = 0; break;
    default: save_and_next(ls);
    }
  }

  info->string = strdup(ls->buf->buf);

  // skip trailing "
  next(ls);
}

static int read_id_or_reserved (lexer_state *ls, token_info *info)
{
  while(lisalnum(ls->current)) save_and_next(ls);

  // check for reserved words
  for(int i = 0; i < NUM_TOK; ++i) {
    if(!strcmp(ls->buf->buf, tokens[i]))
      return FIRST_TOK + i;
  }

  // not a reserved word, must be an id
  info->string = strdup(ls->buf->buf);
  return TK_ID;
}

static void read_numeric (lexer_state *ls, token_info *info)
{
  int seen_dot  = 0;

  for (;;) {
    if (ls->current == '.') {
      if (seen_dot) break;
      seen_dot = 1;
    }
    else if (!lisdigit (ls->current)) {
      // bad number
      if (lisalpha (ls->current)) {
        while (lisalnum (ls->current)) save_and_next (ls);
        lexer_error (ls, "badly formatted number");
      }

      // end of number
      break;
    }

    save_and_next(ls);
  }

  double number = strtod (ls->buf->buf, NULL);
  printf ("=> %lf\n", number);

  info->string = strdup(ls->buf->buf);
  info->number = number;
}

static int lex(lexer_state *ls, token_info *info)
{
  buffer_reset(ls->buf);

  if (setjmp (ls->error.buf))
    return TK_ERROR;

  for (;;) {
    switch (ls->current) {

    case '\n': case '\r': { // newline
      inc_line(ls);
      break;
    }

    case ' ': case '\t': case ',': { // whitespace
      next(ls);
      break;
    }

    case '-': { // comment or minus
      // minus
      if (next(ls) != '-') return '-';

      // comment, skip line
      while (next(ls) != EOS && !isnewline(ls));
      break;
    }

    case '=': { // EQ
      next(ls);
      return '=';
    }

    case '<': { // LT, LTE, ASSIGN
      next(ls);
      if(ls->current == '=') {
        next(ls);
        return TK_LTE;
      }
      else if(ls->current == '-'){
        next(ls);
        return TK_ASSIGN;
      }
      else return '<';
    }

    case '>': { // GT, GTE
      next(ls);
      if(ls->current == '=') return TK_GTE;
      else return '>';
    }

    case '"': { // STRING
      read_string(ls, info);
      return TK_STRING;
    }

    case EOS: { // EOS
      return TK_EOS;
    }

    default: {
      if (lisdigit(ls->current)) { // NUMERIC
        read_numeric (ls, info);
        return TK_REAL;
      }

      if (lisalpha(ls->current)) { // ID or RESERVED
        return read_id_or_reserved(ls, info);
      }

      // operators, single characters, etc.
      int c = ls->current;
      next(ls);
      return c;
    }
    }
  }
}

token lexer_next_token (lexer_state *ls)
{
  if(ls->t.info.string != NULL) { free(ls->t.info.string); ls->t.info.string = NULL; }

  ls->lastline = ls->linenum;

  // if there is a lookahead token, return that instead
  if (ls->next.type != TK_EOS) {
    ls->t = ls->next;
    ls->next.type = TK_EOS;
  }

  ls->t.type = lex(ls, &ls->t.info);

  return ls->t;
}

token lexer_lookahead (lexer_state* ls)
{
  if(ls->next.info.string != NULL) { free(ls->next.info.string); ls->next.info.string = NULL; }

  ls->next.type = lex(ls, &ls->next.info);

  return ls->next;
}

void lexer_create(lexer_state *ls, reader* r)
{
  ls->buf = calloc (sizeof (buffer), 1);
  buffer_create(ls->buf, 1);

  ls->r = r;

  ls->linenum = 1;
  ls->current = 0;

  ls->t.type = ls->next.type = TK_EOS;
}

void lexer_destroy(lexer_state *ls)
{
  buffer_destroy(ls->buf);
  free(ls->buf);
}

char* tok_to_string(int tok)
{
  char* string = calloc(20, 1);

  // single character
  if (tok < FIRST_TOK)
    string[0] = (char)tok;

  // defined token
  else if (tok < LAST_TOK)
    strcpy(string, tokens[tok - FIRST_TOK]);

  // undefined token
  else
    strcpy(string, "unknown token");

  return string;
}
