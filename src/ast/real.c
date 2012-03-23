#include "ast.h"
#include "util.h"
#include <stdio.h>
real_node* real_node_create (long double value)
{
  real_node* node = calloc (sizeof (real_node), 1);
  node->real = value;
  return node;
}

string_node* real_node_to_string_node (real_node* node)
{
  char* string;
  asprintf (&string, "%Lf", node->real);

  string_node* str = string_node_create (string);

  // string_node_create strdups the value for us
  free (string);

  return str;
}

void real_node_destroy (real_node* node)
{
  free (node);
}
