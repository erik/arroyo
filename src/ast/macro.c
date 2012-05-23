#include <stdio.h>

#include "ast.h"
#include "buffer.h"
#include "scope.h"
#include "util.h"

expression_node* macro_node_call(macro_node* macro, expression_node* args,
                                 context* ctx)
{
  if(args->type != NODE_BLOCK) {
    printf("error: expected argument list, not %s\n", node_type_string[args->type]);
    return nil_node_create();
  }

  context* local = context_create();
  local->scope = scope_create(ctx->scope);

  struct expression_list* arg = args->node.block->list;

  if(macro->arity < 0) {
    unsigned min_args = -(macro->arity + 1);

    if(args->node.block->nelements < min_args) {
      context_destroy(local);
      fprintf(stderr, "error: incorrect number of arguments (%d for %d+)\n",
              args->node.block->nelements, min_args);
      return nil_node_create();
    }

    for(unsigned i = 0; i < min_args; ++i) {
      expression_node* expr = expression_node_clone(arg->expression);

      if(macro->args[i].type != ARG_UNTYPED &&
         expr->type != (node_type)macro->args[i].type) {
        fprintf(stderr, "error: incorrect argument type for argument %d: " \
                "got %s, expected %s\n" ,
                i, node_type_string[expr->type],
                node_type_string[macro->args[i].type]);
        return nil_node_create();
      }

      scope_insert(local->scope, strdup(macro->args[i].id), expr);
      arg = arg->next;
    }

    array_node* array = array_node_create();

    for(unsigned i = min_args; i < args->node.block->nelements; ++i) {
      array_node_push_expression(
        array, expression_node_clone(arg->expression));
      arg = arg->next;
    }

    scope_insert(local->scope, strdup(macro->args[min_args].id),
                 expression_node_create(NODE_ARRAY,
                                        (ast_node){.array=array}));
  }

  else {

    if(args->node.block->nelements != macro->nargs) {
      context_destroy(local);
      fprintf(stderr, "error: incorrect number of arguments (%d for %d)\n",
              args->node.block->nelements, macro->nargs);
      return nil_node_create();
    }

    for(unsigned i = 0; i < macro->nargs; ++i) {
      expression_node* expr = expression_node_clone(arg->expression);

      if(macro->args[i].type != ARG_UNTYPED &&
         expr->type != (node_type)macro->args[i].type) {
        fprintf(stderr, "error: incorrect argument type for argument %d: "\
                "got %s, expected %s\n" ,
                i, node_type_string[expr->type],
                node_type_string[macro->args[i].type]);
        return nil_node_create();
      }

      scope_insert(local->scope, strdup(macro->args[i].id), expr);

      arg = arg->next;
    }
  }

  expression_node* ret = expression_node_evaluate(macro->body, local);
  context_destroy(local);

  return ret;
}

expression_node* macro_node_evaluate(macro_node* macro, context* ctx)
{
  // assign the id if it's given
  if(macro->id) {

    // remove old binding if it exists
    bucket* existing = scope_get_bucket(ctx->scope, macro->id);

    if(existing) {
      expression_node_destroy(existing->value);

      existing->value = expression_node_create(
        NODE_MACRO, (ast_node) {.fn = fn_node_clone(macro)});

      return expression_node_create(
        NODE_MACRO, (ast_node) {.fn = fn_node_clone(macro)});

    }

    scope_insert(ctx->scope, strdup(macro->id), expression_node_create(
                   NODE_MACRO, (ast_node) {.fn = fn_node_clone(macro)}));
  }

  return expression_node_create(NODE_MACRO,
                                (ast_node) {.fn = fn_node_clone(macro) });
}


char* macro_node_to_string(macro_node* macro)
{
  buffer b;
  buffer_create(&b, 10);

  buffer_puts(&b, "macro ");
  buffer_puts(&b, macro->id ? macro->id : "<macro>");

  buffer_puts(&b, " (");
  for(unsigned i = 0; i < macro->nargs; ++i) {
    buffer_puts(&b, macro->args[i].id);
    if(macro->args[i].type != ARG_UNTYPED) {
      buffer_putc(&b, ':');
      buffer_puts(&b, node_type_string[macro->args[i].type]);
    }
    if(i != macro->nargs - 1) buffer_putc(&b, ' ');
  }
  buffer_puts(&b, ") ");

  char* body = expression_node_to_string(macro->body);
  buffer_puts(&b, body);
  free(body);

  buffer_putc(&b, '\0');

  return b.buf;
}

char* macro_node_inspect(macro_node* macro)
{
  buffer b;
  buffer_create(&b, 10);

  buffer_puts(&b, "macro ");
  buffer_puts(&b, macro->id ? macro->id : "<macro>");

  buffer_puts(&b, " (");
  for(unsigned i = 0; i < macro->nargs; ++i) {
    buffer_puts(&b, macro->args[i].id);
    if(macro->args[i].type != ARG_UNTYPED) {
      buffer_putc(&b, ':');
      buffer_puts(&b, node_type_string[macro->args[i].type]);
    }
    if(i != macro->nargs - 1)
      buffer_putc(&b, ' ');
  }
  buffer_puts(&b, ") ");

  char* body = expression_node_inspect(macro->body);
  buffer_puts(&b, body);
  free(body);

  buffer_putc(&b, '\0');

  return b.buf;
}
