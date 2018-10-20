#ifndef LLIST_ITER_H
#define LLIST_ITER_H

#include "llist.h"

void list_foreach(llist* lst, void (*iter_fun) (int));

void list_foreach_closure(llist* lst, void* locals, void (*iter_fun) (int, void*));

llist* list_map(llist* lst, int (*transform_fun) (int));

void list_map_mut(llist* lst, int (*transform_fun) (int));

int list_foldl(llist* lst, int acc, int (*acc_fun) (int, int));

#endif
