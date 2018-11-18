#ifndef ALLOC_DEBUG_H
#define ALLOC_DEBUG_H

#include "alloc.h"

void heap_dump_chunk(FILE* out, const chunk_head_t* ptr, size_t chunk_content_bytes_printed);

void heap_full_dump(FILE* out, const chunk_head_t* heap_start, size_t chunk_content_bytes_printed);

#endif
