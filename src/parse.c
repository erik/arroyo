// for strdup
#define _BSD_SOURCE 1

#include <stdio.h>
#include <stdarg.h>

#include "parse.h"
#include "lex.h"

static void parse_array       (parser_state*);
static void parse_assignment  (parser_state*);
static void parse_block       (parser_state*);
static void parse_conditional (parser_state*);
static void parse_expression  (parser_state*);
static void parse_function    (parser_state*);
static void parse_hash        (parser_state*);
static void parse_if          (parser_state*);
static void parse_loop        (parser_state*);
static void parse_program     (parser_state*);

static void parser_error (parser_state* ps, const char* fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);

  fprintf (stderr, "Parse error on line %d, at '%s': ", ps->ls->linenum, tok_to_string (ps->t.type));
  vfprintf (stderr, fmt, ap);
  fprintf (stderr, "\n");

  va_end (ap);

  if (++ps->error.count > ps->error.max || ps->die_on_error)
    longjmp (ps->error.buf, 1);
}

static void next_token (parser_state* ps)
{
  ps->t = lexer_next_token(ps->ls);

  if (ps->t.type == TK_ERROR) {
    parser_error (ps, "syntax error");
  }
}

static int accept (parser_state* ps, int type)
{
  if (type == ps->t.type) {
    const char* info = ps->t.info.string == NULL ? "" : ps->t.info.string;
    char* str = tok_to_string(type);
    printf("\t%-10s\t%s\n", str, info);
    free(str);

    next_token (ps);
    return 1;
  }
  return 0;
}

static int expect (parser_state* ps, int type)
{
  if (accept (ps, type)) return 1;

  char* want = tok_to_string (type);
  char* got  = tok_to_string (ps->t.type);

  parser_error (ps, "expected '%s', got '%s'", want, got);

  free (want);
  free (got);

  return 0;
}

static enum unary_op get_unaryop (parser_state* ps)
{
  switch (ps->t.type) {
  case '-' : return OP_UNM;
  case '!' : return OP_NOT;
  default  : return OP_NOTUNOP;
  }
}

static enum binary_op get_binop (parser_state* ps)
{
  switch (ps->t.type) {
  case '.'    : return OP_DOT;

  case '+'    : return OP_ADD;
  case '-'    : return OP_SUB;
  case '*'    : return OP_MUL;
  case '/'    : return OP_DIV;
  case '%'    : return OP_MOD;

  case TK_NEQ : return OP_NEQ;
  case '='    : return OP_EQ;
  case '<'    : return OP_LT;
  case TK_LTE : return OP_LTE;
  case '>'    : return OP_GT;
  case TK_GTE : return OP_GTE;

  case TK_AND : return OP_AND;
  case TK_OR  : return OP_OR;
  case TK_XOR : return OP_XOR;

  case TK_ASSIGN : return OP_ASSIGN;

  default     : return OP_NOTBINOP;
  }
}

/* "[" EXPRESSION* "]" */
static void parse_array (parser_state* ps)
{
  while (ps->t.type != ']') {
    parse_expression (ps);
  }
  expect (ps, ']');
}

/* ID "<-" EXPRESSION */
static void parse_assignment (parser_state* ps)
{
  parse_expression (ps);
}

/* '(' EXPRESSION* ')' */
static void parse_block (parser_state* ps)
{
  while (ps->t.type != ')') {
    parse_expression (ps);
  }

  expect (ps, ')');
}

