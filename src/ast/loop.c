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

expression_node* loop_node_evaluate(loop_node* loop, scope* s)
{
  expression_node* value = nil_node_create();
  scope* local = scope_create(s);

  if(loop->init)
    expression_node_destroy(expression_node_evaluate(loop->init, local));

  for(;;) {
    expression_node* cond = expression_node_evaluate(loop->cond, local);

    // this is very ugly, but doesn't require branching, so it may
    // speed up evaluation ever so slightly for some loops

    const int bool_val =
      (cond->type == NODE_BOOL && ((bool_node*)cond->ast_node)->bool)
      || cond->type == NODE_NIL;

    const int do_break =
      (loop->type == LOOP_WHILE && !bool_val)
      || (loop->type == LOOP_UNTIL && bool_val)
      || 0;

    expression_node_destroy(cond);

    if(do_break)
      break;

    expression_node_destroy(value);
    value = expression_node_evaluate(loop->body, local);
  }

  scope_destroy(local);

  return value;
}

expression_node* loop_node_clone(loop_node* node)
{
  // TODO
  return NULL;
}

string_node* loop_node_to_string_node(loop_node* loop)
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
  buffer_putc(&b, '\0');

  string_node* string = string_node_create(b.buf);
  buffer_destroy(&b);

  return string;
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
