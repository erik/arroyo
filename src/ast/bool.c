#include "ast.h"

bool_node* bool_node_create(int val)
{
  bool_node* bool = malloc(sizeof(bool_node));
  bool->bool = val;
  return bool;
}

bool_node* bool_node_from_expression(expression_node* expression)
{
  if(expression->type == NODE_NIL ||
     (expression->type == NODE_BOOL &&
      ((bool_node*)expression->ast_node)->bool != 1))
    return bool_node_create(0);

  return bool_node_create(1);
}

void bool_node_destroy(bool_node* bool)
{
  free(bool);
}

expression_node* bool_node_evaluate(bool_node* bool, scope* scope)
{
  return expression_node_create(NODE_BOOL, bool_node_create(bool->bool));
}

expression_node* bool_node_clone(bool_node* bool)
{
  return expression_node_create(NODE_BOOL, bool_node_create(bool->bool));
}

string_node* bool_node_to_string_node(bool_node* bool)
{
  return string_node_create(bool->bool ? "true" : "false");
}
