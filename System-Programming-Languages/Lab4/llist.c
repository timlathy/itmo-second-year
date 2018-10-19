#include "llist.h"

llist* list_create(int el) {
	llist* lst = NULL;
	list_add_front(&lst, el);
	return lst;
}

void list_add_front(llist** lst, int el) {
	llist* prev_head = *lst;
	*lst = (llist*) malloc(sizeof(llist));
	(*lst)->el = el;
	(*lst)->rest = prev_head;
}

void list_add_back(llist** lst, int el) {
  llist* last_node = list_last_node(*lst);
  last_node->rest = list_create(el);
}

llist* list_node_at(llist* lst, int index) {
  llist* curr_node = lst;

  for (int i = 0; i < index; i++) {
    if (curr_node == NULL) return NULL;
    curr_node = curr_node->rest;
  }

  return curr_node;
}

llist* list_last_node(llist* lst) {
  llist* last_node = lst;
  while (last_node->rest != NULL)
    last_node = last_node->rest;
  return last_node;
}

int list_get(llist* lst, int index) {
  llist* node_at_index = list_node_at(lst, index);
  return node_at_index == NULL ? 0 : node_at_index->el;
}

int list_length(llist* lst) {
  int length = 0;
  llist* curr_node = lst;
  while ((curr_node = curr_node->rest) != NULL) length++;
  return length;
}

void list_free(llist** lstptr) {
  list_free_preserve_ptr(*lstptr);
  *lstptr = NULL;
}

void list_free_preserve_ptr(llist* lst) {
  if (lst->rest != NULL) list_free_preserve_ptr(lst->rest);
  free(lst);
}
