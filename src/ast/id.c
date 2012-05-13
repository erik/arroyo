#include "ast.h"
#include "util.h"
#include <stdio.h>

char* id_node_to_string(expression_node* id)
{
  return strdup(id->node.string);
}

char* id_node_inspect(expression_node* id)
{
  return strdup(id->node.string);
}

expression_node* id_node_evaluate(expression_node* id, context* ctx)
{
  expression_node* value = scope_get(ctx->scope, id->node.string);

  if(!value)
    return nil_node_create();

  return expression_node_clone(value);
}
