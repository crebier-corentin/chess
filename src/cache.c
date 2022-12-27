#include "cache.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

Cache cache_create(void)
{
    Cache c = {0};
    c.cap = (268435456LLU * 4) / sizeof(CacheEntry); // 1Gb
    c.entries = calloc(c.cap, sizeof(CacheEntry));
    return c;
}
void cache_free(Cache *cache)
{
    free(cache->entries);
    cache->entries = NULL;
}

CacheEntry *cache_get(Cache *cache, uint64_t key)
{
    CacheEntry *entry = &cache->entries[key % cache->cap];
    if (!entry->is_set || entry->key != key)
    {

        return NULL;
    }

    return entry;
}

void cache_set(Cache *cache, CacheEntry entry)
{
    CacheEntry *cache_entry = &cache->entries[entry.key % cache->cap];

    cache_entry->is_set = true;
    cache_entry->key = entry.key;
    cache_entry->type = entry.type;
    cache_entry->value = entry.value;
    cache_entry->move = entry.move;
    cache_entry->depth = entry.depth;
}
