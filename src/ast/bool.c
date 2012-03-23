#include "ast.h"

bool_node* bool_node_create (int val)
{
  bool_node* bool = calloc (sizeof (bool_node), 1);
  bool->bool = val;
  return bool;
}

void bool_node_destroy (bool_node* bool)
{
  free (bool);
}

string_node* bool_node_to_string_node (bool_node* bool)
{
  return string_node_create (bool->bool ? "true" : "false");
}
