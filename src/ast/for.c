#include <stdio.h>

#include "ast.h"
#include "util.h"
#include "buffer.h"
#include "scope.h"

for_node* for_node_create(void)
{
  for_node* node = malloc(sizeof(for_node));

  node->num_ids = 0;
  node->ids = NULL;
  node->in_expr = NULL;
  node->body = NULL;

  return node;
}

void for_node_destroy(for_node* node)
{
  for(unsigned i = 0; i < node->num_ids; ++i) {
    free(node->ids[i]);
  }
  free(node->ids);

  expression_node_destroy(node->in_expr);
  expression_node_destroy(node->body);

  free(node);
}

expression_node* for_node_evaluate(for_node* node, scope* s)
{
  expression_node* eval = expression_node_evaluate(node->in_expr, s);

  switch(eval->type) {
  case NODE_ARRAY: {
    if(node->num_ids != 1) {
      fprintf(stderr, "for: arrays require single variable\n");

      expression_node_destroy(eval);
      return nil_node_create();
    }

    expression_node* ret = NULL;

    scope* local = scope_create(s);

    unsigned len = eval->node.array->nelements;

    for(unsigned i = 0; i < len; ++i) {
      scope_insert(local, strdup(node->ids[0]),
                   expression_node_clone(eval->node.array->elements[i]));

      ret = expression_node_evaluate(node->body, local);

      struct bucket* b = scope_get_bucket(local, node->ids[0]);
      expression_node_destroy(b->value);
      b->value = nil_node_create();

      if(i < len - 1)
        expression_node_destroy(ret);
    }

    scope_destroy(local);
    expression_node_destroy(eval);

    return ret ? ret : nil_node_create();
  }

  case NODE_HASH: {
    fprintf(stderr, "for: hashes not yet supported\n");

    if(node->num_ids < 2) {
      fprintf(stderr, "for: hashes require two variable\n");

      expression_node_destroy(eval);
      return nil_node_create();
    }

    break;
  }

  default:
    fprintf(stderr, "for: can only operate on"             \
            " arrays and hashes, type %s is not supported\n",
            node_type_string[eval->type]);
    expression_node_destroy(eval);

    return nil_node_create();
  }

  return nil_node_create();
}

char* for_node_to_string(for_node* node)
{
  buffer b;
  buffer_create(&b, 20);

  char* tmp;

  buffer_puts(&b, "for");

  for(unsigned i = 0; i < node->num_ids; ++i) {
    buffer_putc(&b, ' ');
    buffer_puts(&b, node->ids[i]);
  }

  buffer_puts(&b, " in ");

  tmp = expression_node_to_string(node->in_expr);
  buffer_puts(&b, tmp);
  free(tmp);

  buffer_putc(&b, ' ');
  tmp = expression_node_to_string(node->body);
  buffer_puts(&b, tmp);
  free(tmp);

  return b.buf;
}

char* for_node_inspect(for_node* node)
{
  buffer b;
  buffer_create(&b, 20);

  char* tmp;

  buffer_puts(&b, "for");

  for(unsigned i = 0; i < node->num_ids; ++i) {
    buffer_putc(&b, ' ');
    buffer_puts(&b, node->ids[i]);
  }

  buffer_puts(&b, " in ");

  tmp = expression_node_inspect(node->in_expr);
  buffer_puts(&b, tmp);
  free(tmp);

  buffer_putc(&b, ' ');
  tmp = expression_node_inspect(node->body);
  buffer_puts(&b, tmp);
  free(tmp);

  return b.buf;
}

for_node* for_node_clone(for_node* node)
{
  for_node* new = for_node_create();

  new->num_ids = node->num_ids;
  new->ids = malloc(sizeof(char*) * new->num_ids);

  for(unsigned i = 0; i < new->num_ids; ++i)
    new->ids[i] = strdup(node->ids[i]);

  new->in_expr = expression_node_clone(node->in_expr);
  new->body = expression_node_clone(node->body);

  return new;
}

void for_node_add_id(for_node* node, char* id)
{
  node->ids = realloc(node->ids, sizeof(char*) * ++node->num_ids);
  node->ids[node->num_ids - 1] = strdup(id);
}
