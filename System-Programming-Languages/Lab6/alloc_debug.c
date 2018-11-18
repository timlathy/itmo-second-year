#include <stdio.h>

#include "alloc_debug.h"

void heap_dump_chunk(FILE* out, const chunk_head_t* ptr, size_t chunk_content_bytes_printed) {
  fprintf(out, "[%p] { next: %p, capacity: %lu, is_free: %s }",
      (void*) ptr, (void*) ptr->next, ptr->capacity, ptr->is_free ? "true" : "false");

  char* contents = (char*) (ptr + 1);

  for (size_t i = 0; i < chunk_content_bytes_printed; i++) {
    if (i % 16 == 0)
      fprintf(out, "\n[%p]", (void*) (contents + i));
    fprintf(out, " %02hhx", contents[i]);
  }

  if (chunk_content_bytes_printed > 0 && chunk_content_bytes_printed < ptr->capacity)
    fprintf(out, " ... (%lu bytes omitted)", ptr->capacity - chunk_content_bytes_printed);

  fputc('\n', out);
}

void heap_full_dump(FILE* out, const chunk_head_t* heap_start, size_t chunk_content_bytes_printed) {
  for (const chunk_head_t* chnk = heap_start; chnk != NULL; chnk = chnk->next)
    heap_dump_chunk(out, chnk, chunk_content_bytes_printed);
}
