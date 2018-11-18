#include <stdio.h>

#include "alloc.h"

int main(int argc, char** argv) {
  heap_init();
  void* p1 = heap_alloc(4096 - sizeof(chunk_head_t));
  void* p2 = heap_alloc(64);
  printf("p1: %p, p2: %p\n", p1, p2);
  return 0;
}
