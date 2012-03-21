#pragma once

#ifndef _PARSE_H_
#define _PARSE_H_

#include <setjmp.h>

#include "lex.h"

struct lexer_state;

enum expression_type {
  EXP_REAL, EXP_STRING, EXP_BLOCK, EXP_ID, EXP_ASSIGN,

  LAST_EXP
};

enum unary_op {
  // must be first
  OP_NOTUNOP = 0,

  OP_UNM, OP_NOT
};

enum binary_op {
  // must be first
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

typedef struct expression_node {
  enum expression_type type;

  union {
    struct {
      double real;
    } real_node;

    struct {
      const char* string;
    } string_node;

    struct {
      unsigned num_expr;
      struct expression_node* expressions;
    } block_node;

    struct {
      const char* id;
    } id_node;

    struct {
      struct id_node* id;
      struct expression_node* value;
    } assign_node;
  } node;

} expression_node;

typedef struct parser_state {
  lexer_state* ls;
  token t;

  int die_on_error;   // die after a single error?

  struct {
    unsigned int max; // max number of errors before bailing out
    jmp_buf buf;      // jmp_buf to error handling / exit
    unsigned count;   // current number of errors
  } error;
} parser_state;

int parse (parser_state* ps);

#endif /* _PARSE_H_ */
