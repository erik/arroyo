#include "lex.h"
#include "reader.h"
#include "ast.h"
#include "parse.h"
#include "scope.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <getopt.h>

int run_repl(scope* scope)
{
  parser_state* ps = calloc(sizeof(parser_state), 1);
  ps->die_on_error = 0;
  ps->error.max = 20;

  for(unsigned line = 1; ; ++line) {
    char* input = readline(">> ");

    if(input == NULL)
      break;

    add_history(input);

    char* ret = malloc(strlen(input) + 2);
    strcpy(ret, input);
    strcat(ret, "\n");
    free(input);

    reader r;
    string_reader_create(&r, ret);

    lexer_state* ls = calloc(sizeof(lexer_state), 1);
    lexer_create(ls, &r);

    ps->ls = ls;
    ps->ls->linenum = line;
    ps->t = lexer_next_token(ps->ls);

    expression_node* program = parse_program(ps);

    if(program && program->node.block) {

      for(struct expression_list* expr = program->node.block->list; expr; expr = expr->next) {

        expression_node* eval = expression_node_evaluate(expr->expression, scope);
        char* str = expression_node_inspect(eval);

        printf("==> %s\n", str);

        free(str);
        expression_node_destroy(eval);
      }

      expression_node_destroy(program);

      free(r.fn_data);
      lexer_destroy(ls);
      free(ls);

      free(ret);
    }
  }

  free(ps);
  return 0;
}

int run_file(const char* filename, scope* scope)
{
  reader r;

  lexer_state* ls = calloc(sizeof(lexer_state), 1);
  parser_state* ps = calloc(sizeof(parser_state), 1);

  FILE* fp = fopen(filename, "r");

  if(!fp) {
    printf("Error: can't open file '%s'\n", filename);
    return 1;
  }

  file_reader_create(&r, fp);
  lexer_create(ls, &r);

  ps->ls = ls;
  ps->die_on_error = 0;
  ps->error.max = 20;
  ps->t = lexer_next_token(ps->ls);

  expression_node* program = parse_program(ps);
  expression_node* eval = expression_node_evaluate(program, scope);
  expression_node_destroy(eval);
  expression_node_destroy(program);

  lexer_destroy(ls);
  free(ls);
  free(ps);

  free(r.fn_data);

  fclose(fp);

  return 0;
}

int usage(void)
{
  fprintf(stderr,
          "Usage: arroyo [switches] [file]\n"
          "  -r, --repl\tStart a REPL (after file is evaluated, if given)\n"
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
