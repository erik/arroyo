#include "ast.h"
#include "buffer.h"

array_node* array_node_create ()
{
  array_node* array = calloc (sizeof (array_node), 1);

  array->nelements = 0;
  array->elements  = malloc (0);

  return array;
}

void array_node_destroy (array_node* array)
{
  free (array->elements);
  free (array);
}

expression_node* array_node_evaluate (array_node* array)
{
  // TODO
  return NULL;
}

string_node* array_node_to_string_node (array_node* array)
{
  buffer b;
  buffer_create (&b, 10);

  buffer_puts (&b, "[ ");

  for (unsigned i = 0; i < array->nelements; ++i) {
    char* str = expression_node_to_string (array->elements[i]);
    buffer_puts (&b, str);
    buffer_putc (&b, ' ');
    free (str);
  }

  buffer_puts (&b, " ]");

  string_node* string = string_node_create (b.buf);
  buffer_destroy (&b);
  return string;
}

void array_node_push_expression (array_node* array, expression_node* expr)
{
  array->elements = realloc (array->elements, array->nelements + 1);
  array->elements[array->nelements++] = expr;
}
