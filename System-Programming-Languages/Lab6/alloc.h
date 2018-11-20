#ifndef ALLOC_H
#define ALLOC_H

#include <stddef.h>
#include <stdbool.h>

/* Every process has its own virtual address space, divided into
 * program text (executable code), static data, heap, and stack sections:
 *
 * 0xf... | stack        |
 *        | ------------ |
 *        |              |
 *        |              |
 *        | ------------ |
 *        | heap         |
 *        | ------------ |
 *        | static data  |
 *        | ------------ |
 * 0x0... | program text |
 *        | ------------ |
 * 
 * Dynamic memory allocation (malloc) happens on the heap: this region "grows"
 * toward the stack, which is located near the opposite end of the virtual
 * address space. The stack meanwhile "grows" toward the heap
 * (remember, `push` decreases the stack pointer):
 *
 * | stack |            | stack |
 * | ----- |            | ----- |
 * |       |            |       |
 * |       |            |       |
 * |       |   malloc   | ----- |
 * |       |     ->     |       |
 * | ----- |            | heap  |
 * | heap  |            |       |
 * | ----- |            | ----- |
 *
 * Our heap begins at an arbitrary location chosen after the _program break_
 * (the end of static code and data): */
#define HEAP_START ((void*) 0x0404000)

/* The allocator sees heap as a collection of variable-sized chunks, which
 * are either _allocated_ or _free_:
 *
 * | | | | | |*|*|*|*|*|*|*|*|*| | | | | |
 *  f r e e   a l l o c a t e d   f r e e
 *
 * The sequence of `alloc` and `free` calls can be arbitrary:
 *
 * | | | | | | | | | | | | | | | | | | | |
 * |*|*|*|*| | | | | | | | | | | | | | | | p1 = alloc(4)
 * |*|*|*|*|-|-|-|-|-|-|-|-| | | | | | | | p2 = alloc(8)
 * | | | | |-|-|-|-|-|-|-|-| | | | | | | | free(p1)
 * | | | | |-|-|-|-|-|-|-|-|*|*|*|*|*|*| | p3 = alloc(6)
 *
 * The allocator must know how many bytes are occupied by each area
 * (to `free` it later), as well as which areas are free and which
 * are occupied to place subsequent allocations. To manage this,
 * we store some metadata at the beginning of every allocated chunk:
 */
typedef struct chunk_head_t chunk_head_t;
struct chunk_head_t {
  chunk_head_t* next;
  size_t capacity;
  bool is_free;
};

/* When there are no free chunks available that can fit the requested size,
 * we use the `mmap` system call to request more memory, expanding the heap.
 * The length of the area we ask `mmap` for must be a multiple of the system
 * page size (4 kilobytes on x86_64 Linux): */
#define CHUNK_INIT_SIZE (1 * sysconf(_SC_PAGESIZE))

/* Before any allocations can happen, we need to have at least one free chunk:
 * `heap_init` requests `CHUNK_INIT_SIZE` bytes from the OS and places a
 * `chunk_head_t` at the beginning of the area, with `is_free` set to `true`. */
void* heap_init();

/* The data returned by `alloc` must be aligned on an eight-byte boundary
 * (the address should be divisible by 8) for performance reasons. */
#define CHUNK_ALIGN 8

#define CHUNK_MIN_SIZE (sizeof(chunk_head_t) + CHUNK_ALIGN)

void* heap_alloc(size_t requested_size);

void heap_free(void* ptr);

#endif
