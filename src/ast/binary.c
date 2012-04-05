#include "scope.h"
#include "ast.h"
#include "buffer.h"
#include "util.h"

#include <math.h>
#include <stdio.h>

static inline char* get_binop_str(enum binary_op op)
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
    return "CONCAT";
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

string_node* binary_node_to_string_node(binary_node* binary)
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

  string_node* string = string_node_create(b.buf);
  buffer_destroy(&b);

  return string;
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

expression_node* binary_node_clone(binary_node* binary)
{
  // TODO
  return NULL;
}

expression_node* binary_node_evaluate(binary_node* binary, scope* scope)
{
  // FIXME: these will all leak memory

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

#define REAL(expr) (((real_node*)expr->ast_node)->real)
#define CAST(expr, type) ((type)expr->ast_node)

#define ARITH(op) {                                                     \
    TYPE_CHECK(LEFT, NODE_REAL);                                        \
    TYPE_CHECK(RIGHT, NODE_REAL);                                       \
    real_node* real = real_node_create(REAL(left) op REAL(right));      \
    expression_node_destroy(left);                                      \
    expression_node_destroy(right);                                     \
    return expression_node_create(NODE_REAL, real);                     \
  }

#define COMP(op) {                                                      \
    TYPE_CHECK(LEFT, NODE_REAL);                                        \
    TYPE_CHECK(RIGHT, NODE_REAL);                                       \
    bool_node* b = bool_node_create(REAL(left) op REAL(right) ? 1 : 0); \
    expression_node_destroy(left);                                      \
    expression_node_destroy(right);                                     \
    return expression_node_create(NODE_BOOL, b);                        \
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
    real_node* real = real_node_create((long)REAL(left) %
                                       (long)REAL(right));
    expression_node_destroy(left);
    expression_node_destroy(right);
    return EXPR(REAL, real);
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

    // make sure types match
    TYPE_CHECK(right, left->type);

    switch(left->type) {
    case NODE_REAL:
      COMP(==);
      break;
    case NODE_BOOL: {
      bool_node* lb = CAST(left, bool_node*);
      bool_node* rb = CAST(right, bool_node*);

      bool_node* b = bool_node_create(lb->bool == rb->bool);

      expression_node_destroy(right);
      expression_node_destroy(left);

      return EXPR(BOOL, b);
    }
    default:
      printf("this comparision is not handled yet\n");
      return expression_node_create(NODE_BOOL, bool_node_create(0));
    }

    break;
  }
  case OP_NEQ:
    COMP(!=);

  case OP_AND: {
    bool_node* left_bool = bool_node_from_expression(LEFT);

    if(!left_bool->bool) {
      expression_node_destroy(left);
      bool_node_destroy(left_bool);
      return EXPR(BOOL, bool_node_create(0));
    }

    bool_node* right_bool = bool_node_from_expression(RIGHT);

    if(!right_bool->bool) {
      expression_node_destroy(left);
      bool_node_destroy(left_bool);
      expression_node_destroy(right);
      bool_node_destroy(right_bool);
      return EXPR(BOOL, bool_node_create(0));
    }

    expression_node_destroy(left);
    bool_node_destroy(left_bool);
    bool_node_destroy(right_bool);

    return right;
  }
  case OP_OR: {
    bool_node* left_bool = bool_node_from_expression(LEFT);
    if(left_bool->bool) return left;
    return RIGHT;
  }

  case OP_ASSIGN: {
    TYPE_CHECK(binary->lhs, NODE_ID);
    id_node* id = binary->lhs->ast_node;

    right = expression_node_evaluate(binary->rhs, scope);

    // remove old binding if it exists
    bucket* existing = scope_get_bucket(scope, id->id);
    if(existing) {
      expression_node_destroy(existing->value);
      existing->value = expression_node_clone(right);

      return right;
    }

    scope_insert(scope, strdup(id->id), expression_node_clone(right));

    return right;
  }

  case OP_XOR:
  case OP_CONCAT:
  case OP_DOT:
  default:
    puts("binary node hit default, not handled yet");
    return nil_node_create();
  }

#undef LEFT
#undef RIGHT
#undef TYPE_CHECK
#undef EXPR
#undef REAL
#undef COMP
#undef ARITH
}
