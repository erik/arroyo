#include "ast.h"
#include "util.h"

expression_node* nil_node_create()
{
  expression_node* node = malloc(sizeof(expression_node));

  node->type = NODE_NIL;
  node->ast_node = NULL;
  return node;
}

void nil_node_destroy(expression_node* nil)
{
  free(nil);
}

expression_node* nil_node_evaluate(void* nil, scope* scope)
{
  return nil_node_create();
}

expression_node* nil_node_clone(void* node)
{
  return nil_node_create();
}

string_node* nil_node_to_string_node(expression_node* nil)
{
  return string_node_create("nil");
}

char* nil_node_inspect(expression_node* nil)
{
  return strdup("nil");
}
