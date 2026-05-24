#include "allocator.h"

void stub_free(IAllocator *self, void *ptr) {
  (void)self;
  (void)ptr;
}
void *stub_realloc(IAllocator *self, void *ptr, size_t size) {
  (void)self;
  (void)ptr;
  (void)size;
  return NULL;
}
void stub_reset(IAllocator *self) { (void)self; }

//ОБОЛОЧКА malloc

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

//LINEAR-ALLOCATOR
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
    LinearCtx* ctx = malloc(sizeof(LinearCtx));
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

//POOL-ALLOCATOR

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

    PoolCtx* ctx = malloc(sizeof(PoolCtx));
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

//BUDDY-ALLOCATOR

static int size_to_order(size_t size) {
    int order = 0;
    size_t s = 1;
    while (s < size) {
        s <<= 1;
        order++;
    }
    return order;
}

static void remove_from_free_list(BuddyCtx* ctx, BlockHeader* block) {
    if (block->prev) block->prev->next = block->next;
    if (block->next) block->next->prev = block->prev;
    if (ctx->free_lists[block->order] == block) {
        ctx->free_lists[block->order] = block->next;
    }
    block->prev = block->next = NULL;
}

static void add_to_free_list(BuddyCtx* ctx, BlockHeader* block) {
    int order = block->order;
    block->next = ctx->free_lists[order];
    block->prev = NULL;
    if (ctx->free_lists[order]) {
        ctx->free_lists[order]->prev = block;
    }
    ctx->free_lists[order] = block;
    block->is_free = true;
}

static void* buddy_alloc_impl(IAllocator* self, size_t size) {
    BuddyCtx* ctx = (BuddyCtx*)self->ctx;

    size_t needed_size = size + sizeof(BlockHeader);
    int target_order = size_to_order(needed_size);
    if (target_order < ctx->min_order) target_order = ctx->min_order;
    if (target_order > ctx->max_order) return NULL;

    int current_order = target_order;
    while (current_order <= ctx->max_order && ctx->free_lists[current_order] == NULL) {
        current_order++;
    }

    if (current_order > ctx->max_order) return NULL;

    BlockHeader* block = ctx->free_lists[current_order];
    remove_from_free_list(ctx, block);

    while (current_order > target_order) {
        current_order--;
        size_t half_size = 1ULL << current_order;

        BlockHeader* buddy = (BlockHeader*)((char*)block + half_size);
        buddy->order = current_order;

        add_to_free_list(ctx, buddy);

        block->order = current_order;
    }

    block->is_free = false;
    return (void*)((char*)block + sizeof(BlockHeader));
}

static void buddy_free_impl(IAllocator* self, void* ptr) {
    BuddyCtx* ctx = (BuddyCtx*)self->ctx;

    BlockHeader* block = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
    block->is_free = true;

    while (block->order < ctx->max_order) {
        size_t offset = (char*)block - (char*)ctx->buffer;
        size_t buddy_offset = offset ^ (1ULL << block->order);
        BlockHeader* buddy = (BlockHeader*)((char*)ctx->buffer + buddy_offset);

        if (!buddy->is_free || buddy->order != block->order) {
            break;
        }

        remove_from_free_list(ctx, buddy);

        if (buddy < block) {
            block = buddy;
        }

        block->order++;
    }

    add_to_free_list(ctx, block);
}

IAllocator create_buddy_alloc(void* buffer, size_t size) {
    BuddyCtx* ctx = malloc(sizeof(BuddyCtx));
    ctx->buffer = buffer;
    ctx->total_size = size;

    ctx->max_order = 0;
    while ((1ULL << (ctx->max_order + 1)) <= size) ctx->max_order++;

    ctx->min_order = size_to_order(sizeof(BlockHeader) + 8);

    for (int i = 0; i < MAX_ORDERS; i++) ctx->free_lists[i] = NULL;

    BlockHeader* root = (BlockHeader*)buffer;
    root->order = ctx->max_order;
    add_to_free_list(ctx, root);

    IAllocator a;
    a.alloc = buddy_alloc_impl;
    a.free = buddy_free_impl;
    a.realloc = stub_realloc;
    a.reset = stub_reset;
    a.ctx = ctx;
    return a;
}

//ARRAY-LIST


void array_list_init(ArrayList* list, IAllocator* alloc, size_t capacity){
    list->alloc = alloc;
    list->size = 0;
    list-> capacity = capacity;
    list->data = (int*)i_alloc(alloc, capacity*sizeof(int));
}

void array_list_push(ArrayList* list, int value){
    if (list->size == list->capacity){
        list->capacity *= 2;
        int* new_data = (int*)i_alloc(list->alloc,list->capacity*sizeof(int));
        for (size_t i = 0; i < list->size;i++){
            new_data[i] = list->data[i];
        }
        i_free(list->alloc,list->data);
        list->data = new_data;
    }
    list->data[list->size++] = value;
}

void array_list_free(ArrayList* list){
    i_free(list->alloc, list->data);
    list->size = 0;
    list->capacity = 0;
}

//LINKED-LIST

void queue_init(Queue* q, IAllocator* alloc){
    q->alloc = alloc;
    q->head = NULL;
    q->tail = NULL;
}

void queue_push(Queue*q,int value){
    QueueNode* node = (QueueNode*)i_alloc(q->alloc, sizeof(QueueNode));
    node->next = NULL;
    node->value = value;
    if (q->head == NULL){
        q->head = node;
        q->tail = node;
        return;
    }
    q->tail->next = node;
    q->tail = node;
}

int queue_pop(Queue*q){
    if(q->head == NULL){
        return -1;
    }
    QueueNode* node = q->head;
    q->head = node->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    int value = node->value;
    i_free(q->alloc,node);
    return value;
}

//HASHTABLE

static size_t hash_djb2(const char* str) {
    size_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

void hash_table_init(HashTable* table, IAllocator* alloc, size_t capacity) {
    table->alloc = alloc;
    table->capacity = capacity;
    table->size = 0;

    table->buckets = (HashNode**)i_alloc(alloc, capacity * sizeof(HashNode*));

    if (table->buckets) {
        for (size_t i = 0; i < capacity; i++) {
            table->buckets[i] = NULL;
        }
    }
}

void hash_table_insert(HashTable* table, const char* key, int value) {

    size_t index = hash_djb2(key) % table->capacity;
    HashNode* current = table->buckets[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }

    HashNode* new_node = (HashNode*)i_alloc(table->alloc, sizeof(HashNode));
    if (!new_node) return;

    new_node->key = key;
    new_node->value = value;

    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;

    table->size++;
}

bool hash_table_get(HashTable* table, const char* key, int* out_value) {
    size_t index = hash_djb2(key) % table->capacity;
    HashNode* current = table->buckets[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            *out_value = current->value;
            return true; // Нашли!
        }
        current = current->next;
    }
    return false;
}

bool hash_table_remove(HashTable* table, const char* key) {
    if (!table->buckets) return false;

    size_t index = hash_djb2(key) % table->capacity;
    HashNode* current = table->buckets[index];
    HashNode* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                table->buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }

            i_free(table->alloc, current);
            table->size--;
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;
}

void hash_table_free(HashTable* table) {
    if (!table->buckets) return;

    for (size_t i = 0; i < table->capacity; i++) {
        HashNode* current = table->buckets[i];
        while (current != NULL) {
            HashNode* next = current->next;
            i_free(table->alloc, current);
            current = next;
        }
    }
    i_free(table->alloc, table->buckets);
    table->buckets = NULL;
    table->size = 0;
    table->capacity = 0;
}
