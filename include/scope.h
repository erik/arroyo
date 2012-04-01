#pragma once

#ifndef _SCOPE_H_
#define _SCOPE_H_


struct expression_node;

struct hashnode {
  unsigned hash;
  char* key;
  struct expression_node* value;

  // XXX: linked list is unacceptable for this
  struct hashnode* next;
} hashnode;

typedef struct scope {
  struct hashnode* root;

  struct scope* parent;
} scope;


scope* scope_create(scope* parent);
void scope_destroy(scope* scope);
void scope_insert(scope* scope, char* key, struct expression_node* value);
struct expression_node* scope_get(scope* scope, char* key);

#endif /* _SCOPE_H_ */
