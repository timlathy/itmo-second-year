#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "llist.h"
#include "llist_iter.h"

void write_element(int el, void* dstfile) { fprintf(dstfile, "%d\n", el); }

void print_element_space(int el) { printf("%d ", el); }
void print_element_newline(int el) { printf("%d\n", el); }

int plus(int a, int b) { return a + b; }
int square(int a) { return a * a; }
int cube(int a) { return a * a * a; }

bool list_save(llist* lst, const char* filename) {
  FILE* dst = fopen(filename, "w");
  if (dst == NULL)
    return false;

  list_foreach_closure(lst, dst, write_element);

  return fclose(dst) == 0;
}

bool list_load(llist** lst, const char* filename) {
  FILE* src = fopen(filename, "r");
  if (src == NULL)
    return false;

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

int main() {
  char opt = 0;
  while (opt != 'L' && opt != 'I') {
    printf("Do you wish to load the list from a file or enter it interactively?\n");
    printf("[L]oad, [I]nteractive: ");
    scanf("%c", &opt);
    printf("\n");
  }

  llist* lst = NULL;
  if (opt == 'L') {
    char filename[64]; // 64 chars is ought to be enough for anybody
    printf("Enter filename to open: ");
    scanf("%63s", filename);
    printf("\n");
    if (!list_load(&lst, filename)) {
      fprintf(stderr, "Unable to open the file\n");
      return 1;
    }
  }
  else {
    printf("Enter list elements delimited by space; finish the input with ^D\n");
    list_read_stdin(&lst);
  }

  list_foreach(lst, print_element_space); printf("\n");

  list_foreach(lst, print_element_newline);
  printf("Sum of elements: %d\n", list_foldl(lst, 0, plus));

  llist* squares_lst = list_map(lst, square);
  printf("Squares: \n");
  list_foreach(squares_lst, print_element_space); printf("\n");

  llist* cubes_lst = list_map(lst, cube);
  printf("Cubes: \n");
  list_foreach(cubes_lst, print_element_space); printf("\n");

  printf("Do you wish to save the list to a file?\n");
  printf("[Yy]es, [Nn]o: ");
  scanf("%c", &opt);
  
  if (opt == 'Y' || opt == 'y') {
    char filename[64]; // 64 chars is ought to be enough for anybody
    printf("Enter filename to save to: ");
    scanf("%63s", filename);
    printf("\n");
    return list_save(lst, filename);
  }

  list_free(&lst);
  assert(lst == NULL);

  return 0;
}
