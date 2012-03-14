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

  jmp_buf err_buf;        // jmp_buf to error handling / exit
  int die_on_error;       // die after a single error?
  unsigned int error_max; // max number of errors before bailing out
  unsigned error_count;   // current number of errors
} parser_state;

void parse (parser_state* ps);

#endif /* _PARSE_H_ */
