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

expression_node* array_node_evaluate(array_node* array, scope* scope)
{
  // TODO
  return NULL;
}

expression_node* array_node_clone(array_node* array)
{
  // TODO
  return NULL;
}

string_node* array_node_to_string_node(array_node* array)
{
  buffer b;
  buffer_create(&b, 10);

  buffer_putc(&b, '[');

  for(unsigned i = 0; i < array->nelements; ++i) {
    char* str = expression_node_to_string(array->elements[i]);
    buffer_puts(&b, str);
    if(i != array->nelements - 1) buffer_putc(&b, ' ');
    free(str);
  }

  buffer_putc(&b, ']');

  string_node* string = string_node_create(b.buf);
  buffer_destroy(&b);
  return string;
}

void array_node_push_expression(array_node* array, expression_node* expr)
{
  array->elements = realloc(array->elements,
                             sizeof(expression_node*) * (array->nelements + 1));
  array->elements[array->nelements++] = expr;
}
