#include "llist_io.h"

void write_element(int el, void* dstfile) {
  fprintf(dstfile, "%d\n", el);
}

bool list_save(llist* lst, const char* filename) {
  FILE* dst = fopen(filename, "w");
  if (dst == NULL) return false;

  list_foreach_closure(lst, dst, write_element);

  return fclose(dst) == 0;
}

bool list_load(llist** lst, const char* filename) {
  FILE* src = fopen(filename, "r");
  if (src == NULL) return false;

  int el;
  while (fscanf(src, "%d", &el) != EOF) {
    list_add_back(lst, el);
  }

  return fclose(src) == 0;
}

void list_read_stdin(llist** lst) {
  int el;
  while (scanf("%d", &el) != EOF)
    list_add_front(lst, el);
}

bool list_serialize(llist* lst, const char* filename) {
  FILE* dst = fopen(filename, "wb");
  if (dst == NULL) return false;

  int len = list_length(lst);
  int* buf = (int*) malloc(len * sizeof(int));

  for (int buf_iter = 0; lst != NULL; buf_iter++) {
    buf[buf_iter] = lst->el;
    lst = lst->rest;
  }

  return fwrite(buf, len, sizeof(int), dst) != 0 && fclose(dst) == 0;
}

bool list_deserialize(llist** lst, const char* filename) {
  FILE* src = fopen(filename, "rb");
  if (src == NULL) return false;

  fseek(src, 0, SEEK_END);
  long buf_size = ftell(src);
  rewind(src);

  if (buf_size % sizeof(int) != 0) return false;

  int* buf = (int*) malloc(buf_size);
  fread(buf, buf_size, sizeof(int), src);
  
  for (int i = 0; i < buf_size / sizeof(int); i++)
    list_add_front(lst, buf[i]);

  return true;
}
