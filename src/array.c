#include "array.h"
#include <string.h>

void *array_create_size_func(size_t elSize, size_t cap)
{
    ArrayHeader *header = malloc(sizeof(ArrayHeader) + elSize * cap);
    header->len = 0;
    header->cap = cap;
    return header + 1;
}

void *array_grow_func(void *a, size_t elSize, size_t wantedCap)
{
    ArrayHeader *header = array_header(a);
    size_t newCap = header->cap * 2;
    if (newCap < wantedCap)
    {
        newCap = wantedCap;
    }
    ArrayHeader *newHeader = realloc(header, sizeof(ArrayHeader) + elSize * newCap);
    newHeader->cap = newCap;
    return newHeader + 1;
}

void *array_clone_func(void *a, size_t elSize)
{
    void *copy = array_create_size_func(elSize, array_len(a));
    array_len(copy) = array_len(a);

    memcpy(copy, a, elSize * array_len(a));

    return copy;
}
