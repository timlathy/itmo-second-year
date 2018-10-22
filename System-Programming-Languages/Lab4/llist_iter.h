#ifndef LLIST_ITER_H
#define LLIST_ITER_H

#include "llist.h"

void list_foreach(const llist* lst, void (*iter_fun) (int));

void list_foreach_ctx(const llist* lst, void* context, void (*iter_fun) (int, void*));

llist* list_map(const llist* lst, int (*transform_fun) (int));

void list_map_mut(llist* lst, int (*transform_fun) (int));

int list_foldl(const llist* lst, int acc, int (*acc_fun) (int, int));

llist* list_iterate(int init, int iter_count, int (*iter_fun) (int));

#endif
