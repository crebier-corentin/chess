#include "cache.h"
#include <assert.h>
#include <siphash.h>
#include <stb_ds.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef DEBUG_CACHE
uint64_t col = 0;
uint64_t new = 0;
uint64_t update = 0;
uint64_t miss = 0;
uint64_t hit = 0;
#endif

Cache cache_create()
{
    Cache c = {0};
    c.cap = (268435456LLU * 4) / sizeof(CacheEntry); // 256mb
    // c.cap = 256000;
    c.entries = calloc(c.cap, sizeof(CacheEntry));
    return c;
}
void cache_free(Cache *cache)
{
    free(cache->entries);
    cache->entries = NULL;
}

static const uint8_t siphash_key[] = {0xee, 0xb1, 0x87, 0x72, 0x5c, 0x81, 0x79, 0x42,
                                      0xde, 0x73, 0x2c, 0xae, 0x90, 0x14, 0x9b, 0xf4};

static uint64_t hash_key(uint64_t key)
{
    return key;
    //  uint64_t hash;
    // siphash(&key, sizeof(key), &siphash_key, &hash, sizeof(hash));
    // return hash;
}

CacheEntry *cache_get(Cache *cache, uint64_t key)
{
    CacheEntry *entry = &cache->entries[hash_key(key) % cache->cap];
    if (!entry->is_set || entry->key != key)
    {
#ifdef DEBUG_CACHE
        miss++;
#endif
        return NULL;
    }
#ifdef DEBUG_CACHE
    hit++;
#endif

    return entry;
}

void cache_set(Cache *cache, CacheEntry entry)
{
    CacheEntry *cache_entry = &cache->entries[hash_key(entry.key) % cache->cap];

#ifdef DEBUG_CACHE
    if (cache_entry->is_set)
    {
        if (cache_entry->key == entry.key)
        {
            update++;
        }
        else
        {

            col++;
        }
    }
    else
    {
        new ++;
    }
#endif

    cache_entry->is_set = true;
    cache_entry->key = entry.key;
    cache_entry->type = entry.type;
    cache_entry->value = entry.value;
    cache_entry->move = entry.move;
    cache_entry->depth = entry.depth;
}

void cache_print_debug(Cache *cache)
{
#ifdef DEBUG_CACHE
    printf("Hit: %llu, Miss: %llu\n", hit, miss);
    printf("New: %llu, Update: %llu, Collision: %llu\n", new, update, col);

    uint64_t count = 0;
    for (size_t i = 0; i < cache->cap; i++)
    {
        if (cache->entries[i].is_set)
        {
            count++;
        }
    }
    printf("Used: %llu/%llu (Unused: %llu) Fill rate: %f %%\n", count, cache->cap, cache->cap - count,
           ((double)count) / ((double)cache->cap) * 100.0);
#endif
}
