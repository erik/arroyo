#include "ast.h"
#include "buffer.h"

#include <math.h>

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

  buffer_putc (&b, '\0');

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
  // FIXME: these will all leak memory

  expression_node* left  = NULL;
  expression_node* right = NULL;

#define LEFT  (left=expression_node_evaluate (binary->lhs))
#define RIGHT (right=expression_node_evaluate (binary->rhs))
#define TYPE_CHECK(obj, tpe) { node_type _t = (obj)->type;      \
    if (_t != tpe) {                                            \
      printf ("type mismatch. (got %s, expected %s)\n",         \
              node_type_string[_t], node_type_string[tpe]);     \
      return nil_node_create ();                                \
    }                                                           \
  }
#define EXPR(type, val) expression_node_create (NODE_##type, val)
#define REAL(expr) (((real_node*)expr->ast_node)->real)
#define ARITH(op)                                                       \
  TYPE_CHECK (LEFT, NODE_REAL);                                         \
  TYPE_CHECK (RIGHT, NODE_REAL);                                        \
  return EXPR (REAL, real_node_create (REAL(left) op REAL(right)));

#define COMP(op)                                                        \
    TYPE_CHECK (LEFT, NODE_REAL);                                       \
    TYPE_CHECK (RIGHT, NODE_REAL);                                      \
    return expression_node_create (NODE_BOOL,                           \
                                   bool_node_create (REAL (left) op REAL (right) ? 1 : 0)); \

  switch (binary->op){
  case OP_ADD: ARITH (+);
  case OP_SUB: ARITH (-);
  case OP_MUL: ARITH (*);
  case OP_DIV: ARITH (/);
  case OP_MOD:
    TYPE_CHECK (LEFT, NODE_REAL);
    TYPE_CHECK (RIGHT, NODE_REAL);
    return EXPR (REAL, real_node_create ((long)REAL(left) %
                                         (long)REAL(right)));
  case OP_LT:  COMP (<);
  case OP_LTE: COMP (<=);
  case OP_GT:  COMP (>);
  case OP_GTE: COMP (>=);
  case OP_EQ:  COMP (==);
  case OP_NEQ: COMP (!=);

  case OP_AND: {
    bool_node* left_bool = bool_node_create_from_expression (LEFT);

    if (!left_bool->bool) {
      expression_node_destroy (left);
      bool_node_destroy (left_bool);
      return EXPR (BOOL, bool_node_create (0));
    }

    bool_node* right_bool = bool_node_create_from_expression (RIGHT);

    if (!right_bool->bool) {
      expression_node_destroy (left);
      bool_node_destroy (left_bool);
      expression_node_destroy (right);
      bool_node_destroy (right_bool);
      return EXPR (BOOL, bool_node_create (0));
    }

    expression_node_destroy (left);
    bool_node_destroy (left_bool);
    bool_node_destroy (right_bool);

    return right;
  }
  case OP_OR:{
    bool_node* left_bool = bool_node_create_from_expression (LEFT);
    if (left_bool->bool) return left;
    return RIGHT;
  }

  case OP_XOR:
  case OP_CONCAT:
  case OP_DOT:
  case OP_ASSIGN:
  default:
    puts ("binary node hit default, not handled yet");
    return expression_node_create (NODE_NIL, NULL);
  }

#undef LEFT
#undef RIGHT
#undef TYPE_CHECK
#undef EXPR
#undef REAL
#undef COMP
#undef ARITH
}
