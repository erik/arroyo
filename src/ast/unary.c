#include "ast.h"
#include "buffer.h"

#include <stdio.h>

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
  expression_node* expr = expression_node_evaluate(node->expr, scope);

  switch(node->op) {
  case OP_UNM: {
    if(expr->type != NODE_REAL) {
      printf("unary not: expected real, not %s\n", node_type_string[expr->type]);
      break;
    }

    real_node* inverted = real_node_create(-((real_node*)expr->ast_node)->real);

    expression_node_destroy(expr);
    return expression_node_create(NODE_REAL, inverted);
  }
  case OP_NOT: {
    bool_node* bool = bool_node_from_expression(expr);
    bool->bool = !bool->bool;

    expression_node_destroy(expr);
    return expression_node_create(NODE_BOOL, bool);
  }

  case OP_PRINT: {
    expression_node* expr = expression_node_evaluate(node->expr, scope);
    char* str = expression_node_to_string(expr);
    expression_node_destroy(expr);

    printf("%s\n", str);

    free(str);

    return  nil_node_create();
  }

  default:
    printf("not handled / not unary operator\n");
  }

  expression_node_destroy(expr);
  return expression_node_create(NODE_NIL, NULL);
}

expression_node* unary_node_clone(unary_node* node)
{
  // TODO
  return NULL;
}

string_node* unary_node_to_string_node(unary_node* node)
{
  buffer b;
  buffer_create(&b, 10);

  switch(node->op) {
  case OP_UNM:
    buffer_puts(&b, "-");
    break;
  case OP_NOT:
    buffer_puts(&b, "!");
    break;
  default:
    buffer_puts(&b, "BADUNARY");
  }

  char* tmp = expression_node_to_string(node->expr);
  buffer_puts(&b, tmp);
  free(tmp);

  string_node* string = string_node_create(b.buf);
  buffer_destroy(&b);
  return string;
}
