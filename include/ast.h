#pragma once

#ifndef _AST_H_
#define _AST_H_

#include "scope.h"

#include <stdlib.h>

typedef enum {
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

  NODE_BLOCK,

  MAX_NODE_TYPE
} node_type;

static const char* node_type_string[MAX_NODE_TYPE] = {
  "string", "real", "bool", "id", "nil",
  "function", "array", "hash", "binary", "unary",
  "loop", "_if", "block"
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
  OP_PRINT
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
  OP_ASSIGN
};

struct expression_node;

typedef struct {
  long double real;
} real_node;

typedef struct {
  unsigned size;
  char* string;
} string_node;

typedef struct {
  int bool;
} bool_node;

typedef struct {
  char* id;
} id_node;

typedef struct {
  char* id;
  struct typed_id* args;
  unsigned int nargs;
  struct expression_node* body;
} fn_node;

typedef struct {
  unsigned nelements;
  struct expression_node** elements;
} array_node;

typedef struct {
  // TODO
  int dummy;
} hash_node;

typedef struct {
  struct expression_node* lhs;
  struct expression_node* rhs;
  enum binary_op op;
} binary_node;

typedef struct {
  struct expression_node* expr;
  enum unary_op op;
} unary_node;

typedef struct {
  enum loop_type type;
  struct expression_node* init;
  struct expression_node* cond;
  struct expression_node* body;
} loop_node;

typedef struct {
  struct expression_node* condition;
  struct expression_node* thenbody;

  unsigned nelseif;
  struct expression_node** elseifcondition;
  struct expression_node** elseifbody;

  struct expression_node* elsebody;
} if_node;

typedef struct {
  struct expression_list {
    struct expression_node* expression;
    struct expression_list* next;
  } expression_list;

  struct expression_list* list, *last;

  scope* scope;
} block_node;

union ast_node;

typedef struct expression_node {
  node_type type;
  void* ast_node;
} expression_node;

// expression
expression_node* expression_node_create(node_type, void* node_struct);
void             expression_node_destroy(expression_node*);
expression_node* expression_node_evaluate(expression_node*, scope*);
expression_node* expression_node_clone(expression_node*);
string_node*     expression_node_to_string_node(expression_node*);
char*            expression_node_to_string(expression_node*);
char*            expression_node_inspect(expression_node*);

// nil
expression_node* nil_node_create(void);
void             nil_node_destroy(expression_node*);
expression_node* nil_node_evaluate(void*, scope*);
expression_node* nil_node_clone(void*);
string_node*     nil_node_to_string_node(expression_node*);
char*            nil_node_inspect(expression_node*);

// real
real_node*       real_node_create(long double);
void             real_node_destroy(real_node*);
expression_node* real_node_evaluate(real_node*, scope*);
expression_node* real_node_clone(real_node*);
string_node*     real_node_to_string_node(real_node*);
char*            real_node_inspect(real_node*);

// string node
string_node*     string_node_create(const char* string);
void             string_node_destroy(string_node*);
expression_node* string_node_evaluate(string_node*, scope*);
expression_node* string_node_clone(string_node*);
string_node*     string_node_to_string_node(string_node*);
char*            string_node_inspect(string_node*);

// bool node
bool_node*       bool_node_create(int);
void             bool_node_destroy(bool_node*);
expression_node* bool_node_evaluate(bool_node*, scope*);
expression_node* bool_node_clone(bool_node*);
string_node*     bool_node_to_string_node(bool_node*);
bool_node*       bool_node_from_expression(expression_node*);
char*            bool_node_inspect(bool_node*);

// id node
id_node*         id_node_create(char*);
void             id_node_destroy(id_node*);
expression_node* id_node_evaluate(id_node*, scope*);
expression_node* id_node_clone(id_node*);
string_node*     id_node_to_string_node(id_node*);
char*            id_node_inspect(id_node*);

// fn node
fn_node*         fn_node_create();
void             fn_node_destroy(fn_node*);
expression_node* fn_node_evaluate(fn_node*, scope*);
expression_node* fn_node_clone(fn_node*);
string_node*     fn_node_to_string_node(fn_node*);
void             fn_node_add_argument(fn_node*, char* name, int type);
char*            fn_node_inspect(fn_node*);

// array node
array_node*      array_node_create();
void             array_node_destroy(array_node*);
expression_node* array_node_evaluate(array_node*, scope*);
expression_node* array_node_clone(array_node*);
string_node*     array_node_to_string_node(array_node*);
void             array_node_push_expression(array_node*, expression_node*);
char*            array_node_inspect(array_node*);

// block node
block_node*      block_node_create(void);
void             block_node_destroy(block_node*);
expression_node* block_node_evaluate(block_node*, scope*);
expression_node* block_node_clone(block_node*);
string_node*     block_node_to_string_node(block_node*);
void             block_node_push_expression(block_node*, expression_node*);
char*            block_node_inspect(block_node*);

// if node
if_node*         if_node_create(void);
void             if_node_destroy(if_node*);
expression_node* if_node_evaluate(if_node*, scope*);
expression_node* if_node_clone(if_node*);
string_node*     if_node_to_string_node(if_node*);
void             if_node_add_elseif(if_node*, expression_node*, expression_node*);
char*            if_node_inspect(if_node*);

// loop node
loop_node*       loop_node_create(void);
void             loop_node_destroy(loop_node*);
expression_node* loop_node_evaluate(loop_node*, scope*);
expression_node* loop_node_clone(loop_node*);
string_node*     loop_node_to_string_node(loop_node*);
char*            loop_node_inspect(loop_node*);

// binary
binary_node*     binary_node_create(enum binary_op);
void             binary_node_destroy(binary_node*);
expression_node* binary_node_evaluate(binary_node*, scope*);
expression_node* binary_node_clone(binary_node*);
string_node*     binary_node_to_string_node(binary_node*);
char*            binary_node_inspect(binary_node*);

// unary
unary_node*      unary_node_create(enum unary_op);
void             unary_node_destroy(unary_node*);
expression_node* unary_node_evaluate(unary_node*, scope*);
expression_node* unary_node_clone(unary_node*);
string_node*     unary_node_to_string_node(unary_node*);
char*            unary_node_inspect(unary_node*);

// TODO: finish
// ... etc

#endif /* _AST_H_ */
