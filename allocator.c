#include "allocator.h"
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

static void* sys_alloc_impl(IAllocator* self, size_t size) { return malloc(size); }
static void  sys_free_impl(IAllocator* self, void* ptr) { free(ptr); }
static void* sys_realloc_impl(IAllocator* self, void* ptr, size_t size) { return realloc(ptr, size); }

IAllocator create_sys_alloc(){
    IAllocator allocator;
    allocator.alloc = sys_alloc_impl;
    allocator.free = sys_free_impl;
    allocator.realloc = sys_realloc_impl;
    allocator.reset = stub_reset;
    allocator.ctx = NULL;
    return allocator;
}
