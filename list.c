#include "list.h"
#include <stdio.h>
#include <stdlib.h>
int list_add_tail(List *list, int val) {
  if (list->tail == NULL) {
    return list_add_head(list, val);
  }
  ListNode *node = (ListNode *)malloc(sizeof(ListNode));
  node->data = val;
  node->next = NULL;
  list->tail->next = node;
  list->tail = node;
  return 0;
}

int list_add_head(List *list, int val) {
  ListNode *node = (ListNode *)malloc(sizeof(ListNode));

  node->data = val;
  node->next = list->head;
  list->head = node;
  if (list->tail == NULL) {
    list->tail = node;
  }
  return 0;
}

int list_add_middle(List *list, int val, int index) {
  ListNode *pointer = list->head;
  for (int i = 0; i < index - 1; i++) {
    pointer = pointer->next;
  }
  ListNode *node = (ListNode *)malloc(sizeof(ListNode));
  node->data = val;
  node->next = pointer->next;
  pointer->next = node;
  if (node->next == NULL) {
    list->tail = node;
  }
  return 0;
}

int list_init(List *list) {
  list->head = NULL;
  list->tail = NULL;
  return 0;
}

int list_print(List *list) {
  ListNode *pointer = list->head;
  while (pointer != NULL) {
    printf("%d ", pointer->data);
    pointer = pointer->next;
  }
  printf("\n");
  return 0;
}

int list_rm(List *list, int index) {
  if (index == 0) {
    ListNode *temp = list->head;
    list->head = list->head->next;
    free(temp);
    if (list->head == NULL) {
      list->tail = NULL;
    }
    return 0;
  } else {
    ListNode *pointer = list->head;
    for (int i = 0; i < index - 1; i++) {
      pointer = pointer->next;
    }
    ListNode *temp = pointer->next;
    pointer->next = temp->next;
    free(temp);
    if (pointer->next == NULL) {
      list->tail = pointer;
    }
    return 0;
  }
}

int list_index(List *list, int index) {
  ListNode *pointer = list->head;
  for (int i = 0; i < index; i++) {
    pointer = pointer->next;
  }
  return pointer->data;
}

int list_len(List *list) {
  ListNode *pointer = list->head;
  int count = 0;
  while (pointer != NULL) {
    count++;
    pointer = pointer->next;
  }
  return count;
}
