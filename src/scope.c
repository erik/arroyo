#include "scope.h"
#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* implementation of Jenkins' one-at-a-time hash function
 * https://en.wikipedia.org/wiki/Jenkins_hash_function#one-at-a-time
 */
static unsigned hash (char* key, unsigned len)
{
  unsigned hash, i;

  for(hash = i = 0; i < len; ++i) {
    hash += key[i];
    hash += hash << 10;
    hash ^= hash >> 6;
  }

  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;

  return hash;
}

static void hashnode_destroy(struct hashnode* node)
{
  if(!node) return;

  free(node->key);
  expression_node_destroy(node->value);
}

scope* scope_create(scope* parent)
{
  scope* s = calloc(sizeof(scope), 1);

  s->root = NULL;

  if(parent)
    s->parent = parent;

  return s;
}

void scope_destroy(scope* scope)
{
  struct hashnode* ptr = scope->root;

  while(ptr) {
    struct hashnode* next = ptr->next;
    hashnode_destroy(ptr);
    free(ptr);
    ptr = next;
  }

  free(scope);
}

void scope_insert(scope* scope, char* key, expression_node* value)
{
  unsigned hashed = hash(key, strlen(key));

  // XXX: inserting at the front creates duplicates each time
  // something is assigned, and is terrible form
  struct hashnode* node = calloc(sizeof(struct hashnode), 1);
  node->hash = hashed;
  node->value = value;
  node->key = key;
  node->next = scope->root;

  scope->root = node;
}

expression_node* scope_get(scope* scope, char* key)
{
  if(scope->root == NULL) {
    if(scope->parent)
      return scope_get(scope->parent, key);
    else
      return NULL;

  }
  unsigned hashed = hash(key, strlen(key));
  struct hashnode* node = scope->root;

  while(node) {
    if(node->hash == hashed)
      return node->value;

    node = node->next;
  }

  // not found in current scope, check parent
  if(scope->parent != NULL) {
    return scope_get(scope->parent, key);
  }

  return NULL;
}
