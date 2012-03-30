#pragma once

#ifndef _PARSE_H_
#define _PARSE_H_

#include <setjmp.h>

#include "lex.h"
#include "ast.h"

struct lexer_state;

enum expression_type {
  EXP_REAL, EXP_STRING, EXP_BLOCK, EXP_ID, EXP_ASSIGN,

  LAST_EXP
};

typedef struct parser_state {
  lexer_state* ls;
  token t;
  token_info info;

  expression_node* ast_root;

  int die_on_error;   // die after a single error?

  struct {
    unsigned int max; // max number of errors before bailing out
    jmp_buf buf;      // jmp_buf to error handling / exit
    unsigned count;   // current number of errors
  } error;
} parser_state;

int parse(parser_state* ps);
expression_node* parse_expression(parser_state* ps);

#endif /* _PARSE_H_ */
