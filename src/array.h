#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define Array(T) T *

typedef struct ArrayHeader
{
    size_t len;
    size_t cap;
} ArrayHeader;

#define array_header(a) ((ArrayHeader *)(a)-1)
#define array_len(a) (array_header(a)->len)
#define array_cap(a) (array_header(a)->cap)

void *array_create_size_func(size_t elSize, size_t cap);

#define array_create_size(T, size) ((Array(T))array_create_size_func(sizeof(T), size))
#define array_create(T) array_create_size(T, 0)
#define array_free(a) (free(array_header(a)))

void *array_grow_func(void *a, size_t elSize, size_t wantedCap);
#define array_grow(a, cap) ((a) = array_grow_func((a), sizeof(*(a)), (cap)))

#define array_push(a, v)                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        if (array_len(a) == array_cap(a))                                                                              \
            array_grow(a, array_cap(a) + 1);                                                                           \
        (a)[array_len(a)] = v;                                                                                         \
        array_len(a)++;                                                                                                \
    } while (0)

#define array_pop(a) (a[--array_len(a)])

void *array_clone_func(void *a, size_t elSize);
#define array_clone(a) (array_clone_func(a, sizeof(*(a))))

#ifdef __cplusplus
}
#endif
