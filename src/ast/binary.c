#include "ast.h"
#include "buffer.h"

binary_node* binary_node_create (enum binary_op op)
{
  binary_node* binary = calloc (sizeof (binary_node), 1);
  binary->lhs = NULL;
  binary->rhs = NULL;

  binary->op = op;

  return binary;
}

void binary_node_destroy (binary_node* binary)
{
  expression_node_destroy (binary->lhs);
  expression_node_destroy (binary->rhs);
  free (binary);
}

string_node* binary_node_to_string_node (binary_node* binary)
{
  buffer b;
  buffer_create (&b, 10);

  char* tmp = expression_node_to_string (binary->lhs);
  buffer_puts (&b, tmp);
  free (tmp);

  switch (binary->op) {
  case OP_ADD:    tmp = "+"; break;
  case OP_SUB:    tmp = "-"; break;
  case OP_MUL:    tmp = "*"; break;
  case OP_DIV:    tmp = "/"; break;
  case OP_MOD:    tmp = "%"; break;
  case OP_LT:     tmp = "<"; break;
  case OP_LTE:    tmp = "<="; break;
  case OP_GT:     tmp = ">"; break;
  case OP_GTE:    tmp = ">="; break;
  case OP_EQ:     tmp = "="; break;
  case OP_NEQ:    tmp = "/="; break;
  case OP_AND:    tmp = "and"; break;
  case OP_OR:     tmp = "or"; break;
  case OP_XOR:    tmp = "xor"; break;
  case OP_CONCAT: tmp = "CONCAT"; break;
  case OP_DOT:    tmp = "."; break;
  case OP_ASSIGN: tmp = "<-"; break;
  default:        tmp = "BADOP";
  }

  buffer_putc (&b, ' ');
  buffer_puts (&b, tmp);
  buffer_putc (&b, ' ');

  tmp = expression_node_to_string (binary->rhs);
  buffer_puts (&b, tmp);
  free (tmp);

  buffer_putc (&b, ' ');

  string_node* string = string_node_create (b.buf);
  buffer_destroy (&b);

  return string;
}

void binary_node_set_lhs (binary_node* bin, expression_node* left)
{
  bin->lhs = left;
}

void binary_node_set_rhs (binary_node* bin, expression_node* right)
{
  bin->rhs = right;
}

expression_node* binary_node_evaluate (binary_node* binary)
{
  // TODO
  return NULL;
}
