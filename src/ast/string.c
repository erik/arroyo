#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "ast.h"
#include "util.h"
#include "buffer.h"


expression_node* string_node_evaluate(expression_node* node, scope* scope)
{
  const char* string = node->node.string;
  unsigned len = strlen(string);

  buffer b;
  buffer_create(&b, len);

  for(unsigned i = 0; i < len; ++i) {
    if(string[i] == '$') {
      if(string[i+1] == '$') {
        buffer_putc(&b, '$');
        i++;
      }
      else {
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
