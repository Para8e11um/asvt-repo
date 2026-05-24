#include "allocator.h"
#include <assert.h>
#include <stdio.h>

void test_buddy_allocator() {
  size_t mem_size = 4096;
  void *memory = malloc(mem_size);
  IAllocator alloc = create_buddy_alloc(memory, mem_size);

  void *ptr1 = i_alloc(&alloc, 100);
  assert(ptr1 != NULL);

  void *ptr2 = i_alloc(&alloc, 200);
  assert(ptr2 != NULL);

  void *huge_ptr = i_alloc(&alloc, 4000);
  assert(huge_ptr == NULL);

  i_free(&alloc, ptr1);
  i_free(&alloc, ptr2);

  void *root_ptr = i_alloc(&alloc, 3000);
  assert(root_ptr != NULL);

  i_free(&alloc, root_ptr);
  free(memory);
  free(alloc.ctx);
}

void test_data_structures_with_buddy() {
  size_t mem_size = 8192;
  void *memory = malloc(mem_size);
  IAllocator alloc = create_buddy_alloc(memory, mem_size);

  ArrayList list;
  array_list_init(&list, &alloc, 2);

  array_list_push(&list, 10);
  array_list_push(&list, 20);
  array_list_push(&list, 30);

  assert(list.size == 3);
  assert(list.capacity == 4);
  assert(list.data[2] == 30);
  array_list_free(&list);

  Queue q;
  queue_init(&q, &alloc);
  queue_push(&q, 100);
  queue_push(&q, 200);

  assert(queue_pop(&q) == 100);
  assert(queue_pop(&q) == 200);
  assert(queue_pop(&q) == -1);

  free(memory);
  free(alloc.ctx);
}

void test_hash_table() {
  size_t mem_size = 8192;
  void *memory = malloc(mem_size);
  IAllocator alloc = create_buddy_alloc(memory, mem_size);

  HashTable ht;

  hash_table_init(&ht, &alloc, 16);
  assert(ht.buckets != NULL);

  hash_table_insert(&ht, "apple", 100);
  hash_table_insert(&ht, "banana", 200);

  int val;
  assert(hash_table_get(&ht, "apple", &val) == true);
  assert(val == 100);

  assert(hash_table_get(&ht, "banana", &val) == true);
  assert(val == 200);

  assert(hash_table_get(&ht, "cherry", &val) == false);

  hash_table_insert(&ht, "apple", 999);
  assert(hash_table_get(&ht, "apple", &val) == true);
  assert(val == 999);
  assert(ht.size == 2);

  assert(hash_table_remove(&ht, "apple") == true);
  assert(hash_table_get(&ht, "apple", &val) == false);
  assert(ht.size == 1);

  hash_table_free(&ht);

  assert(ht.buckets == NULL);
  assert(ht.size == 0);

  void *big_chunk = i_alloc(&alloc, 7000);
  assert(big_chunk != NULL);
  i_free(&alloc, big_chunk);

  free(memory);
  free(alloc.ctx);
}

int main() {
  test_buddy_allocator();
  test_data_structures_with_buddy();
  test_hash_table();
  return 0;
}
