#pragma once

#ifndef _LEX_H_
#define _LEX_H_

#include "buffer.h"
#include "reader.h"

#include <setjmp.h>
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
  int lastline; // last line(for when lookahead is used)

  reader* r;    // reader

  token next;   // lookahead token
  token t;      // current token

  struct {
    jmp_buf buf;
  } error;

} lexer_state;

// jump out of range of char
#define FIRST_TOK 257

enum token_type {
  TK_ERROR = FIRST_TOK, TK_EOS,

  TK_ID, TK_REAL, TK_STRING, TK_LIT_STRING,
  TK_IF, TK_ELSEIF, TK_ELSE,
  TK_FN, TK_MACRO,
  TK_ASSIGN, TK_CALL,
  TK_LTE, TK_GTE, TK_NEQ,
  TK_TRUE, TK_FALSE, TK_NIL,
  TK_AND, TK_OR, TK_XOR,
  TK_NOT,
  TK_LOOP, TK_WHILE, TK_UNTIL, TK_DO,
  TK_FOR, TK_IN,
  TK_INC,
  TK_CONCAT,
  TK_CASE, TK_OF, TK_RARROW, TK_DEFAULT,
  TK_SHIFT,
  // temporary
  TK_PRINT, TK_EVAL, TK_REQUIRE,

  LAST_TOK
};

// numer of tokens
#define NUM_TOK (LAST_TOK - FIRST_TOK)

static const char* tokens[NUM_TOK] = {
  "<error>", "<eos>",
  "<id>", "<real>", "<string>", "<literal string>",
  "if", "elseif", "else",
  "fn", "macro",
  "<-", "->",
  "<=", ">=", "/=",
  "true", "false", "nil",
  "and", "or", "xor",
  "not",
  "loop", "while", "until", "do",
  "for", "in",
  "++",
  "..",
  "case", "of", "=>", "default",
  "<<",
  "print", "eval", "require"
};

// simplistic token to string handling
char* tok_to_string(int tok);

token lexer_next_token(lexer_state* ls);
token lexer_lookahead  (lexer_state* ls);

void lexer_create      (lexer_state* ls, reader* r);
void lexer_destroy     (lexer_state* ls);

#endif /* _LEX_H_ */
