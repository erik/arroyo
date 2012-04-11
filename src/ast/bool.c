#include "ast.h"
#include "util.h"

inline int bool_node_value_of(expression_node* node)
{
  if(node->type == NODE_NIL ||
     (node->type == NODE_BOOL &&
      node->node.bool != 1))
    return 0;

  return 1;
}

expression_node* bool_node_from_expression(expression_node* node)
{
  return expression_node_create(NODE_BOOL, (ast_node)
                                {.bool = bool_node_value_of(node)});
}


char* bool_node_to_string(expression_node* bool)
{
  return strdup(bool->node.bool ? "true" : "false");
}

char* bool_node_inspect(expression_node* bool)
{
  return strdup(bool->node.bool ? "true" : "false");
}
