// MAP_ANONYMOUS is not included in the C99 standard
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

/* Check out the header file for a brief introduction
 * to the concept of dynamic memory management */
#include "alloc.h"

char* heap_start;

void* heap_init() {
  heap_start = mmap(HEAP_START, CHUNK_INIT_SIZE, PROT_READ | PROT_WRITE,
    MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, 0, 0);

  ((chunk_head_t*) heap_start)->next = NULL;
  ((chunk_head_t*) heap_start)->capacity = CHUNK_INIT_SIZE - sizeof(chunk_head_t);
  ((chunk_head_t*) heap_start)->is_free = true;

  return (void*) heap_start;
}

bool reserve_new_chunk(chunk_head_t* last_chnk, size_t requested_size) {
  size_t chunk_size = requested_size + requested_size % sysconf(_SC_PAGESIZE);

  void* heap_end = (char*) last_chnk + sizeof(chunk_head_t) + last_chnk->capacity;

  void* new_area = mmap(heap_end, chunk_size, PROT_READ | PROT_WRITE,
    MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, 0, 0);
  if (new_area != MAP_FAILED && last_chnk->is_free) {
    last_chnk->capacity += chunk_size;
    return true;
  }
  else if (new_area == MAP_FAILED) {
    new_area = mmap(NULL, chunk_size, PROT_READ | PROT_WRITE,
      MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if (new_area == MAP_FAILED)
      return false;
  }
  chunk_head_t* new_chnk_head = (chunk_head_t*) new_area;
  new_chnk_head->next = NULL;
  new_chnk_head->capacity = chunk_size - sizeof(chunk_head_t);
  new_chnk_head->is_free = true;
  last_chnk->next = new_chnk_head;

  return true;
}

void merge_succeeding_free_chunks(chunk_head_t* chnk) {
  while ((char*) chnk->next == (char*) (chnk + 1) + chnk->capacity && chnk->next->is_free) {
    chnk->capacity += chnk->next->capacity + sizeof(chunk_head_t);
    chnk->next = chnk->next->next;
  }
}

void* heap_alloc(size_t requested_size) {
  if (requested_size % CHUNK_ALIGN != 0)
    requested_size += CHUNK_ALIGN - (requested_size % CHUNK_ALIGN);

  chunk_head_t* chnk = (chunk_head_t*) heap_start;
  while (true) {
    if (chnk->is_free) {
      merge_succeeding_free_chunks(chnk);
      if (chnk->capacity >= requested_size) break;
    }
    if (chnk->next == NULL) {
      if (!reserve_new_chunk(chnk, requested_size)) return NULL;
    }
    else chnk = chnk->next;
  }
  chnk->is_free = false;

  size_t remaining_capacity = chnk->capacity - requested_size;
  /* If we cannot fit a new chunk into the free space at the end of the chunk,
   * there is nothing we can do but leave the capacity as is. */
  if (remaining_capacity < CHUNK_MIN_SIZE)
    /* chnk + 1 skips the chunk header — we return a pointer to the usable area */
    return (void*) (chnk + 1);

  chnk->capacity = requested_size;
  char* chnk_end = ((char*) chnk) + sizeof(chunk_head_t) + requested_size;
  chunk_head_t* succ_chnk = (chunk_head_t*) chnk_end;
  succ_chnk->next = chnk->next;
  succ_chnk->capacity = remaining_capacity - sizeof(chunk_head_t);
  succ_chnk->is_free = true;
  chnk->next = (chunk_head_t*) chnk_end;
  return (void*) (chnk + 1);
}

void heap_free(void* ptr) {
  chunk_head_t* ptr_chnk = (chunk_head_t*) ((char*) ptr - sizeof(chunk_head_t));
  ptr_chnk->is_free = true;
  merge_succeeding_free_chunks(ptr_chnk);
}
