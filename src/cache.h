#include "common.h"
#include "move.h"
#include <assert.h>
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
    uint64_t key;
    double value;
    int depth;
    Move move;
    uint8_t type;
    bool is_set;
} CacheEntry;

typedef struct Cache
{
    size_t cap;
    CacheEntry *entries;
} Cache;

Cache cache_create(void);
void cache_free(Cache *cache);

CacheEntry *cache_get(Cache *cache, uint64_t key);
void cache_set(Cache *cache, CacheEntry entry);

#ifdef __cplusplus
}
#endif
