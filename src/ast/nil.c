#include "ast.h"
#include "util.h"

expression_node* nil_node_create(void)
{
  expression_node* node = malloc(sizeof(expression_node));

  node->type = NODE_NIL;
  return node;
}


char* nil_node_to_string(expression_node* nil)
{
  return strdup("nil");
}

char* nil_node_inspect(expression_node* nil)
{
  return strdup("nil");
}
