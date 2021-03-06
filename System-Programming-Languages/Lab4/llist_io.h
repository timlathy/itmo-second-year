#ifndef LLIST_IO_H
#define LLIST_IO_H

#include <stdio.h>
#include "llist.h"
#include "llist_iter.h"

bool list_save(const llist* lst, const char* filename);

bool list_load(llist** lst, const char* filename);

void list_read_stdin(llist** lst);

bool list_serialize(const llist* lst, const char* filename);

bool list_deserialize(llist** lst, const char* filename);

#endif
