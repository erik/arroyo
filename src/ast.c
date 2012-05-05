#include "ast.h"
#include "util.h"

#include <stdio.h>
#include <string.h>

inline expression_node* expression_node_create(node_type type, ast_node n)
{
  expression_node* node = malloc(sizeof(expression_node));
  node->type = type;
  node->node = n;
  return node;
}

inline void expression_node_destroy(expression_node* node)
{
  if(!node) return;

  switch (node->type) {
  case NODE_NIL:
  case NODE_REAL:
  case NODE_BOOL:
    // nothing to free
    break;

  case NODE_LITERAL_STRING:
  case NODE_STRING:
  case NODE_ID:
    free(node->node.string);
    break;

  case NODE_ARRAY:
    array_node_destroy(node->node.array);
    break;
  case NODE_BINARY:
    binary_node_destroy(node->node.binary);
    break;
  case NODE_BLOCK:
    block_node_destroy(node->node.block);
    break;
  case NODE_CASE:
    case_node_destroy(node->node.case_);
    break;
  case NODE_FN:
  case NODE_MACRO:
    fn_node_destroy(node->node.fn);
    break;
  case NODE_FOR:
    for_node_destroy(node->node.for_);
    break;
  case NODE_IF:
    if_node_destroy(node->node.if_);
    break;
  case NODE_LOOP:
    loop_node_destroy(node->node.loop);
    break;
  case NODE_UNARY:
    unary_node_destroy(node->node.unary);
    break;

  default:
    printf("BUG: destroy hit default for %s\n", node_type_string[node->type]);
  }

  free(node);
  node = NULL;
}

inline expression_node* expression_node_evaluate(expression_node* node, scope* scope)
{
  switch (node->type) {
  case NODE_NIL:
  case NODE_REAL:
  case NODE_BOOL: {
    // evaluate to self
    return expression_node_create(node->type, node->node);
  }

  case NODE_LITERAL_STRING: {
    return expression_node_create(NODE_LITERAL_STRING, (ast_node){.string = strdup(node->node.string)});
  }

  case NODE_STRING:
    return string_node_evaluate(node, scope);
  case NODE_ID:
    return id_node_evaluate(node, scope);
  case NODE_ARRAY:
    return array_node_evaluate(node->node.array, scope);
  case NODE_BINARY:
    return binary_node_evaluate(node->node.binary, scope);
  case NODE_BLOCK:
    return block_node_evaluate(node->node.block, scope);
  case NODE_CASE:
    return case_node_evaluate(node->node.case_, scope);
  case NODE_FN:
    return fn_node_evaluate(node->node.fn, scope);
  case NODE_FOR:
    return for_node_evaluate(node->node.for_, scope);
  case NODE_MACRO:
    return macro_node_evaluate(node->node.fn, scope);
  case NODE_IF:
    return if_node_evaluate(node->node.if_, scope);
  case NODE_LOOP:
    return loop_node_evaluate(node->node.loop, scope);
  case NODE_UNARY:
    return unary_node_evaluate(node->node.unary, scope);

  default:
    printf("BUG: evaluate hit default for %s\n", node_type_string[node->type]);
  }

  return nil_node_create();
}

inline expression_node* expression_node_clone(expression_node* node)
{
  expression_node* cloned = expression_node_create(node->type,
                                                   (ast_node) { .real = 0 });

  switch (node->type) {
  case NODE_NIL:
  case NODE_REAL:
  case NODE_BOOL: {
    // evaluate to self
    cloned->node = node->node;
    break;
  }
  case NODE_STRING:
  case NODE_ID:
  case NODE_LITERAL_STRING: {
    cloned->node.string = strdup(node->node.string);
    break;
  }
  case NODE_ARRAY:
    cloned->node.array = array_node_clone(node->node.array);
    break;
  case NODE_BINARY:
    cloned->node.binary = binary_node_clone(node->node.binary);
    break;
  case NODE_BLOCK:
    cloned->node.block = block_node_clone(node->node.block);
    break;
  case NODE_CASE:
    cloned->node.case_ = case_node_clone(node->node.case_);
    break;
  case NODE_FN:
  case NODE_MACRO:
    cloned->node.fn = fn_node_clone(node->node.fn);
    break;
  case NODE_IF:
    cloned->node.if_ = if_node_clone(node->node.if_);
    break;
  case NODE_LOOP:
    cloned->node.loop = loop_node_clone(node->node.loop);
    break;
  case NODE_FOR:
    cloned->node.for_ = for_node_clone(node->node.for_);
    break;
  case NODE_UNARY:
    cloned->node.unary = unary_node_clone(node->node.unary);
    break;

  default:
    printf("BUG: clone hit default for %s\n", node_type_string[node->type]);
  }

  return cloned;
}

