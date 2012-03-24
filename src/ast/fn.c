#include "ast.h"
#include "util.h"
#include "buffer.h"

fn_node* fn_node_create ()
{
  fn_node* fn = calloc (sizeof (fn_node), 1);
  fn->id = NULL;

  fn->nargs = 0;
  fn->args = malloc (0);
  fn->body = NULL;

  return fn;
}

void fn_node_destroy (fn_node* fn)
{
  if (fn->id) free (fn->id);

  for (unsigned i = 0; i < fn->nargs; ++i) {
    free (fn->args[i].id);
  }
  free (fn->args);

  expression_node_destroy (fn->body);

  free (fn);
}

expression_node* fn_node_evaluate (fn_node* fn)
{
  // TODO:
  return NULL;
}

string_node* fn_node_to_string_node (fn_node* fn)
{
  buffer b;
  buffer_create (&b, 10);

  buffer_puts (&b, "fn ");
  buffer_puts (&b, fn->id ? fn->id : "<fn>");

  buffer_puts (&b, " (");
  for (unsigned i = 0; i < fn->nargs; ++i) {
    buffer_puts (&b, fn->args[i].id);
    buffer_putc (&b, ' ');
    if (fn->args[i].arg_type != -1)
      buffer_puts (&b, node_type_string[fn->args[i].arg_type]);
  }
  buffer_puts (&b, ") ");

  char* body = expression_node_to_string (fn->body);
  buffer_puts (&b, body);
  free (body);

  buffer_putc (&b, '\0');

  string_node* string = string_node_create (b.buf);

  buffer_destroy (&b);

  return string;
}

void fn_node_add_argument (fn_node* fn, char* name, int type)
{
  fn->args = realloc (fn->args, sizeof (struct typed_id) * ++fn->nargs);
  fn->args[fn->nargs - 1] = (struct typed_id) {
    .id = strdup (name),
    .arg_type = type
  };
}

void fn_node_set_id (fn_node* fn, char* name)
{
  if (fn->id) free (fn->id);
  fn->id = strdup (name);
}

void fn_node_set_body (fn_node* fn, expression_node* expr)
{
  fn->body = expr;
}
