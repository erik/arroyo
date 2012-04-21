#include "scope.h"
#include "ast.h"
#include "buffer.h"
#include "util.h"

#include <math.h>
#include <stdio.h>

inline char* get_binop_str(enum binary_op op)
{
  switch(op) {
  case OP_ADD:
    return "+";
  case OP_SUB:
    return "-";
  case OP_MUL:
    return "*";
  case OP_DIV:
    return "/";
  case OP_MOD:
    return "%";

  case OP_LT:
    return "<";
  case OP_LTE:
    return "<=";
  case OP_GT:
    return ">";
  case OP_GTE:
    return ">=";
  case OP_EQ:
    return "=";
  case OP_NEQ:
    return "/=";

  case OP_AND:
    return "and";
  case OP_OR:
    return "or";
  case OP_XOR:
    return "xor";

  case OP_CONCAT:
    return "..";
  case OP_DOT:
    return ".";
  case OP_ASSIGN:
    return "<-";
  default:
    return "BADOP";
  }
}

binary_node* binary_node_create(enum binary_op op)
{
  binary_node* binary = malloc(sizeof(binary_node));
  binary->lhs = NULL;
  binary->rhs = NULL;

  binary->op = op;

  return binary;
}

void binary_node_destroy(binary_node* binary)
{
  expression_node_destroy(binary->lhs);
  expression_node_destroy(binary->rhs);
  free(binary);
}

char* binary_node_to_string(binary_node* binary)
{
  buffer b;
  buffer_create(&b, 10);

  char* tmp = expression_node_to_string(binary->lhs);
  buffer_puts(&b, tmp);
  free(tmp);

  buffer_putc(&b, ' ');
  buffer_puts(&b, get_binop_str(binary->op));
  buffer_putc(&b, ' ');

  tmp = expression_node_to_string(binary->rhs);
  buffer_puts(&b, tmp);
  free(tmp);

  buffer_putc(&b, '\0');

  return b.buf;
}

char* binary_node_inspect(binary_node* binary)
{
  buffer b;
  buffer_create(&b, 10);

  char* tmp = expression_node_inspect(binary->lhs);
  buffer_puts(&b, tmp);
  free(tmp);

  buffer_putc(&b, ' ');
  buffer_puts(&b, get_binop_str(binary->op));
  buffer_putc(&b, ' ');

  tmp = expression_node_inspect(binary->rhs);
  buffer_puts(&b, tmp);
  free(tmp);

  buffer_putc(&b, '\0');

  return b.buf;
}

binary_node* binary_node_clone(binary_node* binary)
{
  binary_node* new = binary_node_create(binary->op);

  new->lhs = expression_node_clone(binary->lhs);
  new->rhs = expression_node_clone(binary->rhs);

  return new;
}

expression_node* binary_node_evaluate(binary_node* binary, scope* scope)
{
  expression_node* left  = NULL;
  expression_node* right = NULL;

#define LEFT  (left = expression_node_evaluate(binary->lhs, scope))
#define RIGHT (right = expression_node_evaluate(binary->rhs, scope))

#define TYPE_CHECK(obj, tpe) {                                  \
    node_type _t = (obj)->type;                                 \
    if(_t != tpe) {                                             \
      printf("type mismatch. (got %s, expected %s)\n",          \
             node_type_string[_t], node_type_string[tpe]);      \
      return nil_node_create();                                 \
    }                                                           \
  }

#define EXPR(type, val) expression_node_create(NODE_##type, val)

#define ARITH(op) {                                                     \
    TYPE_CHECK(LEFT, NODE_REAL);                                        \
    TYPE_CHECK(RIGHT, NODE_REAL);                                       \
    double real = left->node.real op right->node.real;                  \
    expression_node_destroy(left);                                      \
    expression_node_destroy(right);                                     \
    return expression_node_create(NODE_REAL, (ast_node){.real = real}); \
  }

#define COMP(op) {                                                      \
    TYPE_CHECK(LEFT, NODE_REAL);                                        \
    TYPE_CHECK(RIGHT, NODE_REAL);                                       \
    char b = left->node.real op right->node.real ? 1 : 0;              \
    expression_node_destroy(left);                                      \
    expression_node_destroy(right);                                     \
    return expression_node_create(NODE_BOOL, (ast_node){.bool = b});    \
  }

  switch(binary->op) {
  case OP_ADD:
    ARITH(+);
  case OP_SUB:
    ARITH(-);
  case OP_MUL:
    ARITH(*);
  case OP_DIV:
    ARITH(/);
  case OP_MOD: {
    TYPE_CHECK(LEFT, NODE_REAL);
    TYPE_CHECK(RIGHT, NODE_REAL);

    double real = (long)left->node.real %
      (long)right->node.real;

    expression_node_destroy(left);
    expression_node_destroy(right);
    return EXPR(REAL, (ast_node){.real = real});
  }
  case OP_LT:
    COMP(<);
  case OP_LTE:
    COMP(<=);
  case OP_GT:
    COMP(>);
  case OP_GTE:
    COMP(>=);

  case OP_EQ: {
    left = expression_node_evaluate(binary->lhs, scope);
    right = expression_node_evaluate(binary->rhs, scope);

    ast_node bool = {.bool = expression_node_equal(left, right)};

    expression_node_destroy(left);
    expression_node_destroy(right);

    return EXPR(BOOL, bool);
  }
  case OP_NEQ: {
    left = expression_node_evaluate(binary->lhs, scope);
    right = expression_node_evaluate(binary->rhs, scope);

    ast_node bool = {.bool = !expression_node_equal(left, right)};

    expression_node_destroy(left);
    expression_node_destroy(right);

    return EXPR(BOOL, bool);
  }

  case OP_AND: {
    int left_bool = bool_node_value_of(LEFT);

    if(!left_bool) {
      expression_node_destroy(left);
      return EXPR(BOOL, (ast_node){.bool = 0});
    }

    int right_bool = bool_node_value_of(RIGHT);

    if(!right_bool) {
      expression_node_destroy(left);
      expression_node_destroy(right);

      return EXPR(BOOL, (ast_node){.bool = 0});
    }

    expression_node_destroy(left);

    return right;
  }
  case OP_OR: {
    int left_bool = bool_node_value_of(LEFT);

    if(left_bool)
      return left;

    expression_node_destroy(left);

    return RIGHT;
  }

  case OP_ASSIGN: {
    TYPE_CHECK(binary->lhs, NODE_ID);

    right = expression_node_evaluate(binary->rhs, scope);

    // remove old binding if it exists
    bucket* existing = scope_get_bucket(scope, binary->lhs->node.string);

    if(existing) {
      expression_node_destroy(existing->value);
      existing->value = expression_node_clone(right);

      return right;
    }

    scope_insert(scope, strdup(binary->lhs->node.string), expression_node_clone(right));

    return right;
  }

  case OP_CONCAT: {
    TYPE_CHECK(LEFT, NODE_STRING);
    right = expression_node_evaluate(binary->rhs, scope);

    char* ls = left->node.string;

    char* right_str = expression_node_to_string(right);

    buffer b;
    buffer_create(&b, strlen(ls));

    buffer_puts(&b, ls);
    buffer_puts(&b, right_str);

    free(right_str);
    expression_node_destroy(left);
    expression_node_destroy(right);

    return expression_node_create(NODE_STRING, (ast_node){.string = b.buf});
  }

  case OP_XOR:
  case OP_DOT:
  default:
    puts("binary node hit default, not handled yet");
    return nil_node_create();
  }

#undef LEFT
#undef RIGHT
#undef TYPE_CHECK
#undef EXPR
#undef COMP
#undef ARITH
}
