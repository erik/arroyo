#pragma once

#ifndef _LEX_H_
#define _LEX_H_

#include "buffer.h"
#include "reader.h"

#include <stdio.h>

// additional token values
typedef struct token_info {
  char* string;
  double number;
} token_info;

typedef struct token {
  int type;
  token_info info;
} token;

typedef struct lexer_state {
  buffer* buf;  // token buffer

  char current; // current character
  int linenum;  // current line
  int lastline; // last line (for when lookahead is used)

  reader* r;    // reader

  token next;   // lookahead token
  token t;      // current token
} lexer_state;

// jump out of range of char
#define FIRST_TOK 257

enum token_type {
  TK_EOS = FIRST_TOK, TK_ID, TK_REAL, TK_STRING,
  TK_IF, TK_WHEN, TK_FOR, TK_TO,
  TK_FN, TK_ASSIGN, TK_LTE, TK_GTE,
  TK_NEQ, TK_TRUE, TK_FALSE, TK_AND,
  TK_OR, TK_XOR,

  LAST_TOK
};

// numer of tokens
#define NUM_TOK (LAST_TOK - FIRST_TOK)


static const char *tokens[NUM_TOK] = {
  "<eos>", "<id>", "<real>", "<string>",
  "if", "when", "for", "to",
  "fn", "<-", "<=", ">=",
  "/=", "true", "false", "and",
  "or", "xor"
};

// simplistic token to string handling
char* tok_to_string (int tok);

token lexer_next_token (lexer_state* ls);
token lexer_lookahead  (lexer_state* ls);

void lexer_create      (lexer_state* ls, reader* r);
void lexer_destroy     (lexer_state* ls);

#endif /* _LEX_H_ */
