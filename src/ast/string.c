#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "ast.h"
#include "util.h"
#include "buffer.h"

string_node* string_node_create(const char* string)
{
  string_node* node = calloc(sizeof(string_node), 1);
  node->string = strdup(string);
  return node;
}

void string_node_destroy(string_node* node)
{
  free(node->string);
  free(node);
}

expression_node* string_node_evaluate(string_node* node, scope* scope)
{
  unsigned len = strlen(node->string);

  buffer b;
  buffer_create(&b, len);

  for(unsigned i = 0; i < len; ++i) {
    if(node->string[i] == '$') {
      if(node->string[i+1] == '$') {
        buffer_putc(&b, '$');
        i++;
      }
      else {
        unsigned begin = ++i;
        while(isalnum(node->string[i]) && ++i < len);

        buffer var;
        buffer_create(&var, i - begin);
        buffer_putsn(&var, (node->string+begin), i - begin);
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
      buffer_putc(&b, node->string[i]);
  }

  string_node* string = string_node_create(b.buf);

  buffer_destroy(&b);

  return expression_node_create(NODE_STRING, string);
}

expression_node* string_node_clone(string_node* node)
{
  return expression_node_create(
    NODE_STRING, string_node_create(node->string));
}

string_node* string_node_to_string_node(string_node* node)
{
  buffer b;
  buffer_create(&b, strlen(node->string) + 2);

  buffer_putc(&b, '"');
  buffer_puts(&b, node->string);
  buffer_putc(&b, '"');

  string_node* string = string_node_create(b.buf);

  buffer_destroy(&b);
  return string;
}
