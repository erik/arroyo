#include <string.h>
#include <stdio.h>
#include "ast.h"
#include "util.h"

string_node* string_node_create (const char* string)
{
  string_node* node = calloc (sizeof (string_node), 1);
  node->string = strdup (string);
  return node;
}

void string_node_destroy (string_node* node)
{
  // FIXME: free shouldn't trigger double free
  // free (node->string);

  node->string = NULL;
  free (node);
}

string_node* string_node_to_string_node (string_node* node)
{
  return node;
}
