#include "ast.h"
#include "util.h"
#include "buffer.h"

case_node* case_node_create(void)
{
  case_node* node = calloc(sizeof(case_node), 1);

  return node;
}

void case_node_destroy(case_node* node)
{
  expression_node_destroy(node->expression);

  for(struct case_list* c = node->cases; c; ) {
    struct case_list* next = c->next;

    expression_node_destroy(c->cond);
    expression_node_destroy(c->body);

    free(c);
    c = next;
  }

  if(node->default_case)
    expression_node_destroy(node->default_case);

  free(node);
}

expression_node* case_node_evaluate(case_node* node, scope* s)
{
  scope* local = scope_create(s);
  expression_node* eval = expression_node_evaluate(node->expression, local);

  expression_node* result = NULL;

  for(struct case_list* c = node->cases; c; c = c->next) {
    expression_node* cond = expression_node_evaluate(c->cond, local);

    if(expression_node_equal(cond, eval)) {
      expression_node_destroy(cond);
      result = expression_node_evaluate(c->body, local);
      break;
    }

    expression_node_destroy(cond);
  }

  if(!result && node->default_case) {
    result = expression_node_evaluate(node->default_case, local);
  }

  expression_node_destroy(eval);
  scope_destroy(local);

  return result ? result : nil_node_create();
}

case_node* case_node_clone(case_node* node)
{
  case_node* new = case_node_create();

  new->expression = expression_node_clone(node->expression);

  for(struct case_list* c = node->cases; c; c = c->next) {
    case_node_add_case(new, expression_node_clone(c->cond),
                       expression_node_clone(c->body));
  }

  if(node->default_case)
    new->default_case = expression_node_clone(node->default_case);

  return new;
}

char* case_node_to_string(case_node* node)
{
  buffer b;
  buffer_create(&b, 20);

  char* tmp;

  buffer_puts(&b, "case ");

  tmp = expression_node_to_string(node->expression);
  buffer_puts(&b, tmp);
  free(tmp);

  buffer_puts(&b, " of (");

  for(struct case_list* c = node->cases; c; c = c->next) {
    tmp = expression_node_to_string(c->cond);
    buffer_puts(&b, tmp);
    free(tmp);

    buffer_puts(&b, " => ");

    tmp = expression_node_to_string(c->body);
    buffer_puts(&b, tmp);
    free(tmp);

    if(c->next)
      buffer_puts(&b, ", ");
  }

  if(node->default_case) {
    buffer_puts(&b, " default => ");
    tmp = expression_node_to_string(node->default_case);
    buffer_puts(&b, tmp);
    free(tmp);
  }

  buffer_putc(&b, ')');

  return b.buf;
}

char* case_node_inspect(case_node* node)
{
  buffer b;
  buffer_create(&b, 20);

  char* tmp;

  buffer_puts(&b, "case ");

  tmp = expression_node_inspect(node->expression);
  buffer_puts(&b, tmp);
  free(tmp);

  buffer_puts(&b, " of (");

  for(struct case_list* c = node->cases; c; c = c->next) {
    tmp = expression_node_inspect(c->cond);
    buffer_puts(&b, tmp);
    free(tmp);

    buffer_puts(&b, " => ");

    tmp = expression_node_inspect(c->body);
    buffer_puts(&b, tmp);
    free(tmp);

    if(c->next)
      buffer_puts(&b, ", ");
  }

  if(node->default_case) {
    buffer_puts(&b, " default => ");
    tmp = expression_node_inspect(node->default_case);
    buffer_puts(&b, tmp);
    free(tmp);
  }

  buffer_putc(&b, ')');

  return b.buf;
}

void case_node_add_case(case_node* node, expression_node* of, expression_node* body)
{
  struct case_list* new = malloc(sizeof(struct case_list));

  new->cond = of;
  new->body = body;
  new->next = NULL;

  if(!node->cases)
    node->cases = node->last = new;
  else
    node->last = node->last->next = new;
}