inline expression_node* expression_node_call(expression_node* node,
                                             expression_node* arg, scope* s)
{
  switch(node->type) {
  case NODE_ARRAY:
    return array_node_call(node->node.array, arg, s);

  case NODE_FN:
    return fn_node_call(node->node.fn, arg, s);

  case NODE_MACRO:
    return macro_node_call(node->node.fn, arg, s);

  default:
    printf("call not supported for %s\n", node_type_string[node->type]);
  }

  return nil_node_create();
}

inline expression_node* expression_node_to_string_node(expression_node* node)
{
  char* string = expression_node_to_string(node);

  if(!string)
    return NULL;

  return expression_node_create(NODE_STRING, (ast_node){.string = string});
}

inline char* expression_node_to_string(expression_node* node)
{
  switch (node->type) {
  case NODE_NIL:
    return nil_node_to_string(node);
  case NODE_REAL:
    return real_node_to_string(node);
  case NODE_BOOL:
    return bool_node_to_string(node);

  case NODE_STRING:
  case NODE_ID:
  case NODE_LITERAL_STRING:
    return strdup(node->node.string);

  case NODE_ARRAY:
    return array_node_to_string(node->node.array);
  case NODE_BINARY:
    return binary_node_to_string(node->node.binary);
  case NODE_BLOCK:
    return block_node_to_string(node->node.block);
  case NODE_CASE:
    return case_node_to_string(node->node.case_);
  case NODE_FN:
    return fn_node_to_string(node->node.fn);
  case NODE_MACRO:
    return macro_node_to_string(node->node.fn);
  case NODE_IF:
    return if_node_to_string(node->node.if_);
  case NODE_LOOP:
    return loop_node_to_string(node->node.loop);
  case NODE_FOR:
    return for_node_to_string(node->node.for_);
  case NODE_UNARY:
    return unary_node_to_string(node->node.unary);

  default:
    printf("BUG: to_string hit default for %s\n", node_type_string[node->type]);
    return NULL;
  }
}

inline char* expression_node_inspect(expression_node* node)
{
  switch (node->type) {
  case NODE_NIL:
    return nil_node_inspect(node);
  case NODE_REAL:
    return real_node_inspect(node);
  case NODE_BOOL:
    return bool_node_inspect(node);
  case NODE_STRING:
    return string_node_inspect(node);
  case NODE_LITERAL_STRING:
    return literal_string_node_inspect(node);
  case NODE_ID:
    return id_node_inspect(node);
  case NODE_ARRAY:
    return array_node_inspect(node->node.array);
  case NODE_BINARY:
    return binary_node_inspect(node->node.binary);
  case NODE_BLOCK:
    return block_node_inspect(node->node.block);
  case NODE_CASE:
    return case_node_inspect(node->node.case_);
  case NODE_FN:
    return fn_node_inspect(node->node.fn);
  case NODE_MACRO:
    return macro_node_inspect(node->node.fn);
  case NODE_IF:
    return if_node_inspect(node->node.if_);
  case NODE_LOOP:
    return loop_node_inspect(node->node.loop);
  case NODE_FOR:
    return for_node_inspect(node->node.for_);
  case NODE_UNARY:
    return unary_node_inspect(node->node.unary);

  default:
    printf("BUG: inspect hit default for %s\n", node_type_string[node->type]);
    return NULL;
  }
}

// 0 is false, 1 is true
int expression_node_equal(expression_node* left, expression_node* right)
{
  if(left->type != right->type)
    return 0;

  switch(left->type) {
  case NODE_NIL:
    return 1;
  case NODE_REAL:
    return left->node.real == right->node.real;
  case NODE_BOOL:
    return left->node.bool == right->node.bool;
  case NODE_ID:
  case NODE_STRING:
    return !strcmp(left->node.string, right->node.string);
  case NODE_ARRAY: {
    if(left->node.array->nelements != right->node.array->nelements)
      return 0;
    for(unsigned i = 0; i < left->node.array->nelements; ++i) {
      if(!expression_node_equal(left->node.array->elements[i], right->node.array->elements[i]))
        return 0;
    }
    return 1;
  }
  default:
    printf("this comparision is not handled yet\n");
  }

  return 0;
}
