#include "ast.h"

// TODO: placeholder methods for hash nodes, need to be filled in

hash_node* hash_node_create(void)
{
  return NULL;
}

void hash_node_destroy(hash_node* node)
{

}

expression_node* hash_node_evaluate(hash_node* node, context* ctx)
{
  return NULL;
}

hash_node* hash_node_clone(hash_node* hash)
{
  return NULL;
}

expression_node* hash_node_call(hash_node* node, expression_node* arg, context* ctx)
{
  return NULL;
}

char* hash_node_to_string(hash_node* node)
{
  return NULL;
}

char* hash_node_inspect(hash_node* node)
{
  return NULL;
}

void hash_node_insert(hash_node* node, char* key, expression_node* value)
{

}
