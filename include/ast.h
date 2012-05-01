#pragma once

#ifndef _AST_H_
#define _AST_H_

#include "scope.h"

#include <stdlib.h>

typedef enum {
  NODE_LITERAL_STRING,
  NODE_STRING,
  NODE_REAL,
  NODE_BOOL,
  NODE_ID,
  NODE_NIL,

  NODE_FN,
  NODE_ARRAY,
  NODE_HASH,

  NODE_BINARY,
  NODE_UNARY,

  NODE_LOOP,
  NODE_IF,
  NODE_CASE,

  NODE_BLOCK,

  MAX_NODE_TYPE
} node_type;

static const char* node_type_string[MAX_NODE_TYPE] = {
  "literal_string", "string", "real", "bool", "id", "nil",
  "function", "array", "hash", "binary", "unary",
  "loop", "if", "case", "block"
};

struct typed_id {
  char* id;
  int arg_type; // -1 for untyped
};

enum loop_type {
  LOOP_WHILE,
  LOOP_UNTIL,
  LOOP_DO
};

enum unary_op {
  OP_NOTUNOP = 0,
  OP_UNM, OP_NOT,
  OP_INC,
  OP_PRINT,
  OP_REQUIRE,
  OP_EVAL,
  OP_QUOTE
};

enum binary_op {
  OP_NOTBINOP = 0,
  // arithmetic
  OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD,
  // conditionals
  OP_LT, OP_LTE, OP_GT, OP_GTE, OP_EQ, OP_NEQ,
  // boolean
  OP_AND, OP_OR, OP_XOR,
  // stringconcat
  OP_CONCAT,
  // members
  OP_DOT,
  OP_ASSIGN,
  OP_CALL
};

struct expression_node;

typedef struct {
  unsigned nelements;
  struct expression_node** elements;
} array_node;

typedef struct {
  struct expression_node* lhs;
  struct expression_node* rhs;
  enum binary_op op;
} binary_node;

typedef struct {
  struct expression_list {
    struct expression_node* expression;
    struct expression_list* next;
  } expression_list;
  unsigned nelements;
  struct expression_list* list, *last;

  scope* scope;
} block_node;

typedef struct {
  struct case_list {
    struct expression_node* cond;
    struct expression_node* body;
    struct case_list* next;
  } *cases, *last;

  struct expression_node* expression;

  struct expression_node* default_case;
} case_node;

typedef struct {
  char* id;
  struct typed_id* args;
  unsigned int nargs;
  struct expression_node* body;
} fn_node;

typedef struct {
  enum loop_type type;
  struct expression_node* init;
  struct expression_node* cond;
  struct expression_node* body;
} loop_node;

typedef struct {
  // TODO
  int dummy;
} hash_node;

typedef struct {
  struct expression_node* condition;
  struct expression_node* thenbody;

  unsigned nelseif;
  struct expression_node** elseifcondition;
  struct expression_node** elseifbody;

  struct expression_node* elsebody;
} if_node;

typedef struct {
  struct expression_node* expr;
  enum unary_op op;
} unary_node;

typedef union {
  // primitives
  double real;
  char* string; // also used for IDs
  int bool;

  // composite
  array_node* array;
  binary_node* binary;
  block_node* block;
  case_node* case_;
  fn_node* fn;
  if_node* if_;
  loop_node* loop;
  unary_node* unary;

} ast_node;

typedef struct expression_node {
  node_type type;
  ast_node  node;
} expression_node;

// expression
expression_node* expression_node_create(node_type, ast_node);
void             expression_node_destroy(expression_node*);
expression_node* expression_node_evaluate(expression_node*, scope*);
expression_node* expression_node_clone(expression_node*);
expression_node* expression_node_call(expression_node*, expression_node*, scope*);
expression_node* expression_node_to_string_node(expression_node*);
char*            expression_node_to_string(expression_node*);
char*            expression_node_inspect(expression_node*);
int              expression_node_equal(expression_node*, expression_node*);

