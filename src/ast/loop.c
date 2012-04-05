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
  expression_node* value = NULL;

  scope* local = scope_create(s);

  if(loop->init) {
    value = expression_node_evaluate(loop->init, local);
    expression_node_destroy(value);
  }

  for(;;) {
    value = expression_node_evaluate(loop->body, local);

    expression_node* cond = expression_node_evaluate(loop->cond, local);
    bool_node* bool = bool_node_from_expression(cond);

    int bool_val = bool->bool;

    expression_node_destroy(cond);
    bool_node_destroy(bool);


    if(loop->type == LOOP_DO) {
      // TODO: currently no way to break from do loop
    }

    else if(loop->type == LOOP_WHILE) {
      if(!bool_val)
        break;
    }

    else if(loop->type == LOOP_UNTIL) {
      if(bool_val)
        break;
    }

    expression_node_destroy(value);

  }
  scope_destroy(local);

  return value ? value : nil_node_create();
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
