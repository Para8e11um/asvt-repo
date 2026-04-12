#include "list.h"
#include <assert.h>

void list_test_1() {
  List list;
  list_init(&list);
  for (int i = 0; i < 10; i++) {
    list_add_tail(&list, i);
  }
  list_add_middle(&list, 100, 4);
  list_rm(&list, 4);
  assert(list_index(&list, 4) == 4);
}

void list_test_2() {
  List list;
  list_init(&list);
  list_add_head(&list, 1);
  list_rm(&list, 0);
  assert(list_len(&list) == 0);
}

void list_test_3() {
  List list;
  list_init(&list);
  list_add_head(&list, 1);
  list_add_tail(&list, 2);
  list_add_middle(&list, 3, 1);
  assert(list_index(&list, 0) == 1);
  assert(list_index(&list, 1) == 3);
  assert(list_index(&list, 2) == 2);
}

void list_test_4() {
  List list;
  list_init(&list);
  for (int i = 0; i < 10; i++) {
    list_add_tail(&list, i);
  }
  assert(list_index(&list, 0) == 0);
  assert(list_index(&list, 9) == 9);
  assert(list_len(&list) == 10);
}

void list_test_5() {
  List list;
  list_init(&list);
  for (int i = 0; i < 10; i++) {
    list_add_tail(&list, i);
  }
  list_rm(&list, 0);
  assert(list_index(&list, 0) == 1);
  assert(list_index(&list, 8) == 9);
  assert(list_len(&list) == 9);
}

void list_test_6() {
  List list;
  list_init(&list);
  list_add_head(&list, 10);
  for (int i = 0; i < 10; i++) {
    list_add_middle(&list, i, 0);
  }
  assert(list_index(&list, 0) == 10);
  assert(list_index(&list, 3) == 7);
  assert(list_index(&list, 9) == 1);
  assert(list_len(&list) == 11);
}

int main() {
  list_test_1();
  list_test_2();
  list_test_3();
  list_test_4();
  list_test_5();
  list_test_6();
  return 0;
}
