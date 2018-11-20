#include <stdio.h>

#include "alloc.h"
#include "alloc_debug.h"

int main(int argc, char** argv) {
  chunk_head_t* heap_start = (chunk_head_t*) heap_init();

  puts("~~~ empty heap:"); heap_full_dump(stdout, heap_start, 16);
  char* p1 = heap_alloc(4096);
  for (size_t i = 0; i < 4096; i++) p1[i] = i + 1;
  printf("~~~ p1 = %p = heap_alloc(4096):\n", p1); heap_full_dump(stdout, heap_start, 16);
  char* p2 = heap_alloc(64);
  for (size_t i = 0; i < 64; i++) p2[i] = 64 - i;
  printf("~~~ p2 = %p = heap_alloc(64):\n", p2); heap_full_dump(stdout, heap_start, 16);
  heap_free(p1);
  puts("~~~ heap_free(p1)"); heap_full_dump(stdout, heap_start, 0);
  char* p3 = heap_alloc(4 * 1024 * 1024);
  printf("~~~ p3 = %p = heap_alloc(4 * 1024 * 1024)\n", p3); heap_full_dump(stdout, heap_start, 0);

  heap_free(p2);
  puts("~~~ heap_free(p2)"); heap_full_dump(stdout, heap_start, 0);
  char* p4 = heap_alloc(6144);
  printf("~~~ p4 = %p = heap_alloc(6144)\n", p4); heap_full_dump(stdout, heap_start, 0);

  return 0;
}
