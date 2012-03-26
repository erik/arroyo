#include "ast.h"
#include "buffer.h"

unary_node* unary_node_create (enum unary_op op)
{
  unary_node* node = calloc (sizeof (unary_node), 1);

  node->op = op;
  node->expr = NULL;

  return node;
}

void unary_node_destroy (unary_node* node)
{
  expression_node_destroy (node->expr);
  free (node);
}

expression_node* unary_node_evaluate (unary_node* node)
{
  // TODO
  return NULL;
}

string_node* unary_node_to_string_node (unary_node* node)
{
  buffer b;
  buffer_create (&b, 10);

  switch (node->op) {
  case OP_UNM:
    buffer_puts (&b, "-");
    break;
  case OP_NOT:
    buffer_puts (&b, "!");
    break;
  default:
    buffer_puts (&b, "BADUNARY");
  }

  char* tmp = expression_node_to_string (node->expr);
  buffer_puts (&b, tmp);
  free (tmp);

  string_node* string = string_node_create (b.buf);
  buffer_destroy (&b);
  return string;
}
