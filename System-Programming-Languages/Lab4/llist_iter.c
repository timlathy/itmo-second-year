#include "llist_iter.h"

void list_foreach(llist* lst, void (*iter_fun) (int)) {
  while (lst != NULL) {
    iter_fun(lst->el);
    lst = lst->rest;
  }
}

llist* list_map(llist* lst, int (*transform_fun) (int)) {
  llist* new_lst = NULL;

  while (lst != NULL) {
    list_add_front(&new_lst, transform_fun(lst->el));
    lst = lst->rest;
  }

  return new_lst;
}

int list_foldl(llist* lst, int acc, int (*acc_fun) (int, int)) {
  while (lst != NULL) {
    acc = acc_fun(lst->el, acc);
    lst = lst->rest;
  }

  return acc;
}
