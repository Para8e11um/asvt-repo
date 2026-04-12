#include "stack.h"
#include <stdbool.h>
#include <stdlib.h>

int stack_init(Stack *stack, int capacity) {
  stack->data = (int *)malloc(capacity * sizeof(int));
  stack->top = -1;
  stack->capacity = capacity;
  return 0;
}
int stack_push(Stack *stack, int val) {
  if (stack->top == stack->capacity - 1) {
    return -1;
  }
  stack->top++;
  stack->data[stack->top] = val;
  return 0;
}
int stack_pop(Stack *stack) {
  if (stack->top == -1) {
    return -1;
  }
  int val = stack->data[stack->top];
  stack->top--;
  return val;
}
bool stack_empty_check(Stack *stack) { return stack->top == -1; }
