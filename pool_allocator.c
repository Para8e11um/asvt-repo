#include "allocator.h"
#include <stdlib.h>

typedef struct Node { struct Node* next; } Node;
typedef struct {
    void* buffer;
    size_t block_size;
    Node* free_list;
} PoolCtx;

static void* pool_alloc_impl(IAllocator* self, size_t size){
    PoolCtx* ctx = (PoolCtx*)self->ctx;
    if (size > ctx->block_size || ctx->free_list==NULL){
        return NULL;
    }
    Node* free_node = ctx->free_list;
    ctx->free_list = free_node->next;
    return (void*)free_node;
}

static void pool_free_impl(IAllocator* self,void* ptr){
    PoolCtx* ctx = (PoolCtx*)self->ctx;
    Node* free_node = (Node*)ptr;
    free_node->next = ctx->free_list;
    ctx->free_list = free_node;
}

IAllocator create_pool_alloc(void* buffer, size_t size, size_t block_size){
    size_t aligned_block_size = (block_size + 7) & ~7;
    if (aligned_block_size < sizeof(Node)){
        aligned_block_size = sizeof(Node);
    }

    PoolCtx* ctx = malloc(sizeof(PoolCtx*));
    ctx->buffer = buffer;
    ctx->block_size = aligned_block_size;
    ctx->free_list = NULL;

    size_t block_amount = size / aligned_block_size;
    for(size_t i = 0; i < block_amount; i++){
        Node* node = (Node*)((char*)buffer + i * aligned_block_size);
        node->next = ctx->free_list;
        ctx->free_list = node;
    }


    IAllocator allocator;
    allocator.alloc = pool_alloc_impl;
    allocator.free = pool_free_impl;
    allocator.realloc = stub_realloc;
    allocator.reset = stub_reset;
    allocator.ctx = ctx;
    return allocator;
}
