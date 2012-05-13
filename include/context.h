#pragma once

#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <setjmp.h>
#include <stdlib.h>
#include <stdbool.h>

#include "scope.h"

typedef struct {
  scope* scope;
  bool jmp_set; // has the jmp_buf been set?
  jmp_buf jmp;
} context;

context* context_create(void);
void context_destroy(context* ctx);

#endif /* _CONTEXT_H_ */
