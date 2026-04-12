#include "stack.h"
#include <assert.h>

void stack_test_1() {
  Stack stack;
  stack_init(&stack, 5);
  assert(stack_empty_check(&stack));
}

void stack_test_2() {
  Stack stack;
  stack_init(&stack, 5);
  stack_push(&stack, 1);
  assert(stack_empty_check(&stack) == false);
  assert(stack_pop(&stack) == 1);
  assert(stack_empty_check(&stack));
}

void stack_test_3() {
  Stack stack;
  stack_init(&stack, 5);
  stack_push(&stack, 1);
  stack_push(&stack, 2);
  assert(stack_pop(&stack) == 2);
  assert(stack_pop(&stack) == 1);
  stack_push(&stack, 3);
  assert(stack_pop(&stack) == 3);
  assert(stack_empty_check(&stack));
}

int main() {
  stack_test_1();
  stack_test_2();
  stack_test_3();
  return 0;
}
