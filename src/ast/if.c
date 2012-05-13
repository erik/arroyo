#include "ast.h"
#include "buffer.h"

if_node* if_node_create(void)
{
  if_node* node = malloc(sizeof(if_node));
  node->condition = NULL;
  node->thenbody = NULL;

  node->nelseif = 0;
  node->elseifcondition = malloc(0);
  node->elseifbody = malloc(0);

  node->elsebody = NULL;

  return node;
}

void if_node_destroy(if_node* node)
{
  expression_node_destroy(node->condition);
  expression_node_destroy(node->thenbody);

  for(unsigned i = 0; i < node->nelseif; ++i) {
    expression_node_destroy(node->elseifcondition[i]);
    expression_node_destroy(node->elseifbody[i]);
  }

  free(node->elseifbody);
  free(node->elseifcondition);

  if(node->elsebody)
    expression_node_destroy(node->elsebody);

  free(node);
}

expression_node* if_node_evaluate(if_node* node, context* ctx)
{
  expression_node* value, *cond;
  bool b;

  cond = expression_node_evaluate(node->condition, ctx);
  b = bool_node_value_of(cond);

  expression_node_destroy(cond);

  if(b)
    return expression_node_evaluate(node->thenbody, ctx);

  for(unsigned i = 0; i < node->nelseif; ++i) {
    cond = expression_node_evaluate(node->elseifcondition[i], ctx);
    b = bool_node_value_of(cond);

    expression_node_destroy(cond);
    if(b)
      return expression_node_evaluate(node->elseifbody[i], ctx);
  }

  if(node->elsebody)
    return expression_node_evaluate(node->elsebody, ctx);

  return nil_node_create();
}

if_node* if_node_clone(if_node* node)
{
  if_node* new = if_node_create();

  new->condition = expression_node_clone(node->condition);
  new->thenbody  = expression_node_clone(node->thenbody);

  for(unsigned i = 0; i < node->nelseif; ++i)
    if_node_add_elseif(new,
                       expression_node_clone(node->elseifcondition[i]),
                       expression_node_clone(node->elseifbody[i]));

  if(node->elsebody)
    new->elsebody  = expression_node_clone(node->elsebody);

  return new;
}

char* if_node_to_string(if_node* node)
{
  char* tmp;

  buffer b;
  buffer_create(&b, 10);

  buffer_puts(&b, "if ");

  // condition
  tmp = expression_node_to_string(node->condition);
  buffer_puts(&b, tmp);
  buffer_putc(&b, ' ');
  free(tmp);

  // then
  tmp = expression_node_to_string(node->thenbody);
  buffer_puts(&b, tmp);
  free(tmp);


  for(unsigned i = 0; i < node->nelseif; ++i) {
    buffer_puts(&b, " elseif ");
    // else if cond
    tmp = expression_node_to_string(node->elseifcondition[i]);
    buffer_puts(&b, tmp);
    buffer_putc(&b, ' ');
    free(tmp);

    // else if body
    tmp = expression_node_to_string(node->elseifbody[i]);
    buffer_puts(&b, tmp);
    buffer_putc(&b, ' ');
    free(tmp);
  }

  if(node->elsebody) {
    buffer_puts(&b, "else ");
    tmp = expression_node_to_string(node->elsebody);
    buffer_puts(&b, tmp);
    free(tmp);
  }

  buffer_putc(&b, 0);

  return b.buf;
}

char* if_node_inspect(if_node* node)
{
  char* tmp;
  buffer b;
  buffer_create(&b, 10);

  buffer_puts(&b, "if ");

  // condition
  tmp = expression_node_inspect(node->condition);
  buffer_puts(&b, tmp);
  buffer_putc(&b, ' ');
  free((char*)tmp);

  // then
  tmp = expression_node_inspect(node->thenbody);
  buffer_puts(&b, tmp);
  free((char*)tmp);


  for(unsigned i = 0; i < node->nelseif; ++i) {
    buffer_puts(&b, " elseif ");
    // else if cond
    tmp = expression_node_inspect(node->elseifcondition[i]);
    buffer_puts(&b, tmp);
    buffer_putc(&b, ' ');
    free((char*)tmp);

    // else if body
    tmp = expression_node_inspect(node->elseifbody[i]);
    buffer_puts(&b, tmp);
    buffer_putc(&b, ' ');
    free((char*)tmp);
  }

  if(node->elsebody) {
    buffer_puts(&b, "else ");
    tmp = expression_node_inspect(node->elsebody);
    buffer_puts(&b, tmp);
    free((char*)tmp);
  }

  buffer_putc(&b, 0);

  return b.buf;
}

void if_node_add_elseif(if_node* node, expression_node* cond, expression_node* body)
{
  node->nelseif += 1;

  node->elseifcondition = realloc(node->elseifcondition, sizeof(expression_node*) *
                                  node->nelseif);
  node->elseifbody = realloc(node->elseifbody, sizeof(expression_node*) *
                             node->nelseif);

  node->elseifcondition[node->nelseif - 1] = cond;
  node->elseifbody[node->nelseif - 1] = body;
}
