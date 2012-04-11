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
  // TODO
  return NULL;
}

char* case_node_to_string(case_node* node)
{
  // TODO
  return NULL;
}

char* case_node_inspect(case_node* node)
{
  // TODO
  return NULL;
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
