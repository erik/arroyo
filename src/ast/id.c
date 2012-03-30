#include "ast.h"
#include "util.h"

id_node* id_node_create(char* name)
{
  id_node* node = calloc(sizeof(id_node), 1);

  node->id = strdup(name);
  node->value = NULL;

  return node;
}

void id_node_destroy(id_node* node)
{
  free(node->id);
  if(node->value)
    expression_node_destroy(node->value);
  free(node);
}

string_node* id_node_to_string_node(id_node* id)
{
  return string_node_create(id->id);;
}

expression_node* id_node_evaluate(id_node* id)
{
  id_node* clone = id_node_create(id->id);
  // FIXME: weak clone, bound to explode
  clone->value = id->value;
  return expression_node_create(NODE_ID, clone);
}

