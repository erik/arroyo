#include "scope.h"
#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Random shuffling of 0 to 255, order has no real significance, just
// improves distribution of hashed values
static const unsigned char mutator[256] = {
  181, 98, 100, 131, 225, 121, 56, 136, 237, 185, 21, 40, 240, 189, 22,
  210, 132, 6, 71, 45, 183, 206, 239, 102, 66, 174, 227, 135, 212, 51,
  243, 97, 232, 229, 122, 128, 9, 43, 208, 141, 7, 201, 50, 2, 107, 108,
  249, 106, 151, 188, 214, 65, 153, 247, 154, 138, 87, 63, 85, 94, 55,
  26, 61, 221, 24, 224, 60, 180, 74, 117, 187, 220, 79, 112, 162, 173,
  147, 223, 8, 205, 222, 23, 0, 69, 217, 244, 30, 37, 109, 81, 252, 46,
  197, 84, 127, 120, 29, 125, 15, 176, 242, 177, 14, 241, 77, 199, 235,
  80, 226, 58, 10, 159, 250, 195, 53, 49, 124, 231, 118, 234, 175, 218,
  1, 76, 137, 25, 39, 54, 75, 93, 190, 209, 172, 95, 207, 148, 178, 171,
  88, 196, 248, 168, 105, 5, 47, 113, 139, 228, 62, 192, 200, 42, 28,
  219, 52, 34, 31, 156, 13, 161, 246, 213, 144, 215, 59, 238, 211, 204,
  253, 64, 111, 16, 119, 167, 202, 91, 203, 149, 20, 89, 254, 44, 184,
  143, 41, 166, 99, 57, 18, 103, 68, 170, 110, 191, 186, 230, 17, 133,
  101, 157, 4, 35, 116, 33, 70, 27, 236, 82, 155, 245, 193, 179, 164,
  72, 140, 115, 123, 158, 251, 104, 145, 83, 32, 90, 78, 129, 134, 194,
  3, 255, 38, 233, 96, 152, 130, 11, 126, 198, 36, 146, 142, 150, 182,
  216, 86, 73, 67, 48, 160, 92, 19, 12, 165, 114, 169, 163
};

// Simple Pearson hashing
static inline unsigned char hash(const char* key)
{
  unsigned char hashed = 0;

  unsigned char c;
  while((c = *key++)) {
    hashed ^= mutator[c^hashed];
  }

  return hashed;
}

scope* scope_create(scope* parent)
{
  scope* s = calloc(sizeof(scope), 1);
  s->parent = parent;
  return s;
}

void scope_destroy(scope* scope)
{
  for(unsigned i = 0; i < 256; ++i) {
    if(!scope->buckets[i])
      continue;

    bucket* ptr = scope->buckets[i];

    while(ptr) {
      bucket* next = ptr->next;

      expression_node_destroy(ptr->value);
      free(ptr->key);
      free(ptr);

      ptr = next;
    }
  }

  free(scope);
}

void scope_insert(scope* scope, char* key, expression_node* value)
{
  unsigned char hashed = hash(key);

  bucket* b = scope->buckets[hashed];

  // if bucket doesn't exist, create it
  if(!b) {
    b = malloc(sizeof(bucket));
    b->next = NULL;
    b->key = key;
    b->value = value;
    scope->buckets[hashed] = b;
    return;
  }

  // otherwise insert at head of list
  bucket* head = malloc(sizeof(bucket));
  head->key = key;
  head->value = value;
  head->next = b;

  scope->buckets[hashed] = head;
}

// helper function to prevent key from being hashed multiple times
static bucket* scope_get_bucket_(scope* scope, char* key, unsigned char hashed)
{
  bucket* b = scope->buckets[hashed];

  if(!b)
    return scope->parent ? scope_get_bucket_(scope->parent, key, hashed) : NULL;

  while(b) {
    if(!strcmp(b->key, key)) {
      return b;
    }
    b = b->next;
  }
  return NULL;
}

bucket* scope_get_bucket(scope* scope, char* key)
{
  unsigned char hashed = hash(key);

  return scope_get_bucket_(scope, key, hashed);
}

expression_node* scope_get(scope* scope, char* key)
{
  bucket* bucket = scope_get_bucket(scope, key);

  if(bucket)
    return bucket->value;

  return NULL;
}
