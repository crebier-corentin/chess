#include "common.h"
#include "move.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LruNode LruNode;

typedef struct CacheEntry
{
    uint64_t key;
    double value;
    Move move;
    int depth;
    LruNode *node;
} CacheEntry;

typedef struct LruNode
{
    uint64_t key;
    LruNode *prev;
    LruNode *next;
} LruNode;

typedef struct Cache
{
    size_t max_len;
    size_t len;
    CacheEntry *hashmap;

    // first is most recently used
    LruNode *first;
    LruNode *last;
} Cache;

Cache cache_create();
void cache_free(Cache *cache);

CacheEntry *cache_get(Cache *cache, uint64_t key);
void cache_set(Cache *cache, CacheEntry entry);

#ifdef __cplusplus
}
#endif