/* ID | PRIMITIVE | BLOCK | ASSIGNMENT | CONDITIONAL | FUNCCALL */
static void parse_expression (parser_state* ps)
{
  if (accept (ps, TK_ID)) {
    // function call
    if (accept (ps, '(')) {
      parse_block (ps);
    }

    // just an id
    else {
      /* AST stuff goes here */
    }
  }
  else if (accept (ps, TK_FN))
    parse_function (ps);

  else if (accept (ps, TK_REAL))
    /* AST stuff here */ ;
  else if (accept (ps, TK_STRING))
    /* AST stuff here */ ;
  else if (accept (ps, TK_TRUE))
    /* AST stuff here */ ;
  else if (accept (ps, TK_FALSE))
    /* AST stuff here */ ;
  else if (accept (ps, TK_NIL))
    /* AST stuff here */ ;

  else if (accept(ps, '('))
    parse_block(ps);

  else if (accept (ps, '{'))
    parse_hash (ps);

  else if (accept (ps, '['))
    parse_array (ps);

  else if (get_unaryop (ps)) {
    fprintf (stderr,
             "unop %s\n", tok_to_string (ps->t.type));

    next_token (ps);
    parse_expression (ps);
  }

  else if (accept (ps, TK_IF))
    parse_if (ps);

  else if (accept (ps, TK_WHEN))
    parse_expression (ps);

  else if (accept (ps, TK_LOOP))
    parse_loop (ps);

  // binary operator is handled further down, can't start an expression by itself
  else if (get_binop (ps))
    parser_error (ps, "unexpected binary operator");

  // bad tokens, should be eaten by relevant parse_xxxx functions
  else {
    switch (ps->t.type) {
    case ')': case ']': case '}':
      parser_error (ps, "unmatched closing brace");
      break;

    default:
      parser_error (ps, "expected expression, got '%s'",
                    (char*)tok_to_string (ps->ls->t.type));
    }

    return;
  }


  // TODO: this approach currently evaluates left to right and makes
  // precendence difficult to implement, should be refactored
  if (get_binop (ps)) {
    printf("binop %s %s\n",
           tok_to_string (ps->t.type), ps->t.info.string);

    next_token (ps);
    parse_expression (ps);
  }
}

/* "fn" "(" ID* ")" EXPRESSION */
static void parse_function (parser_state* ps)
{

  // named function
  if (accept (ps, TK_ID))
    printf ("named func\n");

  // argument list
  expect (ps, '(');

  do {
    // typed argument
    if (accept (ps, ':')) {
      printf ("got typed argument of %s\n", ps->t.info.string);
      expect (ps, TK_ID);
    }
  } while (accept (ps, TK_ID));

  expect (ps, ')');

  // function body
  parse_expression (ps);
}

/* "{" (PRIMITIVE ":" EXPRESSION)* "}" */
static void parse_hash (parser_state* ps)
{
  while(ps->t.type != '}') {
    /* TODO: AST things here */

    if (accept (ps, TK_STRING)) ;
    else if (accept (ps, TK_ID));
    else
      parser_error (ps, "invalid key type");

    expect (ps, ':');

    parse_expression (ps);
  }
  expect (ps, '}');
}

/* "if" EXPRESSION EXPRESSION ("elseif" EXPRESSION EXPRESSION)* ("else" EXPRESSION)? */
static void parse_if (parser_state* ps)
{
  // if expr
  parse_expression (ps);

  // then expr
  parse_expression (ps);

  // else if expr
  while (accept (ps, TK_ELSEIF))
    parse_expression (ps);

  // else expr
  if (accept (ps, TK_ELSE))
    parse_expression (ps);
}

/* "loop" EXPRESSION? "while" | "until" | "do" EXPRESSION */
static void parse_loop (parser_state* ps)
{
  // no expression
  if (accept (ps, TK_WHILE))      ;
  else if (accept (ps, TK_UNTIL)) ;
  else if (accept (ps, TK_DO))    ;

  // using expression
  else {
    parse_expression (ps);

    if (accept (ps, TK_WHILE))      ;
    else if (accept (ps, TK_UNTIL)) ;
    else if (accept (ps, TK_DO))    ;

    else
      parser_error (ps, "expected loop conditional type");
  }

  parse_expression (ps);
}

static void parse_program (parser_state* ps)
{
  // error handling
  if (setjmp (ps->error.buf)) {
    fprintf (stderr, "Aborting after %d error%s.\n", ps->error.count,
             ps->error.count > 1 || ps->error.count == 0 ? "s" : "");
    return;
  }

  while (ps->t.type != TK_EOS && ps->t.type != TK_ERROR &&
         ps->error.count < ps->error.max) {
    parse_expression (ps);
  }

  expect (ps, TK_EOS);
}

int parse (parser_state* ps)
{
  ps->error.max = 20;

  ps->t = lexer_next_token (ps->ls);

  parse_program (ps);

  if (!ps->error.count) {
    fprintf(stderr, "Parsing completed successfully\n");
    return 0;
  }

  return 1;
}

