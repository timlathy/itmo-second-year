#include <stdio.h>

#include "alloc.h"
#include "alloc_debug.h"

int main(int argc, char** argv) {
  chunk_head_t* heap_start = (chunk_head_t*) heap_init();
  puts("~~~ empty heap:"); heap_full_dump(stdout, heap_start, 16);
  char* p1 = heap_alloc(4096);
  for (size_t i = 0; i < 4096; i++) p1[i] = i + 1;
  puts("~~~ p1 = heap_alloc(4096):"); heap_full_dump(stdout, heap_start, 16);
  char* p2 = heap_alloc(64);
  for (size_t i = 0; i < 64; i++) p2[i] = 64 - i;
  puts("~~~ p2 = heap_alloc(64):"); heap_full_dump(stdout, heap_start, 16);
  heap_free(p1);
  puts("~~~ heap_free(p1)"); heap_full_dump(stdout, heap_start, 16);
  return 0;
}
