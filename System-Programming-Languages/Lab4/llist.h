#ifndef LLIST_H
#define LLIST_H

#include <stdlib.h>

typedef struct llist llist;

struct llist {
  int el;
  llist* rest;
};

llist* list_create(int el);

void list_add_front(llist** lst, int el);

void list_add_back(llist** lst, int el);

llist* list_node_at(llist* lst, int index);

llist* list_last_node(llist* lst);

int list_get(llist* lst, int index);

int list_length(llist* lst);

void list_free_preserve_ptr(llist* lst);

void list_free(llist** lstptr);

#endif
