#include "allocator.h"

typedef struct BlockHeader {
    size_t power;
    bool is_free;
    struct BlockHeader* next;
    struct BlockHeader* prev;
} BlockHeader;

typedef struct {
    void* buffer;
    size_t total_size;
    size_t min_power;
    size_t max_power;
    BlockHeader* free_lists[32];
} BuddyCtx;

static size_t size_to_power(size_t size){
    size_t power = 0;
    size_t p = 1;
    while(p < size){
        p <<= 1;
        power++;
    }
    return power;
}