// array node
array_node*      array_node_create(void);
void             array_node_destroy(array_node*);
expression_node* array_node_evaluate(array_node*, scope*);
array_node*      array_node_clone(array_node*);
expression_node* array_node_call(array_node*, expression_node*, scope*);
char*            array_node_to_string(array_node*);
void             array_node_push_expression(array_node*, expression_node*);
char*            array_node_inspect(array_node*);

// binary
binary_node*     binary_node_create(enum binary_op);
void             binary_node_destroy(binary_node*);
expression_node* binary_node_evaluate(binary_node*, scope*);
binary_node*     binary_node_clone(binary_node*);
char*            binary_node_to_string(binary_node*);
char*            binary_node_inspect(binary_node*);
char*            get_binop_str(enum binary_op);

// block node
block_node*      block_node_create(void);
void             block_node_destroy(block_node*);
expression_node* block_node_evaluate(block_node*, scope*);
block_node*      block_node_clone(block_node*);
char*            block_node_to_string(block_node*);
void             block_node_push_expression(block_node*, expression_node*);
char*            block_node_inspect(block_node*);

// bool node
char*            bool_node_to_string(expression_node*);
expression_node* bool_node_from_expression(expression_node*);
int              bool_node_value_of(expression_node*);
char*            bool_node_inspect(expression_node*);

// case
case_node*       case_node_create(void);
void             case_node_destroy(case_node*);
expression_node* case_node_evaluate(case_node*, scope*);
case_node*       case_node_clone(case_node*);
char*            case_node_to_string(case_node*);
char*            case_node_inspect(case_node*);
void             case_node_add_case(case_node* node, expression_node* of, expression_node* body);

// fn node
fn_node*         fn_node_create(void);
void             fn_node_destroy(fn_node*);
expression_node* fn_node_evaluate(fn_node*, scope*);
fn_node*         fn_node_clone(fn_node*);
expression_node* fn_node_call(fn_node*, expression_node*, scope*);
char*            fn_node_to_string(fn_node*);
void             fn_node_add_argument(fn_node*, char* name, int type);
char*            fn_node_inspect(fn_node*);

// TODO: hash_node

// id node
expression_node* id_node_evaluate(expression_node*, scope*);
char*            id_node_to_string(expression_node*);
char*            id_node_inspect(expression_node*);


// if node
if_node*         if_node_create(void);
void             if_node_destroy(if_node*);
expression_node* if_node_evaluate(if_node*, scope*);
if_node*         if_node_clone(if_node*);
char*            if_node_to_string(if_node*);
void             if_node_add_elseif(if_node*, expression_node*, expression_node*);
char*            if_node_inspect(if_node*);

// literal string node
char*            literal_string_node_inspect(expression_node*); // in ast/string.c

// loop node
loop_node*       loop_node_create(void);
void             loop_node_destroy(loop_node*);
expression_node* loop_node_evaluate(loop_node*, scope*);
loop_node*       loop_node_clone(loop_node*);
char*            loop_node_to_string(loop_node*);
char*            loop_node_inspect(loop_node*);

// nil
expression_node* nil_node_create(void);
char*            nil_node_to_string(expression_node*);
char*            nil_node_inspect(expression_node*);

// real
char*            real_node_to_string(expression_node*);
char*            real_node_inspect(expression_node*);

// string node
expression_node* string_node_evaluate(expression_node*, scope*);
char*            string_node_to_string(expression_node*);
char*            string_node_inspect(expression_node*);

// unary
unary_node*      unary_node_create(enum unary_op);
void             unary_node_destroy(unary_node*);
expression_node* unary_node_evaluate(unary_node*, scope*);
unary_node*      unary_node_clone(unary_node*);
char*            unary_node_to_string(unary_node*);
char*            unary_node_inspect(unary_node*);

#endif /* _AST_H_ */
