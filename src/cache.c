#include "cache.h"
#include <assert.h>
#include <stb_ds.h>

Cache cache_create()
{
    Cache c = {0};
    c.max_len = 268435456 / sizeof(CacheEntry); // 256mb
    return c;
}
void cache_free(Cache *cache)
{
    hmfree(cache->hashmap);

    LruNode *node = cache->first;
    while (node != NULL)
    {
        LruNode *next_node = node->next;
        free(node);
        node = next_node;
    }
    cache->first = NULL;
    cache->last = NULL;
    cache->len = 0;
}

static void move_to_front(Cache *cache, LruNode *node)
{
    // Already in front
    if (cache->first == node)
        return;

    LruNode *prev = node->prev;
    LruNode *next = node->next;

    if (prev != NULL)
    {
        prev->next = next;
    }
    if (next != NULL)
    {
        next->prev = prev;
    }

    if (cache->last == node)
    {
        cache->last = node->prev;
    }
    if (cache->first != NULL)
    {
        cache->first->prev = node;
    }

    node->prev = NULL;
    node->next = cache->first;
    cache->first = node;
}

static LruNode *delete_last(Cache *cache)
{
    assert(cache->len > 0);
    assert(cache->last != NULL);

    LruNode *prev = cache->last->prev;
    if (prev != NULL)
    {
        prev->next = NULL;
    }

    if (cache->first == cache->last)
    {
        cache->first = NULL;
    }

    hmdel(cache->hashmap, cache->last->key);

    LruNode *node = cache->last;

    cache->last = prev;
    cache->len--;

    return node;
}

CacheEntry *cache_get(Cache *cache, uint64_t key)
{
    CacheEntry *entry = hmgetp_null(cache->hashmap, key);

    // Move to front
    if (entry != NULL)
    {
        move_to_front(cache, entry->node);
    }

    return entry;
}

void cache_set(Cache *cache, CacheEntry entry)
{
    CacheEntry *existing_entry = hmgetp_null(cache->hashmap, entry.key);

    // Already exists update it
    if (existing_entry != NULL)
    {
        existing_entry->value = entry.value;
        existing_entry->move = entry.move;
        existing_entry->depth = entry.depth;

        move_to_front(cache, existing_entry->node);
    }
    // Create new entry
    else
    {
        if (cache->len == cache->max_len)
        {
            entry.node = delete_last(cache);
        }
        else
        {
            entry.node = malloc(sizeof(LruNode));
        }

        entry.node->key = entry.key;

        if (cache->first != NULL)
        {
            cache->first->prev = entry.node;
        }
        entry.node->next = cache->first;
        cache->first = entry.node;

        if (cache->last == NULL)
        {
            cache->last = entry.node;
        }

        hmputs(cache->hashmap, entry);

        cache->len++;
    }
}
