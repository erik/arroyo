#pragma once

#ifndef _AST_H_
#define _AST_H_

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
} node_type;

struct typed_id {
  const char* id;
  const char* type;
};

enum loop_type { LOOP_WHILE, LOOP_UNTIL, LOOP_DO };

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
  const char* id;
} id_node;

typedef struct {
  const char* id;
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
  int binary_op;
} binary_node;

typedef struct {
  struct expression_node* rhs;
  int unary_op;
} unary_node;

typedef struct {
  enum loop_type type;
  struct expression_node* init;
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
  unsigned nexpressions;
  struct expression_node** expressions;
} block_node;

union ast_node;

typedef struct expression_node {
  node_type type;
  void* ast_node;
} expression_node;

// expression
expression_node* expression_node_create (node_type, void* node_struct);
void             expression_node_destroy (expression_node*);
expression_node* expression_node_evaluate (expression_node*);
string_node*     expression_node_to_string_node (expression_node*);
char*            expression_node_to_string (expression_node*);

// nil
expression_node* nil_node_create (void);
void             nil_node_destroy (expression_node*);
string_node*     nil_node_to_string_node (expression_node*);

// real
real_node*       real_node_create (long double);
void             real_node_destroy (real_node*);
string_node*     real_node_to_string_node (real_node*);

// string node
string_node*     string_node_create (const char* string);
void             string_node_destroy (string_node*);
string_node*     string_node_to_string_node (string_node*);

// bool node
bool_node*       bool_node_create (int);
void             bool_node_destroy (bool_node*);
string_node*     bool_node_to_string_node (bool_node*);

// id node
id_node*         id_node_create (const char*);
void             id_node_destroy (id_node*);
expression_node* id_node_evaluate (id_node*);
string_node*     id_node_to_string_node (id_node*);

// fn node
fn_node*         fn_node_create (int);
void             fn_node_destroy (fn_node*);
expression_node* fn_node_evaluate (fn_node*);
string_node*     fn_node_to_string_node (fn_node*);

// array node
array_node*      array_node_create ();
void             array_node_destroy (array_node*);
expression_node* array_node_evaluate (array_node*);
string_node*     array_node_to_string_node (array_node*);
void             array_node_push_expression (array_node*, expression_node*);

// block node
block_node*      block_node_create (void);
void             block_node_destroy (block_node*);
expression_node* block_node_evaluate (block_node*);
string_node*     block_node_to_string_node (block_node*);
void             block_node_push_expression (block_node*, expression_node*);

// if node
if_node*         if_node_create (void);
void             if_node_destroy (if_node*);
expression_node* if_node_evaluate (if_node*);
string_node*     if_node_to_string_node (if_node*);
void             if_node_set_condition (if_node*, expression_node*, expression_node*);
void             if_node_add_elseif (if_node*, expression_node*, expression_node*);
void             if_node_set_else (if_node*, expression_node*);

// TODO: finish
// ... etc

#endif /* _AST_H_ */
