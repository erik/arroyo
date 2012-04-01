#include "lex.h"
#include "reader.h"
#include "ast.h"
#include "parse.h"
#include "scope.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <getopt.h>

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

int run_repl(scope* scope)
{
  reader r;
  lexer_state* ls = calloc(sizeof(lexer_state), 1);
  parser_state* ps = calloc(sizeof(parser_state), 1);

  reader_create(&r, readline_reader, NULL);

  lexer_create(ls, &r);

  ps->ls = ls;
  ps->die_on_error = 0;
  ps->error.max = 20;
  ps->t = lexer_next_token(ps->ls);

  switch(setjmp(ps->error.buf)) {
  case 1: // an error occurred, non fatal, so jump back into loop
    ps->t = lexer_next_token(ps->ls);
  case 0: // no error
    while(ps->t.type != TK_EOS) {
      expression_node* node = parse_expression(ps);
      expression_node* eval = expression_node_evaluate(node, scope);
      char* str = expression_node_to_string(eval);

      printf("==> %s\n", str);

      free(str);
      expression_node_destroy(eval);
      expression_node_destroy(node);
    }
    break;
  case 2: // EOS, break out
    break;
  }

  lexer_destroy(ls);
  free(ls);
  free(ps);

  return 0;
}

int run_file(const char* filename, scope* scope)
{
  struct file_struct file;
  reader r;

  lexer_state* ls = calloc(sizeof(lexer_state), 1);
  parser_state* ps = calloc(sizeof(parser_state), 1);

  FILE* fp = fopen(filename, "r");

  if(!fp) {
    printf("Error: can't open file '%s'\n", filename);
    return 1;
  }

  file = (struct file_struct) {
    .fp = fp,
    .buf = NULL,
    .done = 0
  };

  reader_create(&r, file_reader, &file);
  lexer_create(ls, &r);

  ps->ls = ls;
  ps->die_on_error = 0;
  ps->error.max = 20;
  ps->t = lexer_next_token(ps->ls);

  expression_node* program = parse_program(ps);
  expression_node* eval = expression_node_evaluate(program, scope);

  // print the last expression
  char* str = expression_node_to_string(eval);
  puts(str);
  free(str);

  expression_node_destroy(eval);
  expression_node_destroy(program);

  lexer_destroy(ls);
  free(ls);
  free(ps);

  fclose(fp);

  return 0;
}

int usage(void)
{
  fprintf(stderr,
          "Usage: arroyo [switches] [file]\n"           \
          "  -r, --repl\tStart a REPL\n"                \
          "  -h, --help\tPrint this help text\n"
    );
  return 1;
}

int main(int argc, char** argv)
{
  char* filename = NULL;
  int repl_flag=0, help_flag=0;

  struct option long_opts[] = {
    {"help", no_argument, &help_flag, 1},
    {"repl", no_argument, &repl_flag, 1},
    {0,      0,           0,          0}
  };

  int cont = 1;
  while(cont) {
    int opt_index = 0;
    switch(getopt_long(argc, argv, "hr", long_opts, &opt_index)) {
    case -1:
      cont = 0;
      break;

    case 'r':
      repl_flag = 1;
      break;

    case '?':
    case 'h':
      return usage();
    }
  }

  if(optind < argc)
    filename = argv[optind++];

  if((!filename && !repl_flag) || help_flag)
    return usage();

  scope* scope = scope_create(NULL);

  if(filename)
    run_file(filename, scope);

  if(repl_flag)
    run_repl(scope);

  scope_destroy(scope);
  return 0;
}
