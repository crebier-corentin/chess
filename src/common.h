#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_MIMALLOC
#include <mimalloc.h>

#define malloc(n) mi_malloc(n)
#define calloc(n, c) mi_calloc(n, c)
#define realloc(p, n) mi_realloc(p, n)
#define free(p) mi_free(p)

#endif
