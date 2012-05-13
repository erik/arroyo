#include "context.h"

context* context_create(void)
{
  context* new = calloc(sizeof(context), 1);
  /// TODO
  return new;
}

void context_destroy(context* ctx)
{
  if(ctx->scope)
    scope_destroy(ctx->scope);

  free(ctx);
}


