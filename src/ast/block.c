#include "ast.h"
#include "buffer.h"

block_node* block_node_create(void)
{
  block_node* node = malloc(sizeof(block_node));
  node->list = NULL;
  node->last = NULL;
  node->nelements = 0;
  return node;
}

void block_node_destroy(block_node* block)
{
  for(struct expression_list* ptr = block->list; ptr;) {
    expression_node_destroy(ptr->expression);
    struct expression_list* next = ptr->next;
    free(ptr);
    ptr = next;
  }

  free(block);
}

expression_node* block_node_evaluate(block_node* block, context* ctx)
{
  scope* local = scope_create(ctx->scope);

  expression_node* last = NULL;

  for(struct expression_list* ptr = block->list; ptr; ptr = ptr->next) {
    last = expression_node_evaluate(ptr->expression, ctx);
    if(ptr->next)
      expression_node_destroy(last);
  }

  scope_destroy(local);

  if(!last)
    return nil_node_create();

  return last;
}

block_node* block_node_clone(block_node* block)
{
  block_node* new = block_node_create();

  for(struct expression_list* ptr = block->list; ptr; ptr = ptr->next)
    block_node_push_expression(new, expression_node_clone(ptr->expression));

  return new;
}

void block_node_push_expression(block_node* block, expression_node* node)
{
  struct expression_list* new = malloc(sizeof(struct expression_list));
  new->expression = node;
  new->next = NULL;

  block->nelements += 1;

  if(!block->list)
    block->list = block->last = new;
  else
    block->last = block->last->next = new;

}

char* block_node_to_string(block_node* block)
{
  buffer b;

  buffer_create(&b, 10);
  buffer_putc(&b, '(');

  for(struct expression_list* ptr = block->list; ptr; ptr = ptr->next) {
    char *str = expression_node_to_string(ptr->expression);
    buffer_puts(&b, str);
    if(ptr->next)
      buffer_putc(&b, ' ');
    free(str);
  }

  buffer_putc(&b, ')');
  buffer_putc(&b, '\0');

  return b.buf;
}

char* block_node_inspect(block_node* block)
{
  buffer b;
  buffer_create(&b, 10);
  buffer_putc(&b, '(');

  for(struct expression_list* ptr = block->list; ptr; ptr = ptr->next) {
    char *str = expression_node_inspect(ptr->expression);
    buffer_puts(&b, str);
    if(ptr->next)
      buffer_putc(&b, ' ');
    free(str);
  }

  buffer_putc(&b, ')');
  buffer_putc(&b, '\0');

  return b.buf;
}
