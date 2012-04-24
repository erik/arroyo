#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "ast.h"
#include "util.h"
#include "buffer.h"
#include "lex.h"
#include "parse.h"
#include "reader.h"

expression_node* string_node_evaluate(expression_node* node, scope* scope)
{
  const char* string = node->node.string;
  unsigned len = strlen(string);

  buffer b;
  buffer_create(&b, len);

  for(unsigned i = 0; i < len; ++i) {
    if(string[i] == '$') {
      if(string[i+1] == '$') { // literal $
        buffer_putc(&b, '$');
        i++;
      }
      else if(string[i+1] == '{') { // string interpolation
        unsigned begin = (i += 2);

        // XXX: doing simple parsing by counting braces here.
        // this means that other uses of '}' (in strings, for example)
        // will erroneously trigger the end of whole expression
        int brace_level = 0;

        for(char c = 0; (c = string[i]) && i < len && brace_level >= 0; ++i) {
          if(c == '{')
            brace_level++;
          if(c == '}')
            brace_level--;
        }

        i -= 1;

        // do nothing for zero length strings
        if(i - begin <= 0)
          break;

        char* src = calloc(i - begin + 1, 1);
        memcpy(src, string + begin, i - begin);

        lexer_state* ls = calloc(sizeof(lexer_state), 1);
        parser_state* ps = calloc(sizeof(parser_state), 1);
        reader r;
        string_reader_create(&r, src);
        lexer_create(ls, &r);

        ps->ls = ls;
        ps->die_on_error = 1;
        ps->error.max = 1;
        ps->t = lexer_next_token(ps->ls);

        expression_node* program = parse_program(ps);
        expression_node* eval = expression_node_evaluate(program, scope);

        char* tmp = expression_node_to_string(eval);
        buffer_puts(&b, tmp);
        free(tmp);

        expression_node_destroy(eval);
        expression_node_destroy(program);

        lexer_destroy(ls);
        free(ls);
        free(ps);
        free(r.fn_data);
        free(src);
      }
      else { // variable substitution
        unsigned begin = ++i;
        while(isalnum(string[i]) && ++i < len);

        buffer var;
        buffer_create(&var, i - begin);
        buffer_putsn(&var, (string+begin), i - begin);
        buffer_putc(&var, '\0');

        expression_node* expr = scope_get(scope, var.buf);
        buffer_destroy(&var);

        if(expr) {
          char* str = expression_node_to_string(expr);
          buffer_puts(&b, str);
          if(i != len)
            buffer_putc(&b, ' ');

          free(str);
        }
      }
    }
    else
      buffer_putc(&b, string[i]);
  }

  return expression_node_create(NODE_STRING, (ast_node){.string = b.buf });
}


char* string_node_to_string(expression_node* node)
{
  return strdup(node->node.string);
}

char* string_node_inspect(expression_node* node)
{
  buffer b;
  buffer_create(&b, strlen(node->node.string) + 2);
  buffer_putc(&b, '"');
  buffer_puts(&b, node->node.string);
  buffer_putc(&b, '"');

  return b.buf;
}

char* literal_string_node_inspect(expression_node* node)
{
  char* tmp = string_node_inspect(node);

  buffer b;
  buffer_create(&b, strlen(node->node.string) + 2);

  buffer_puts(&b, "\"\"");
  buffer_puts(&b, tmp);
  buffer_puts(&b, "\"\"");

  free(tmp);

  return b.buf;
}

