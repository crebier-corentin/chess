#include "common.h"
#include "move.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CacheEntryType
{
    CacheEntryType_EXACT,
    CacheEntryType_LOWERBOUND,
    CacheEntryType_UPPERBOUND,
} CacheEntryType;

typedef struct CacheEntry
{
    bool is_set;
    uint64_t key;
    CacheEntryType type;
    double value;
    Move move;
    int depth;
} CacheEntry;

typedef struct Cache
{
    size_t cap;
    CacheEntry *entries;
} Cache;

Cache cache_create();
void cache_free(Cache *cache);

CacheEntry *cache_get(Cache *cache, uint64_t key);
void cache_set(Cache *cache, CacheEntry entry);

#ifdef __cplusplus
}
#endif
