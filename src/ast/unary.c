#include "ast.h"
#include "buffer.h"

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

    real_node* inverted = real_node_create(-((real_node*)expr->ast_node)->real);

    expression_node_destroy(expr);
    return expression_node_create(NODE_REAL, inverted);
  }
  case OP_NOT: {
    expression_node* expr = expression_node_evaluate(node->expr, scope);
    bool_node* bool = bool_node_from_expression(expr);
    bool->bool = !bool->bool;

    expression_node_destroy(expr);
    return expression_node_create(NODE_BOOL, bool);
  }

  case OP_INC: {
    expression_node* expr = NULL;
    bucket* bucket = NULL;

    if(node->expr->type == NODE_ID) {
      id_node* id = node->expr->ast_node;
      bucket = scope_get_bucket(scope, id->id);
      expr = bucket->value;
    } else {
      expr = expression_node_evaluate(node->expr, scope);
    }

    if(expr->type != NODE_REAL) {
      printf("unary inc: expected real, not %s\n", node_type_string[expr->type]);
      break;
    }

    real_node* inc = real_node_create(((real_node*)expr->ast_node)->real + 1);
    expression_node_destroy(expr);

    if(bucket)
      bucket->value = real_node_clone(inc);

    return expression_node_create(NODE_REAL, inc);
  }

  case OP_PRINT: {
    expression_node* expr = expression_node_evaluate(node->expr, scope);
    char* str = expression_node_to_string(expr);
    expression_node_destroy(expr);

    printf("%s\n", str);

    free(str);

    return nil_node_create();
  }

  case OP_QUOTE: {
    return expression_node_clone(node->expr);
  }

  default:
    printf("not handled / not unary operator\n");
  }

  return nil_node_create();
}

expression_node* unary_node_clone(unary_node* node)
{
  unary_node* new = unary_node_create(node->op);
  new->expr = expression_node_clone(node->expr);
  return expression_node_create(NODE_UNARY, new);
}

string_node* unary_node_to_string_node(unary_node* node)
{
  buffer b;
  buffer_create(&b, 10);

  buffer_puts(&b, get_unary_str(node->op));

  char* tmp = expression_node_to_string(node->expr);
  buffer_puts(&b, tmp);
  free(tmp);

  string_node* string = string_node_create(b.buf);
  buffer_destroy(&b);
  return string;
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

