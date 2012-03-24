#include <stdio.h>
#include <stdarg.h>

#include "parse.h"
#include "lex.h"
#include "util.h"

static array_node*      parse_array      (parser_state*);
static binary_node*     parse_assignment (parser_state*);
static block_node*      parse_block      (parser_state*);
static fn_node*         parse_function   (parser_state*);
static hash_node*       parse_hash       (parser_state*);
static if_node*         parse_if         (parser_state*, int);
static loop_node*       parse_loop       (parser_state*);
static expression_node* parse_program    (parser_state*);

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
    /* const char* info = ps->t.info.string == NULL ? "" : ps->t.info.string; */
    /* char* str = tok_to_string(type); */
    /* printf("accepting =>\t%-10s\t%s\n", str, info); */
    /* free(str); */

    if (ps->info.string != NULL) free (ps->info.string);

    ps->info = (struct token_info){
      .string = strdup (ps->t.info.string),
      .number = ps->t.info.number
    };

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

// return binary/unary operator precedence (arbitrary values, higher
// precedence is higher number)
static int op_precedence (int op)
{
  switch (op) {
    // unary operators
  case '-':
  case '!':
    return 100;

    // binary operators
  case OP_DOT:
    return 90;

  case OP_MUL: case OP_DIV: case OP_MOD:
    return 80;

  case OP_ADD: case OP_SUB:
    return 70;

  case OP_LT: case OP_LTE:
  case OP_GT: case OP_GTE:
    return 65;

  case OP_EQ: case OP_NEQ:
    return 60;

  case OP_AND:
    return 55;

  case OP_OR: case OP_XOR:
    return 50;

  case OP_ASSIGN:
    return 40;
  }

  return -1;
}

/* "[" EXPRESSION* "]" */
static array_node* parse_array (parser_state* ps)
{
  array_node* array = array_node_create ();

  while (ps->t.type != ']') {
    array_node_push_expression (array, parse_expression (ps));
  }
  expect (ps, ']');

  return array;
}

/* ID "<-" EXPRESSION */
static binary_node* parse_assignment (parser_state* ps)
{
  // TODO
  parse_expression (ps);
  return NULL;
}

/* '(' EXPRESSION* ')' */
static block_node* parse_block (parser_state* ps)
{
  block_node* block = block_node_create ();

  while (ps->t.type != ')') {
    block_node_push_expression (block, parse_expression (ps));
  }

  expect (ps, ')');
  return block;
}

/* ID | PRIMITIVE | BLOCK | ASSIGNMENT | CONDITIONAL | FUNCCALL */
expression_node* parse_expression (parser_state* ps)
{
  node_type type = NODE_NIL;
  void* node = NULL;

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
  else if (accept (ps, TK_FN)) {
    type = NODE_FN;
    node = parse_function (ps);
  }

  else if (accept (ps, TK_REAL)) {
    type = NODE_REAL;
    node = real_node_create (ps->info.number);
  }

  else if (accept (ps, TK_STRING)) {
    type = NODE_STRING;
    node = string_node_create (ps->info.string);
  }

  else if (accept (ps, TK_TRUE)) {
    type = NODE_BOOL;
    node = bool_node_create (1);
  }

  else if (accept (ps, TK_FALSE)) {
    type = NODE_BOOL;
    node = bool_node_create (0);
  }

  else if (accept (ps, TK_NIL))
    type = NODE_NIL;

  else if (accept(ps, '(')) {
    type = NODE_BLOCK;
    node = parse_block(ps);
  }

  else if (accept (ps, '{')) {
    type = NODE_HASH;
    node = parse_hash (ps);
  }

  else if (accept (ps, '[')) {
    type = NODE_ARRAY;
    node = parse_array (ps);
  }

  else if (get_unaryop (ps)) {
    fprintf (stderr,
             "unop %s\n", tok_to_string (ps->t.type));

    next_token (ps);
    parse_expression (ps);
  }

  else if (accept (ps, TK_IF)) {
    type = NODE_IF;
    node = parse_if (ps, 0);
  }

  else if (accept (ps, TK_WHEN)) {
    type = NODE_IF;
    node = parse_if (ps, 1);
  }

  else if (accept (ps, TK_LOOP)) {
    type = NODE_LOOP;
    node = parse_loop (ps);
  }

  else if (accept (ps, ','))
    return parse_expression (ps);

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

    return expression_node_create (NODE_NIL, NULL);
  }


  // TODO: this approach currently evaluates left to right and makes
  // precendence difficult to implement, should be refactored
  if (get_binop (ps)) {
    int t = ps->t.type;

    next_token (ps);
    parse_expression (ps);

    printf("binop '%s'(%s)\n",
           tok_to_string (t), ps->info.string);

  }

  return expression_node_create (type, node);
}

