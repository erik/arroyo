#include <stdio.h>

#include "ast.h"
#include "util.h"
#include "buffer.h"

fn_node* fn_node_create(void)
{
  fn_node* fn = malloc(sizeof(fn_node));
  fn->id = NULL;

  fn->nargs = 0;
  fn->arity = 0;
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

  if(fn->body) {
    expression_node_destroy(fn->body);
    fn->body = NULL;
  }

  free(fn);
}

expression_node* fn_node_evaluate(fn_node* fn, context* ctx)
{
  // assign the id if it's given
  if(fn->id) {

    // remove old binding if it exists
    bucket* existing = scope_get_bucket(ctx->scope, fn->id);

    if(existing) {
      expression_node_destroy(existing->value);

      existing->value = expression_node_create(
        NODE_FN, (ast_node) {.fn = fn_node_clone(fn)});

      return expression_node_create(
        NODE_FN, (ast_node) {.fn = fn_node_clone(fn)});

    }

    scope_insert(ctx->scope, strdup(fn->id), expression_node_create(
                   NODE_FN, (ast_node) {.fn = fn_node_clone(fn)}));
  }

  return expression_node_create(NODE_FN, (ast_node) {.fn = fn_node_clone(fn) });
}

fn_node* fn_node_clone(fn_node* fn)
{
  fn_node* new = fn_node_create();

  if(fn->id)
    new->id = strdup(fn->id);

  new->args = malloc(sizeof(struct typed_id) * fn->nargs);
  new->nargs = fn->nargs;
  new->arity = fn->arity;

  for(unsigned i = 0; i < fn->nargs; ++i) {
    new->args[i].id = strdup(fn->args[i].id);
    new->args[i].type = fn->args[i].type;
  }

  new->body = expression_node_clone(fn->body);
  return new;
}

expression_node* fn_node_call(fn_node* fn, expression_node* args, context* ctx)
{
  if(args->type != NODE_BLOCK) {
    printf("error: expected argument list, not %s\n", node_type_string[args->type]);
    return nil_node_create();
  }

  context* local = context_create();
  local->scope = scope_create(ctx->scope);

  struct expression_list* arg = args->node.block->list;

  if(fn->arity < 0) {
    unsigned min_args = -(fn->arity + 1);

    if(args->node.block->nelements < min_args) {
      context_destroy(local);
      fprintf(stderr, "error: incorrect number of arguments (%d for %d+)\n",
              args->node.block->nelements, min_args);
      return nil_node_create();
    }

    for(unsigned i = 0; i < min_args; ++i) {
      expression_node* expr = expression_node_evaluate(arg->expression, local);

      if(fn->args[i].type != ARG_UNTYPED && expr->type != (node_type)fn->args[i].type) {
        fprintf(stderr, "error: incorrect argument type for argument %d: got %s, expected %s\n" ,
                i, node_type_string[expr->type], node_type_string[fn->args[i].type]);
        return nil_node_create();
      }

      scope_insert(local->scope, strdup(fn->args[i].id), expr);
      arg = arg->next;
    }

    array_node* array = array_node_create();

    for(unsigned i = min_args; i < args->node.block->nelements; ++i) {
      array_node_push_expression(
        array, expression_node_evaluate(arg->expression, local));
      arg = arg->next;
    }

    scope_insert(local->scope, strdup(fn->args[min_args].id),
                 expression_node_create(NODE_ARRAY,
                                        (ast_node){.array=array}));

  }

  else {

    if(args->node.block->nelements != fn->nargs) {
      context_destroy(local);
      fprintf(stderr, "error: incorrect number of arguments (%d for %d)\n",
              args->node.block->nelements, fn->nargs);
      return nil_node_create();
    }

    for(unsigned i = 0; i < fn->nargs; ++i) {
      expression_node* expr = expression_node_evaluate(arg->expression, local);

      if(fn->args[i].type != ARG_UNTYPED && expr->type != (node_type)fn->args[i].type) {
        fprintf(stderr, "error: incorrect argument type for argument %d: got %s, expected %s\n" ,
                i, node_type_string[expr->type], node_type_string[fn->args[i].type]);
        return nil_node_create();
      }

      scope_insert(local->scope, strdup(fn->args[i].id), expr);

      arg = arg->next;
    }
  }

  expression_node* ret = expression_node_evaluate(fn->body, local);
  context_destroy(local);

  return ret;
}

char* fn_node_to_string(fn_node* fn)
{
  buffer b;
  buffer_create(&b, 10);

  buffer_puts(&b, "fn ");
  buffer_puts(&b, fn->id ? fn->id : "<fn>");

  buffer_puts(&b, " (");
  for(unsigned i = 0; i < fn->nargs; ++i) {
    if(fn->args[i].type == ARG_SPLAT)
      buffer_putc(&b, '*');
    buffer_puts(&b, fn->args[i].id);

    if(fn->args[i].type != ARG_UNTYPED &&
       fn->args[i].type != ARG_SPLAT) {

      buffer_putc(&b, ':');
      buffer_puts(&b, node_type_string[fn->args[i].type]);
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
    if(fn->args[i].type == ARG_SPLAT)
      buffer_putc(&b, '*');

    buffer_puts(&b, fn->args[i].id);

    if(fn->args[i].type != ARG_UNTYPED &&
       fn->args[i].type != ARG_SPLAT) {
      buffer_putc(&b, ':');
      buffer_puts(&b, node_type_string[fn->args[i].type]);
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
  fn->args = realloc(fn->args, sizeof(struct typed_id) * ++fn->nargs);

  fn->args[fn->nargs - 1] = (struct typed_id) {
    .id = strdup(name),
    .type = type
  };

  if(type == -2)
    fn->arity = -1 - fn->arity;

  else
    fn->arity += 1;
}
