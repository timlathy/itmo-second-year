#include "llist.h"

llist* list_create(int el) {
  llist* lst = (llist*) malloc(sizeof(llist));
  lst->el = el;
  lst->rest = NULL;
  return lst;
}

void list_add_front(llist** lst, int el) {
  if (*lst == NULL) {
    *lst = list_create(el);
    return;
  }

  llist* prev_head = *lst;
  *lst = (llist*) malloc(sizeof(llist));
  (*lst)->el = el;
  (*lst)->rest = prev_head;
}

void list_add_back(llist** lst, int el) {
  if (*lst == NULL) {
    *lst = list_create(el);
    return;
  }

  llist* last_node = list_last_node(*lst);
  last_node->rest = list_create(el);
}

llist* list_node_at(const llist* lst, int index) {
  const llist* curr_node = lst;

  for (int i = 0; i < index; i++) {
    if (curr_node == NULL) return NULL;
    curr_node = curr_node->rest;
  }

  return (llist*) curr_node;
}

llist* list_last_node(const llist* lst) {
  const llist* last_node = lst;

  while (last_node->rest != NULL)
    last_node = last_node->rest;

  return (llist*) last_node;
}

int list_get(const llist* lst, int index) {
  llist* node_at_index = list_node_at(lst, index);
  return node_at_index == NULL ? 0 : node_at_index->el;
}

int list_length(const llist* lst) {
  int len;
  for (len = 0; lst != NULL; len++) lst = lst->rest;
  return len;
}

void list_free(llist** lstptr) {
  list_free_preserve_ptr(*lstptr);
  *lstptr = NULL;
}

void list_free_preserve_ptr(llist* lst) {
  if (lst->rest != NULL) list_free_preserve_ptr(lst->rest);
  free(lst);
}

bool list_compare(const llist* a, const llist* b) {
  while (a != NULL && b != NULL) {
    if (a->el != b->el) return false;
    a = a->rest;
    b = b->rest;
  }
  return a == NULL && b == NULL;
}
