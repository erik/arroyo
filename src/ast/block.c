#include "ast.h"
#include "buffer.h"

block_node* block_node_create(void)
{
  block_node* node = calloc(sizeof(block_node), 1);
  node->nexpressions = 0;
  node->expressions = malloc(sizeof(expression_node*));
  return node;
}

void block_node_destroy(block_node* block)
{

  for(unsigned i = 0; i < block->nexpressions; ++i)
    expression_node_destroy(block->expressions[i]);

  free(block->expressions);
  free(block);
}

expression_node* block_node_evaluate(block_node* block, scope* s)
{
  scope* local = scope_create(s);

  expression_node* last = NULL;

  for(unsigned i = 0; i < block->nexpressions; ++i) {
    last = expression_node_evaluate(block->expressions[i], local);

    if(i != block->nexpressions - 1)
      expression_node_destroy(last);
  }

  scope_destroy(local);

  if(!last)
    return nil_node_create();

  return last;
}

expression_node* block_node_clone(block_node* block)
{
  // TODO
  return NULL;
}

void block_node_push_expression(block_node* block, expression_node* node)
{
  block->expressions = realloc(block->expressions,
                                ++block->nexpressions * sizeof(expression_node*));

  block->expressions[block->nexpressions - 1] = node;
}

string_node* block_node_to_string_node(block_node* block)
{
  buffer b;

  buffer_create(&b, 10);
  buffer_putc(&b, '(');

  for(unsigned i = 0; i < block->nexpressions; ++i) {
    char *str = expression_node_to_string(block->expressions[i]);
    buffer_puts(&b, str);
    if(i != block->nexpressions - 1) buffer_putc(&b, ' ');
    free(str);
  }

  buffer_putc(&b, ')');
  buffer_putc(&b, '\0');

  string_node* node = string_node_create(b.buf);

  buffer_destroy(&b);

  return node;
}
