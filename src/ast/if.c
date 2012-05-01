#include "ast.h"
#include "buffer.h"

if_node* if_node_create(void)
{
  if_node* node = malloc(sizeof(if_node));
  node->condition = NULL;
  node->thenbody = NULL;

  node->nelseif = 0;
  node->elseifcondition = NULL;
  node->elseifbody = NULL;

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

expression_node* if_node_evaluate(if_node* node, scope* scope)
{
  expression_node* value, *cond;
  int bool;

  cond = expression_node_evaluate(node->condition, scope);
  bool = bool_node_value_of(cond);

  expression_node_destroy(cond);

  if(bool)
    return expression_node_evaluate(node->thenbody, scope);

  for(unsigned i = 0; i < node->nelseif; ++i) {
    cond = expression_node_evaluate(node->elseifcondition[i], scope);
    bool = bool_node_value_of(cond);

    expression_node_destroy(cond);
    if(bool)
      return expression_node_evaluate(node->elseifbody[i], scope);
  }

  if(node->elsebody)
    return expression_node_evaluate(node->elsebody, scope);

  return nil_node_create();
}

if_node* if_node_clone(if_node* node)
{
  if_node* new = if_node_create();

  new->condition = expression_node_clone(node->condition);
  new->thenbody  = expression_node_clone(node->thenbody);

  for(unsigned i = 0; i < node->nelseif; ++i)
    if_node_add_elseif(new, node->elseifcondition[i], node->elseifbody[i]);

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
    buffer_puts(&b, " else ");
    tmp = expression_node_to_string(node->elsebody);
    buffer_puts(&b, tmp);
    buffer_putc(&b, ' ');
    free(tmp);
  }

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
    buffer_puts(&b, " else ");
    tmp = expression_node_inspect(node->elsebody);
    buffer_puts(&b, tmp);
    buffer_putc(&b, ' ');
    free((char*)tmp);
  }

  return b.buf;
}

void if_node_add_elseif(if_node* node, expression_node* cond, expression_node* body)
{
  node->elseifcondition = realloc(node->elseifcondition, node->nelseif + 1);
  node->elseifbody = realloc(node->elseifbody, node->nelseif + 1);

  node->elseifcondition[node->nelseif] = cond;
  node->elseifbody[node->nelseif++] = body;
}
