#pragma once

#ifndef _SCOPE_H_
#define _SCOPE_H_

struct expression_node;

typedef struct bucket {
  char* key;
  struct expression_node* value;

  struct bucket* next;
} bucket;

typedef struct scope {
  struct bucket* buckets[256];

  struct scope* parent;
} scope;


scope* scope_create(scope* parent);
void scope_destroy(scope* scope);
void scope_insert(scope* scope, char* key, struct expression_node* value);
bucket* scope_get_bucket(scope* s, char* key);
struct expression_node* scope_get(scope* scope, char* key);

#endif /* _SCOPE_H_ */
