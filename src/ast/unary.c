#include "ast.h"
#include "buffer.h"
#include "reader.h"
#include "parse.h"
#include "lex.h"

#include <stdio.h>

static inline char* get_unary_str(enum unary_op op)
{
  switch(op) {
  case OP_UNM:
    return "-";
  case OP_NOT:
    return "!";
  case OP_PRINT:
    return "print";
  case OP_INC:
    return "++";
  case OP_QUOTE:
    return "#";
  case OP_EVAL:
    return "eval";
  case OP_REQUIRE:
    return "require";
  default:
    return "BADUNARY";
  }
}

unary_node* unary_node_create(enum unary_op op)
{
  unary_node* node = malloc(sizeof(unary_node));

  node->op = op;
  node->expr = NULL;

  return node;
}

void unary_node_destroy(unary_node* node)
{
  expression_node_destroy(node->expr);
  free(node);
}

expression_node* unary_node_evaluate(unary_node* node, scope* scope)
{
  switch(node->op) {
  case OP_UNM: {
    expression_node* expr = expression_node_evaluate(node->expr, scope);
    if(expr->type != NODE_REAL) {
      printf("unary not: expected real, not %s\n", node_type_string[expr->type]);
      break;
    }

    double inverted = -expr->node.real;

    expression_node_destroy(expr);
    return expression_node_create(NODE_REAL, (ast_node){.real = inverted});
  }
  case OP_NOT: {
    expression_node* expr = expression_node_evaluate(node->expr, scope);
    int bool = bool_node_value_of(expr);

    expression_node_destroy(expr);
    return expression_node_create(NODE_BOOL, (ast_node){.bool = !bool});
  }

  case OP_INC: {
    expression_node* expr = NULL;
    bucket* bucket = NULL;

    if(node->expr->type == NODE_ID) {
      bucket = scope_get_bucket(scope, node->expr->node.string);
      expr = bucket->value;
    } else {
      expr = expression_node_evaluate(node->expr, scope);
    }

    if(expr->type != NODE_REAL) {
      printf("unary inc: expected real, not %s\n", node_type_string[expr->type]);
      break;
    }

    double inc = expr->node.real + 1;

    if(bucket) {
      bucket->value->node.real = inc;
    } else {
      expression_node_destroy(expr);
    }

    return expression_node_create(NODE_REAL, (ast_node){.real = inc });
  }

  case OP_PRINT: {
    expression_node* expr = expression_node_evaluate(node->expr, scope);
    char* str = expression_node_to_string(expr);
    expression_node_destroy(expr);

    printf("%s\n", str);

    free(str);

    return nil_node_create();
  }

  case OP_EVAL: {
    expression_node* expr = expression_node_evaluate(node->expr, scope);

    if(expr->type != NODE_STRING) {
      printf("eval: expected string, not %s\n", node_type_string[expr->type]);
      break;
    }

    lexer_state* ls = calloc(sizeof(lexer_state), 1);
    parser_state* ps = calloc(sizeof(parser_state), 1);
    reader r;
    string_reader_create(&r, expr->node.string);
    lexer_create(ls, &r);

    ps->ls = ls;
    ps->die_on_error = 1;
    ps->error.max = 1;
    ps->t = lexer_next_token(ps->ls);

    expression_node* program = parse_program(ps);
    expression_node* eval = expression_node_evaluate(program, scope);
    expression_node_destroy(program);

    expression_node_destroy(expr);
    lexer_destroy(ls);
    free(ls);
    free(ps);
    free(r.fn_data);

    return eval;
  }

  case OP_QUOTE: {
    return expression_node_clone(node->expr);
  }

  case OP_REQUIRE: {
    expression_node* file_name = expression_node_evaluate(node->expr, scope);

    if(file_name->type != NODE_STRING) {
      printf("require: expected string, not %s\n", node_type_string[file_name->type]);
      break;
    }

    FILE* fp = fopen(file_name->node.string, "r");

    // TODO: add a search path for requires
    if(!fp) {
      printf("require: can't open file '%s'\n", file_name->node.string);
      break;
    }

    expression_node_destroy(file_name);

    lexer_state* ls = calloc(sizeof(lexer_state), 1);
    parser_state* ps = calloc(sizeof(parser_state), 1);

    reader r;

    file_reader_create(&r, fp);
    lexer_create(ls, &r);

    ps->ls = ls;
    ps->die_on_error = 0;
    ps->error.max = 20;
    ps->t = lexer_next_token(ps->ls);

    expression_node* program = parse_program(ps);
    expression_node* eval = expression_node_evaluate(program, scope);
    expression_node_destroy(eval);
    expression_node_destroy(program);

    lexer_destroy(ls);
    free(ls);
    free(ps);

    free(r.fn_data);

    fclose(fp);

    return nil_node_create();
  }

  default:
    printf("not handled / not unary operator\n");
  }

  return nil_node_create();
}

unary_node* unary_node_clone(unary_node* node)
{
  unary_node* new = unary_node_create(node->op);
  new->expr = expression_node_clone(node->expr);
  return new;
}

char* unary_node_to_string(unary_node* node)
{
  buffer b;
  buffer_create(&b, 10);

  buffer_puts(&b, get_unary_str(node->op));

  if(node->op == OP_PRINT)
    buffer_putc(&b, ' ');

  char* tmp = expression_node_to_string(node->expr);
  buffer_puts(&b, tmp);
  free(tmp);

  return b.buf;
}

char* unary_node_inspect(unary_node* node)
{
  buffer b;
  buffer_create(&b, 10);

  buffer_puts(&b, get_unary_str(node->op));

  char* tmp = expression_node_inspect(node->expr);
  buffer_puts(&b, tmp);
  free(tmp);

  return b.buf;
}

