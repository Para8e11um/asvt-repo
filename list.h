typedef struct ListNode {
  int data;
  struct ListNode *next;
} ListNode;

typedef struct List {
  ListNode *head;
  ListNode *tail;
} List;

int list_add_head(List *list, int val);
int list_add_tail(List *list, int val);
int list_add_middle(List *list, int val, int index);
int list_init(List *list);
int list_print(List *list);
int list_rm(List *list, int index);
int list_index(List *list, int index);
int list_len(List *list);
