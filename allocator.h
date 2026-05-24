#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
typedef struct IAllocator IAllocator;
#define MAX_ORDERS 32

// ИНТЕРФЕЙС АЛЛОКАТОРА И ЗАГЛУШКИ
typedef struct IAllocator {
  void *(*alloc)(struct IAllocator *self, size_t size);
  void (*free)(struct IAllocator *self, void *ptr);
  void *(*realloc)(struct IAllocator *self, void *ptr, size_t new_size);
  void (*reset)(struct IAllocator *self);
  void *ctx;
} IAllocator;

void stub_free(IAllocator *self, void *ptr);
void *stub_realloc(IAllocator *self, void *ptr, size_t size);
void stub_reset(IAllocator *self);

// inline-ФУНКЦИИ АЛЛОКАЦИИ
static inline void *i_alloc(IAllocator *a, size_t sz) {
  return a->alloc(a, sz);
}
static inline void i_free(IAllocator *a, void *ptr) { a->free(a, ptr); }
static inline void *i_realloc(IAllocator *a, void *ptr, size_t new_size) {
  return a->realloc(a, ptr, new_size);
}
static inline void i_reset(IAllocator *a, size_t sz) { a->reset(a); }

// ОБОЛОЧКА malloc
static void *sys_alloc_impl(IAllocator *self, size_t size);
static void sys_free_impl(IAllocator *self, void *ptr);
static void *sys_realloc_impl(IAllocator *self, void *ptr, size_t size);

IAllocator create_sys_alloc();

// LINEAR-ALLOCATOR
typedef struct {
  void *buffer;
  size_t size;
  size_t offset;
} LinearCtx;

static void *linear_alloc_impl(IAllocator *self, size_t size);
static void linear_reset_impl(IAllocator *self);
IAllocator create_linear_alloc(void *buffer, size_t size);

// POOL-ALLOCATOR

typedef struct Node {
  struct Node *next;
} Node;
typedef struct {
  void *buffer;
  size_t block_size;
  Node *free_list;
} PoolCtx;

static void *pool_alloc_impl(IAllocator *self, size_t size);
static void pool_free_impl(IAllocator *self, void *ptr);
IAllocator create_pool_alloc(void *buffer, size_t size, size_t block_size);

// BUDDY-ALLOCATOR

typedef struct BlockHeader {
  int order;
  bool is_free;
  struct BlockHeader *next;
  struct BlockHeader *prev;
} BlockHeader;

typedef struct {
  void *buffer;
  size_t total_size;
  int min_order;
  int max_order;
  BlockHeader *free_lists[MAX_ORDERS];
} BuddyCtx;

static int size_to_order(size_t size);
static void remove_from_free_list(BuddyCtx *ctx, BlockHeader *block);
static void add_to_free_list(BuddyCtx *ctx, BlockHeader *block);
static void *buddy_alloc_impl(IAllocator *self, size_t size);
static void buddy_free_impl(IAllocator *self, void *ptr);
IAllocator create_buddy_alloc(void *buffer, size_t size);

// ARRAY-LIST
typedef struct {
  IAllocator *alloc;
  size_t size;
  size_t capacity;
  int *data;
} ArrayList;

void array_list_init(ArrayList *list, IAllocator *alloc, size_t capacity);
void array_list_push(ArrayList *list, int value);
void array_list_free(ArrayList *list);

// LINKED-LIST

typedef struct {
  int value;
  void *next;
} QueueNode;

typedef struct {
  IAllocator *alloc;
  QueueNode *head;
  QueueNode *tail;
} Queue;

void queue_init(Queue *q, IAllocator *alloc);
void queue_push(Queue *q, int value);
int queue_pop(Queue *q);

// HASHTABLE

typedef struct HashNode {
  const char *key;
  int value;
  struct HashNode *next;
} HashNode;

typedef struct {
  IAllocator *alloc;
  HashNode **buckets;
  size_t capacity;
  size_t size;
} HashTable;

void hash_table_init(HashTable *table, IAllocator *alloc, size_t capacity);
void hash_table_insert(HashTable *table, const char *key, int value);
bool hash_table_get(HashTable *table, const char *key, int *out_value);
bool hash_table_remove(HashTable *table, const char *key);
void hash_table_free(HashTable *table);
