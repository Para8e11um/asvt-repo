#include <stdbool.h>
typedef struct Stack {
  int *data;
  int top;
  int capacity;
} Stack;
int stack_init(Stack *stack, int capacity);
int stack_push(Stack *stack, int val);
int stack_pop(Stack *stack);
bool stack_empty_check(Stack *stack);
