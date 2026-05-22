#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct IAllocator IAllocator;

typedef struct IAllocator {
    void* (*alloc)(struct IAllocator* self, size_t size);
    void  (*free)(struct IAllocator* self, void* ptr);
    void* (*realloc)(struct IAllocator* self, void* ptr, size_t new_size);
    void  (*reset)(struct IAllocator* self);
    void* ctx;
} IAllocator;


void stub_free(IAllocator* self, void* ptr){
    (void)self; (void)ptr;
}
void* stub_realloc(IAllocator* self, void* ptr, size_t size){
    (void)self; (void)ptr; (void)size;
    return NULL;
}
void stub_reset(IAllocator* self){
    (void)self;
}


static inline void* i_alloc(IAllocator* a, size_t sz) {
    return a->alloc(a, sz);
}
static inline void i_free(IAllocator* a, void* ptr) {
    a->free(a, ptr);
}
static inline void* i_realloc(IAllocator* a, void* ptr, size_t new_size) {
    return a->realloc(a, ptr, new_size);
}
static inline void i_reset(IAllocator* a, size_t sz) {
    a->reset(a);
}

static void* sys_alloc_impl(IAllocator* self, size_t size);
static void  sys_free_impl(IAllocator* self, void* ptr);
static void* sys_realloc_impl(IAllocator* self, void* ptr, size_t size);

IAllocator create_sys_alloc();