/* "fn" "(" ID* ")" EXPRESSION */
static fn_node* parse_function (parser_state* ps)
{

  fn_node* fn = fn_node_create ();

  // named function
  if (accept (ps, TK_ID))
    fn_node_set_id (fn, ps->info.string);

  // argument list
  expect (ps, '(');

  while (accept (ps, TK_ID)) {
    char* id = strdup (ps->info.string);
    // typed argument
    if (accept (ps, ':')) {
      int type = -1;

      expect (ps, TK_ID);
      printf ("got typed argument of %s\n", ps->info.string);

      for (unsigned i = 0; i < MAX_NODE_TYPE; ++i) {
        if (!strcmp (ps->info.string, node_type_string[i])) {
          type = i;
          break;
        }
      }

      if (type == -1)
        parser_error (ps, "unrecognized type: %s", ps->info.string);

      fn_node_add_argument (fn, id, type);
    } else
      fn_node_add_argument (fn, ps->info.string, -1);
    free (id);
  }

  expect (ps, ')');

  // function body
  fn_node_set_body (fn, parse_expression (ps));

  return fn;
}

/* "{" (PRIMITIVE ":" EXPRESSION)* "}" */
static hash_node* parse_hash (parser_state* ps)
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

  return NULL;
}

/* "if" EXPRESSION EXPRESSION ("elseif" EXPRESSION EXPRESSION)* ("else" EXPRESSION)? */
static if_node* parse_if (parser_state* ps, int when)
{
  if_node* node = if_node_create ();

  // if expr
  expression_node* cond = parse_expression (ps);

  // then expr
  expression_node* body = parse_expression (ps);

  if_node_set_condition (node, cond, body);

  // don't allow elses on when expressions

  if (!when) {
    // else if expr
    while (accept (ps, TK_ELSEIF)) {
      cond = parse_expression (ps);
      body = parse_expression (ps);

      if_node_add_elseif (node, cond, body);
    }

    // else expr
    if (accept (ps, TK_ELSE)) {
      if_node_set_else (node, parse_expression (ps));
    }
  }

  return node;
}

/* "loop" EXPRESSION? "while" | "until" | "do" EXPRESSION */
static loop_node* parse_loop (parser_state* ps)
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

  return NULL;
}

static expression_node* parse_program (parser_state* ps)
{
  block_node* program = block_node_create ();

  // error handling
  if (setjmp (ps->error.buf)) {
    fprintf (stderr, "Aborting after %d error%s.\n", ps->error.count,
             ps->error.count > 1 || ps->error.count == 0 ? "s" : "");
    return expression_node_create (NODE_NIL, NULL);
  }

  while (ps->t.type != TK_EOS && ps->t.type != TK_ERROR &&
         ps->error.count < ps->error.max) {
    block_node_push_expression (program, parse_expression (ps));
  }

  expect (ps, TK_EOS);

  return expression_node_create (NODE_BLOCK, program);
}

int parse (parser_state* ps)
{
  parse_program (ps);

  if (!ps->error.count) {
    fprintf(stderr, "Parsing completed successfully\n");
    return 0;
  }

  return 1;
}

