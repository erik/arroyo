#include <stdio.h>

#include "ast.h"
#include "buffer.h"

array_node* array_node_create()
{
  array_node* array = malloc(sizeof(array_node));

  array->nelements = 0;
  array->elements  = malloc(0);

  return array;
}

void array_node_destroy(array_node* array)
{
  for(unsigned i = 0; i < array->nelements; ++i)
    expression_node_destroy(array->elements[i]);
  free(array->elements);
  free(array);
}

expression_node* array_node_evaluate(array_node* array, context* ctx)
{
  array_node* new = array_node_create();

  for(unsigned i = 0; i < array->nelements; ++i) {
    expression_node* eval = expression_node_evaluate(array->elements[i], ctx);
    array_node_push_expression(new, eval);
  }

  return expression_node_create(NODE_ARRAY, (ast_node){.array = new});
}

array_node* array_node_clone(array_node* array)
{
  array_node* new = array_node_create();

  for(unsigned i = 0; i < array->nelements; ++i) {
    expression_node* clone = expression_node_clone(array->elements[i]);
    array_node_push_expression(new, clone);
  }

  return new;
}

expression_node* array_node_call(array_node* array, expression_node* arg, context* ctx)
{
  expression_node* eval = expression_node_evaluate(arg, ctx);

  if(eval->type != NODE_REAL) {
    printf("expected number for array access, not %s\n", node_type_string[arg->type]);
  } else {
    long index = (long)eval->node.real;

    if((double)index != eval->node.real) {
      fprintf(stderr, "warning: array index is noninteger, truncating to %ld\n", index);
    }

    if(index >= array->nelements) {
      fprintf(stderr, "warning: array index out of bounds: %ld\n", index);
    } else {
      expression_node_destroy(eval);
      return expression_node_clone(array->elements[index]);
    }
  }

    expression_node_destroy(eval);
    return nil_node_create();
}

char* array_node_inspect(array_node* array)
{
  buffer b;
  buffer_create(&b, array->nelements * 10 + 2);

  buffer_putc(&b, '[');

  for(unsigned i = 0; i < array->nelements; ++i) {
    char* str = expression_node_inspect(array->elements[i]);
    buffer_puts(&b, str);
    if(i != array->nelements - 1)
      buffer_putc(&b, ' ');
    free(str);
  }

  buffer_putc(&b, ']');

  return b.buf;
}

char* array_node_to_string(array_node* array)
{
  buffer b;
  buffer_create(&b, 10);

  buffer_putc(&b, '[');

  for(unsigned i = 0; i < array->nelements; ++i) {
    char* str = expression_node_to_string(array->elements[i]);
    buffer_puts(&b, str);
    if(i != array->nelements - 1)
      buffer_putc(&b, ' ');
    free(str);
  }

  buffer_putc(&b, ']');

  return b.buf;
}

void array_node_push_expression(array_node* array, expression_node* expr)
{
  array->elements = realloc(array->elements,
                             sizeof(expression_node*) * (array->nelements + 1));
  array->elements[array->nelements++] = expr;
}
