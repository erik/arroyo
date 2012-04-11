#include "ast.h"
#include "util.h"

#include <stdio.h>
#include <string.h>

#define WHEN_NODE(node, body) case NODE_##node: { body; break; }
#define NODE_TYPE_FUNCTION(type, var, func) switch(type) {      \
    /* WHEN_NODE(HASH, var hash_node_##func);     */            \
    WHEN_NODE(ARRAY,   var array_node_##func);                  \
    WHEN_NODE(BINARY,  var binary_node_##func);                 \
    WHEN_NODE(BLOCK,   var block_node_##func);                  \
    WHEN_NODE(BOOL,    var bool_node_##func);                   \
    WHEN_NODE(CASE,    var case_node_##func);                   \
    WHEN_NODE(FN,      var fn_node_##func);                     \
    WHEN_NODE(ID,      var id_node_##func);                     \
    WHEN_NODE(IF,      var if_node_##func);                     \
    WHEN_NODE(LOOP,    var loop_node_##func);                   \
    WHEN_NODE(NIL,     var nil_node_##func);                    \
    WHEN_NODE(REAL,    var real_node_##func);                   \
    WHEN_NODE(STRING,  var string_node_##func);                 \
    WHEN_NODE(UNARY,   var unary_node_##func);                  \
  default:                                                      \
    printf(#func " hit default: %s\n", node_type_string[type]); \
  }


inline expression_node* expression_node_create(node_type type, void* node_struct)
{
  expression_node* node = malloc(sizeof(expression_node));
  node->type = type;
  node->ast_node = node_struct;
  return node;
}

inline void expression_node_destroy(expression_node* node)
{
  NODE_TYPE_FUNCTION(node->type, /* NONE */, destroy(node->ast_node));

  free(node);
  node = NULL;
}

inline expression_node* expression_node_evaluate(expression_node* node, scope* scope)
{
  expression_node* evaluated = NULL;
  NODE_TYPE_FUNCTION(node->type, evaluated=, evaluate(node->ast_node, scope));

  if(!evaluated)
    return nil_node_create();

  return evaluated;
}

inline expression_node* expression_node_clone(expression_node* node)
{
  expression_node* cloned = NULL;
  NODE_TYPE_FUNCTION(node->type, cloned=, clone(node->ast_node));

  if(!cloned)
    return nil_node_create();

  return cloned;
}

inline string_node* expression_node_to_string_node(expression_node* node)
{
  string_node* string = NULL;
  NODE_TYPE_FUNCTION(node->type, string=, to_string_node(node->ast_node));

  if(!string)
    return NULL;

  return string;
}

inline char* expression_node_to_string(expression_node* node)
{
  string_node* string = expression_node_to_string_node(node);

  if(!string)
    return NULL;

  char* str = strdup(string->string);
  string_node_destroy(string);

  return str;
}

inline char* expression_node_inspect(expression_node* node)
{
  char* string = NULL;
  NODE_TYPE_FUNCTION(node->type, string=, inspect(node->ast_node));

  if(!string)
    return expression_node_to_string(node);

  return string;
}

// 0 is false, 1 is true
int expression_node_equal(expression_node* left, expression_node* right)
{
#define CAST(expr, type) ((type)expr->ast_node)

  if(left->type != right->type)
    return 0;

  switch(left->type) {
  case NODE_REAL: {
    real_node* lr = CAST(left, real_node*);
    real_node* rr = CAST(right, real_node*);

    return lr->real == rr->real;
  }
  case NODE_BOOL: {
    bool_node* lb = CAST(left, bool_node*);
    bool_node* rb = CAST(right, bool_node*);

    return lb->bool == rb->bool;
  }
  case NODE_STRING: {
    string_node* ls = CAST(left, string_node*);
    string_node* rs = CAST(right, string_node*);

    return !strcmp(ls->string, rs->string);
  }
  default:
    printf("this comparision is not handled yet\n");
  }

  return 0;
#undef CAST
}
