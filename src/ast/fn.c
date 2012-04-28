#include "ast.h"
#include "util.h"
#include "buffer.h"

fn_node* fn_node_create(void)
{
  fn_node* fn = malloc(sizeof(fn_node));
  fn->id = NULL;

  fn->nargs = 0;
  fn->args = NULL;
  fn->body = NULL;

  return fn;
}

void fn_node_destroy(fn_node* fn)
{
  if(fn->id)
    free(fn->id);

  for(unsigned i = 0; i < fn->nargs; ++i)
    free(fn->args[i].id);


  free(fn->args);

  if(fn->body)
    expression_node_destroy(fn->body);

  free(fn);
}

expression_node* fn_node_evaluate(fn_node* fn, scope* scope)
{
  expression_node* clone = expression_node_create(
    NODE_FN, (ast_node) {.fn = fn_node_clone(fn)});

  // assign the id if it's given
  if(fn->id) {

    // remove old binding if it exists
    bucket* existing = scope_get_bucket(scope, fn->id);

    if(existing) {
      expression_node_destroy(existing->value);

      existing->value = expression_node_create(
        NODE_FN, (ast_node) {.fn = fn_node_clone(fn)});

      return clone;
    }

    scope_insert(scope, strdup(fn->id), expression_node_create(
                   NODE_FN, (ast_node) {.fn = fn_node_clone(fn)}));
  }

  return clone;
}

fn_node* fn_node_clone(fn_node* fn)
{
  fn_node* new = fn_node_create();

  new->id = strdup(fn->id);

  unsigned args_len = sizeof(struct typed_id) * fn->nargs;
  new->args = malloc(args_len);

  for(unsigned i = 0; i < args_len; ++i) {
    new->args[i].id = strdup(fn->args[i].id);
    new->args[i].arg_type = fn->args[i].arg_type;
  }

  new->body = expression_node_clone(fn->body);
  return new;
}

char* fn_node_to_string(fn_node* fn)
{
  buffer b;
  buffer_create(&b, 10);

  buffer_puts(&b, "fn ");
  buffer_puts(&b, fn->id ? fn->id : "<fn>");

  buffer_puts(&b, " (");
  for(unsigned i = 0; i < fn->nargs; ++i) {
    buffer_puts(&b, fn->args[i].id);
    if(fn->args[i].arg_type != -1) {
      buffer_putc(&b, ':');
      buffer_puts(&b, node_type_string[fn->args[i].arg_type]);
    }
    if(i != fn->nargs - 1) buffer_putc(&b, ' ');
  }
  buffer_puts(&b, ") ");

  char* body = expression_node_to_string(fn->body);
  buffer_puts(&b, body);
  free(body);

  buffer_putc(&b, '\0');

  return b.buf;
}

char* fn_node_inspect(fn_node* fn)
{
  buffer b;
  buffer_create(&b, 10);

  buffer_puts(&b, "fn ");
  buffer_puts(&b, fn->id ? fn->id : "<fn>");

  buffer_puts(&b, " (");
  for(unsigned i = 0; i < fn->nargs; ++i) {
    buffer_puts(&b, fn->args[i].id);
    if(fn->args[i].arg_type != -1) {
      buffer_putc(&b, ':');
      buffer_puts(&b, node_type_string[fn->args[i].arg_type]);
    }
    if(i != fn->nargs - 1)
      buffer_putc(&b, ' ');
  }
  buffer_puts(&b, ") ");

  char* body = expression_node_inspect(fn->body);
  buffer_puts(&b, body);
  free(body);

  buffer_putc(&b, '\0');

  return b.buf;
}


void fn_node_add_argument(fn_node* fn, char* name, int type)
{
  if(!fn->nargs)
    fn->args = malloc(sizeof(struct typed_id) * ++fn->nargs);
  else
    fn->args = realloc(fn->args, sizeof(struct typed_id) * ++fn->nargs);

  fn->args[fn->nargs - 1] = (struct typed_id) {
    .id = strdup(name),
    .arg_type = type
  };
}
