#include <stdio.h>
#include <stdarg.h>

#include "parse.h"
#include "lex.h"

void parse_assignment  (parser_state*);
void parse_block       (parser_state*);
void parse_conditional (parser_state*);
void parse_expression  (parser_state*);
void parse_function    (parser_state*);
void parse_program     (parser_state*);

#define parser_error(fmt, ...) {                                \
    ps->errcount++;                                             \
    fprintf(stderr, "Error while parsing: " fmt, __VA_ARGS__);  \
    puts("");                                                   \
    ps->t = lexer_next_token(ps->ls); /* skip error token */    \
  }

int accept (parser_state* ps, int type)
{
  if (type == ps->t.type) {

    char *str = tok_to_string(type);
    printf("accepting %s... (%s)\n", str, ps->t.info.string);
    free(str);

    ps->t = lexer_next_token(ps->ls);
    return 1;
  }
  return 0;
}

int expect (parser_state* ps, int type)
{
  if(accept(ps, type)) return 1;

  char* want = tok_to_string (type);
  char* got  = tok_to_string (ps->t.type);

  parser_error ("expected type '%s', got '%s'", want, got);

  free(want);
  free(got);

  return 0;
}

enum binary_op get_binop(parser_state* ps)
{

  switch (ps->t.type) {
  case '+': return OP_ADD;
  case '-': return OP_SUB;
  case '*': return OP_MUL;
  case '/': return OP_DIV;
  case '%': return OP_MOD;

  case TK_NEQ: return OP_NEQ;
  case '=': return OP_EQ;
  case '<': return OP_LT;
  case TK_LTE: return OP_LTE;
  case '>': return OP_GT;
  case TK_GTE: return OP_GTE;

      // TODO
  case TK_AND: return OP_AND;
  case TK_OR: return OP_OR;
  case TK_XOR: return OP_XOR;

  default: return OP_NOTBINOP;
  }
}

/* ID "<-" EXPRESSION "." */
void parse_assignment(parser_state *ps)
{
    //  expect(ps, TK_ASSIGN);
  parse_expression(ps);
}

/* '(' EXPRESSION* ')' */
void parse_block(parser_state *ps)
{
  while(ps->t.type != ')') {
    parse_expression(ps);
  }

  expect(ps, ')');
}

/* ID | PRIMITIVE | BLOCK | ASSIGNMENT | CONDITIONAL | FUNCCALL */
void parse_expression(parser_state *ps)
{
  if(accept(ps, TK_ID)) {

      // assignment
    if(accept(ps, TK_ASSIGN)) {
      puts ("assignment");
      parse_assignment(ps);
    }

      // function call
    if(accept(ps, '(')) {
      puts ("function call");
      parse_block(ps);
    }

      // just an id
  }
  else if (accept(ps, TK_FN))
    parse_function(ps);

  else if (accept(ps, TK_REAL))
    /* AST stuff here */ ;

  else if (accept(ps, TK_STRING))
    /* AST stuff here */ ;

  else if (accept(ps, '('))
    parse_block(ps);

  else {
    parser_error("expected expression, got '%s'",
                 (char*)tok_to_string(ps->ls->t.type));
    return;
  }

  int op;

  if((op = get_binop(ps)) != OP_NOTBINOP) {
    printf(":::opr %s (%s)\n", tok_to_string(ps->t.type), ps->t.info.string);
    ps->t = lexer_next_token(ps->ls);
    parse_expression(ps);
  }

}

/* "fun" "(" ID* ")" EXPRESSION* "." */
void parse_function(parser_state *ps)
{
    // argument list
  expect(ps, '(');
  do {
  } while (accept(ps, TK_ID));
  expect(ps, ')');

    // function body
  while(ps->ls->t.type != '.') {
    parse_expression(ps);
  }

  expect(ps, '.');
}

void parse_program(parser_state *ps)
{
  while(ps->t.type != TK_EOS && ps->errcount < ERROR_MAX) {
    parse_expression(ps);
  }

  if(ps->errcount >= ERROR_MAX) {
    fprintf(stderr, "too many errors, aborting.\n");
    return;
  }

  expect(ps, TK_EOS);
}

void parse(parser_state *ps)
{
  ps->t = lexer_next_token(ps->ls);
  parse_program(ps);

  if(ps->errcount) {
    fprintf(stderr, "parsing failed\n");
  } else {
    fprintf(stderr, "no errors\n");
  }

}

