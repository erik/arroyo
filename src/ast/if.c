#include "ast.h"
#include "buffer.h"

if_node* if_node_create(void)
{
  if_node* node = calloc(sizeof(if_node), 1);
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

expression_node* if_node_evaluate(if_node* node)
{
  expression_node* value, *cond;
  bool_node* bool;

  cond = expression_node_evaluate(node->condition);
  bool = bool_node_from_expression(cond);
  if(bool->bool) {
    expression_node_destroy(cond);
    bool_node_destroy(bool);
    return expression_node_evaluate(node->thenbody);
  }

  for(unsigned i = 0; i < node->nelseif; ++i) {
    cond = expression_node_evaluate(node->elseifcondition[i]);
    bool = bool_node_from_expression(cond);

    if(bool->bool) {
      expression_node_destroy(cond);
      bool_node_destroy(bool);
      return expression_node_evaluate(node->elseifbody[i]);
    }

    expression_node_destroy(cond);
    bool_node_destroy(bool);
  }

  if(node->elsebody)
    return expression_node_evaluate(node->elsebody);

  return expression_node_create(NODE_NIL, NULL);
}

string_node* if_node_to_string_node(if_node* node)
{
  string_node* string;
  const char* tmp;

  buffer b;
  buffer_create(&b, 10);

  buffer_puts(&b, "if ");

  // condition
  tmp = expression_node_to_string(node->condition);
  buffer_puts(&b, tmp);
  buffer_putc(&b, ' ');
  free((char*)tmp);

  // then
  tmp = expression_node_to_string(node->thenbody);
  buffer_puts(&b, tmp);
  free((char*)tmp);


  for(unsigned i = 0; i < node->nelseif; ++i) {
    buffer_puts(&b, " elseif ");
    // else if cond
    tmp = expression_node_to_string(node->elseifcondition[i]);
    buffer_puts(&b, tmp);
    buffer_putc(&b, ' ');
    free((char*)tmp);

    // else if body
    tmp = expression_node_to_string(node->elseifbody[i]);
    buffer_puts(&b, tmp);
    buffer_putc(&b, ' ');
    free((char*)tmp);
  }

  if(node->elsebody) {
    buffer_puts(&b, " else ");
    tmp = expression_node_to_string(node->elsebody);
    buffer_puts(&b, tmp);
    buffer_putc(&b, ' ');
    free((char*)tmp);
  }

  string = string_node_create(b.buf);
  buffer_destroy(&b);

  return string;
}

void if_node_add_elseif(if_node* node, expression_node* cond, expression_node* body)
{
  node->elseifcondition = realloc(node->elseifcondition, node->nelseif + 1);
  node->elseifbody = realloc(node->elseifbody, node->nelseif + 1);

  node->elseifcondition[node->nelseif] = cond;
  node->elseifbody[node->nelseif++] = body;

}
