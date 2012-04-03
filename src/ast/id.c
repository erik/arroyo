#include "ast.h"
#include "util.h"
#include <stdio.h>

id_node* id_node_create(char* name)
{
  id_node* node = malloc(sizeof(id_node));
  node->id = strdup(name);
  return node;
}

void id_node_destroy(id_node* node)
{
  free(node->id);
  free(node);
}

string_node* id_node_to_string_node(id_node* id)
{
  return string_node_create(id->id);
}

expression_node* id_node_evaluate(id_node* id, scope* scope)
{
  expression_node* value = scope_get(scope, id->id);

  if(!value)
    return nil_node_create();

  return expression_node_clone(value);
}

expression_node* id_node_clone(id_node* id)
{
  return expression_node_create(NODE_ID,
                                id_node_create(id->id));
}
