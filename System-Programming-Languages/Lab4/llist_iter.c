#include "llist_iter.h"

void list_foreach(llist* lst, void (*iter_fun) (int)) {
  while (lst != NULL) {
    iter_fun(lst->el);
    lst = lst->rest;
  }
}

void list_foreach_closure(llist* lst, void* locals, void (*iter_fun) (int, void*)) {
  while (lst != NULL) {
    iter_fun(lst->el, locals);
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

void list_map_mut(llist* lst, int (*transform_fun) (int)) {
  while (lst != NULL) {
    lst->el = transform_fun(lst->el);
    lst = lst->rest;
  }
}

int list_foldl(llist* lst, int acc, int (*acc_fun) (int, int)) {
  while (lst != NULL) {
    acc = acc_fun(lst->el, acc);
    lst = lst->rest;
  }

  return acc;
}

llist* list_iterate(int init, int iter_count, int (*iter_fun) (int)) {
  llist* lst = list_create(init);

  int acc = init;
  for (int i = 0; i < iter_count; acc = iter_fun(acc), i++)
    list_add_back(&lst, acc);
  
  return lst;
}
