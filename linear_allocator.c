#include "allocator.h"
typedef struct {
    void* buffer;
    size_t size;
    size_t offset;
} LinearCtx;

static void* linear_alloc_impl(IAllocator* self, size_t size){
    LinearCtx* ctx = (LinearCtx*)self->ctx;
    size_t aligned_size = (size + 7) & ~7;
    if (ctx->offset+aligned_size > ctx->size){
        return NULL;
    }
    void* ptr = (char*)ctx->buffer + ctx->offset;
    ctx->offset += aligned_size;
    return ptr;
}
static void linear_reset_impl(IAllocator* self){
    LinearCtx* ctx = (LinearCtx*)self->ctx;
    ctx->offset = 0;
}

IAllocator create_linear_alloc(void* buffer, size_t size){
    LinearCtx* ctx = malloc(sizeof(LinearCtx*));
    ctx->buffer = buffer;
    ctx->size = size;
    ctx->offset = 0;

    IAllocator allocator;
    allocator.alloc = linear_alloc_impl;
    allocator.free = stub_free;
    allocator.realloc = stub_realloc;
    allocator.reset = linear_reset_impl;
    allocator.ctx = ctx;
    return allocator;
}
