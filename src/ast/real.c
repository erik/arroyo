#include "ast.h"
#include "util.h"
#include <stdio.h>

char* real_node_to_string(expression_node* node)
{
  char* string;
  if((long)node->node.real == node->node.real)
    asprintf(&string, "%ld", (long)node->node.real);
  else
    asprintf(&string, "%lf", node->node.real);
  return string;
}

char* real_node_inspect(expression_node* node)
{
  char* string;
  if((long)node->node.real == node->node.real)
    asprintf(&string, "%ld", (long)node->node.real);
  else
    asprintf(&string, "%lf", node->node.real);
  return string;
}
