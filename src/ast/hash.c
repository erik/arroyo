#include "ast.h"
#include "buffer.h"
#include "util.h"

#include <stdio.h>

// TODO: placeholder methods for hash nodes, need to be filled in

hash_node* hash_node_create(void)
{
  hash_node* hash = malloc(sizeof(hash_node));
  hash->hash = scope_create(NULL);
  return hash;
}

void hash_node_destroy(hash_node* hash)
{
  scope_destroy(hash->hash);
  free(hash);
}

expression_node* hash_node_evaluate(hash_node* hash, context* ctx)
{
  return expression_node_create(NODE_HASH,
                                (ast_node){.hash = hash_node_clone(hash)});
}

hash_node* hash_node_clone(hash_node* hash)
{
  hash_node* new = hash_node_create();

  for(unsigned i = 0; i < 256; ++i) {
    for(bucket* ptr = hash->hash->buckets[i]; ptr; ) {
      scope_insert(new->hash, strdup(ptr->key), expression_node_clone(ptr->value));
      ptr = ptr->next;
    }
  }

  return new;
}

expression_node* hash_node_call(hash_node* hash, expression_node* arg, context* ctx)
{
  // TODO
  return NULL;
}

char* hash_node_to_string(hash_node* hash)
{
  buffer b;
  buffer_create(&b, 10);

  buffer_putc(&b, '{');

  for(unsigned i = 0; i < 256; ++i) {
    for(bucket* ptr = hash->hash->buckets[i]; ptr; ) {
      buffer_puts(&b, ptr->key);
      buffer_putc(&b, ':');

      char* tmp = expression_node_to_string(ptr->value);
      buffer_puts(&b, tmp);
      free(tmp);

      buffer_puts(&b, ", ");

      ptr = ptr->next;
    }
  }

  buffer_putc(&b, '}');

  return b.buf;
}

char* hash_node_inspect(hash_node* hash)
{
  buffer b;
  buffer_create(&b, 10);

  buffer_putc(&b, '{');

  for(unsigned i = 0; i < 256; ++i) {
    for(bucket* ptr = hash->hash->buckets[i]; ptr; ) {

      buffer_puts(&b, ptr->key);
      buffer_putc(&b, ':');

      char* tmp = expression_node_inspect(ptr->value);
      buffer_puts(&b, tmp);
      free(tmp);

      buffer_puts(&b, ", ");

      ptr = ptr->next;
    }
  }

  buffer_putc(&b, '}');

  return b.buf;
}

void hash_node_insert(hash_node* hash, char* key, expression_node* value)
{
  // TODO: check for collisions
  scope_insert(hash->hash, key, value);
}
