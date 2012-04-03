#include "ast.h"
#include "util.h"
#include <stdio.h>
real_node* real_node_create(long double value)
{
  real_node* node = malloc(sizeof(real_node));
  node->real = value;
  return node;
}

string_node* real_node_to_string_node(real_node* node)
{
  char* string;
  asprintf(&string, "%Lf", node->real);

  string_node* str = string_node_create(string);

  // string_node_create strdups the value for us
  free(string);
  return str;
}

expression_node* real_node_evaluate(real_node* real, scope* scope)
{
  return real_node_clone(real);
}

expression_node* real_node_clone(real_node* node)
{
  return expression_node_create(NODE_REAL, real_node_create(node->real));
}

void real_node_destroy(real_node* node)
{
  free(node);
}
