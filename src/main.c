#include "lex.h"
#include "reader.h"
#include "parse.h"
#include "ast.h"
#include "scope.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

const char* readline_reader(void* dummy, unsigned* size)
{
  (void)dummy;

  const char* input = readline(">> ");

  if(input == NULL) {
    *size = 0;
    return NULL;
  }

  if((strcmp(input, "exit") == 0) || (strcmp(input, "quit") == 0)) {
    exit(0);
  }

  add_history(input);

  static char* ret = NULL;

  if(ret != NULL) free(ret);

  ret = malloc(strlen(input) + 3);
  strcpy(ret, input);
  strcat(ret, ",\n");

  *size = strlen(ret);
  return ret;
}

static struct file_struct {
  FILE* fp;
  char* buf;
  int done;
} file_struct;

const char* file_reader(void* f, unsigned* size)
{
  struct file_struct* file = f;

  if(file->buf)
    free(file->buf);

  if(file->done) {
    *size = 0;
    return NULL;
  }

  file->buf = calloc(4096, 1);

  *size = fread(file->buf, 1, 4096, file->fp);

  if(*size < 4096)
    file->done = 1;

  return file->buf;
}

int main(int argc, char** argv)
{
  int dorepl = 0;
  char* filename = NULL;

  for(int i = 1; i < argc; ++i) {
    if(!strcmp(argv[i], "--repl"))
      dorepl = 1;
    else
      filename = argv[i];
  }

  reader r;
  lexer_state* ls = calloc(sizeof(lexer_state), 1);
  parser_state* ps = calloc(sizeof(parser_state), 1);

  scope* scope = scope_create(NULL);

  struct file_struct file;

  if(filename != NULL) {
    FILE* fp = fopen(filename, "r");

    if(!fp) {
      printf("can't open file '%s'\n", filename);
      return 1;
    }

    file = (struct file_struct) {
      .fp = fp,
      .buf = NULL,
      .done = 0
    };

    reader_create(&r, file_reader, &file);
  } else if(dorepl) {
    reader_create(&r, readline_reader, NULL);
  } else {
    printf("need to give me *something* to do!\n");
    return 1;
  }

  lexer_create(ls, &r);

  ps->ls = ls;
  ps->die_on_error = 0;
  ps->error.max = 20;
  ps->t = lexer_next_token(ps->ls);

  if(dorepl) {
    switch(setjmp(ps->error.buf)) {
    case 1: // an error occurred, non fatal, so jump back into loop
      ps->t = lexer_next_token(ps->ls);
    case 0: // no error
      while(ps->t.type != TK_EOS) {
        expression_node* node = parse_expression(ps);
        expression_node* eval = expression_node_evaluate(node, scope);
        string_node* str = expression_node_to_string_node(eval);

        printf("==> %s\n", str->string);

        string_node_destroy(str);
        expression_node_destroy(eval);
        expression_node_destroy(node);
      }
      break;
    case 2: // EOS, break out
      break;
    }
  }
  else {
    expression_node* program = parse_program(ps);
    expression_node* eval = expression_node_evaluate(program, scope);

    puts(expression_node_to_string(eval));

    expression_node_destroy(program);
  }

  scope_destroy(scope);
  lexer_destroy(ls);
  free(ls);
  free(ps);

  return 0;
}
