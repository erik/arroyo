#include "ast.h"
#include "buffer.h"

loop_node* loop_node_create(void)
{
  loop_node* loop = malloc(sizeof(loop_node));

  loop->type = LOOP_DO;
  loop->init = NULL;
  loop->cond = NULL;
  loop->body = NULL;

  return loop;
}

void loop_node_destroy(loop_node* loop)
{
  if(loop->init) expression_node_destroy(loop->init);
  if(loop->cond) expression_node_destroy(loop->cond);
  expression_node_destroy(loop->body);

  free(loop);
}

expression_node* loop_node_evaluate(loop_node* loop, context* ctx)
{
  expression_node* value = nil_node_create();
  context* local = context_create();
  local->scope = scope_create(ctx->scope);

  if(loop->init)
    expression_node_destroy(expression_node_evaluate(loop->init, local));

  for(;;) {
    expression_node* cond = expression_node_evaluate(loop->cond, local);
    const bool b = bool_node_value_of(cond);

    const bool do_break = (loop->type == LOOP_WHILE && !b)
      || (loop->type == LOOP_UNTIL && b)
      || false;

    expression_node_destroy(cond);

    if(do_break)
      break;

    expression_node_destroy(value);
    value = expression_node_evaluate(loop->body, local);
  }

  context_destroy(local);

  return value;
}

loop_node* loop_node_clone(loop_node* node)
{
  loop_node* new = loop_node_create();

  new->type = node->type;

  if(node->init)
    new->init = expression_node_clone(node->init);
  new->cond = expression_node_clone(node->cond);
  new->body = expression_node_clone(node->body);

  return new;
}

char* loop_node_to_string(loop_node* loop)
{
  char* tmp;
  buffer b;
  buffer_create(&b, 10);

  buffer_puts(&b, "loop ");
  if(loop->init) {
    tmp = expression_node_to_string(loop->init);
    buffer_puts(&b, tmp);
    buffer_putc(&b, ' ');
    free(tmp);
  }

  switch(loop->type) {
  case LOOP_DO:
    buffer_puts(&b, "do");
    break;
  case LOOP_UNTIL:
    buffer_puts(&b, "until ");
    tmp = expression_node_to_string(loop->cond);
    buffer_puts(&b, tmp);
    free(tmp);
    break;
  case LOOP_WHILE:
    buffer_puts(&b, "while ");
    tmp = expression_node_to_string(loop->cond);
    buffer_puts(&b, tmp);
    free(tmp);
    break;
  }

  buffer_putc(&b, ' ');

  tmp = expression_node_to_string(loop->body);
  buffer_puts(&b, tmp);
  free(tmp);

  buffer_putc(&b, '\0');

  return b.buf;
}


char* loop_node_inspect(loop_node* loop)
{
  char* tmp;
  buffer b;
  buffer_create(&b, 10);

  buffer_puts(&b, "loop ");
  if(loop->init) {
    tmp = expression_node_inspect(loop->init);
    buffer_puts(&b, tmp);
    buffer_putc(&b, ' ');
    free(tmp);
  }

  switch(loop->type) {
  case LOOP_DO:
    buffer_puts(&b, "do");
    break;
  case LOOP_UNTIL:
    buffer_puts(&b, "until ");
    tmp = expression_node_inspect(loop->cond);
    buffer_puts(&b, tmp);
    free(tmp);
    break;
  case LOOP_WHILE:
    buffer_puts(&b, "while ");
    tmp = expression_node_inspect(loop->cond);
    buffer_puts(&b, tmp);
    free(tmp);
    break;
  }

  buffer_putc(&b, ' ');

  tmp = expression_node_inspect(loop->body);
  buffer_puts(&b, tmp);
  buffer_putc(&b, '\0');

  return b.buf;
}
