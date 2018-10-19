#include <stdio.h>
#include <assert.h>

#include "llist.h"
#include "llist_iter.h"
#include "llist_io.h"

void print_element_space(int el) { printf("%d ", el); }
void print_element_newline(int el) { printf("%d\n", el); }

int plus(int a, int b) { return a + b; }
int square(int a) { return a * a; }
int cube(int a) { return a * a * a; }

int main() {
  char opt = 0;
  while (opt != 't' && opt != 'b' && opt != 'i') {
    printf("Do you wish to load the list from a file or enter it interactively?\n");
    printf("Load [t]ext, Load [b]inary, [i]nteractive: ");
    scanf("%c", &opt);
    printf("\n");
  }

  llist* lst = NULL;
  if (opt == 't' || opt == 'b') {
    char filename[64]; // 64 chars is ought to be enough for anybody
    printf("Enter filename to open: ");
    scanf("%63s", filename);
    printf("\n");

    if (!( (opt == 't' && list_load(&lst, filename))
        || (opt == 'b' && list_deserialize(&lst, filename)))) {
      fprintf(stderr, "Unable to read the specified file\n");
      return 1;
    }
  }
  else {
    printf("Enter list elements delimited by space; finish the input with ^D\n");
    list_read_stdin(&lst);
  }

  list_foreach(lst, print_element_space); printf("\n");

  list_foreach(lst, print_element_newline);
  printf("Number of elements: %d\n", list_length(lst));
  printf("Sum of elements: %d\n", list_foldl(lst, 0, plus));

  llist* squares_lst = list_map(lst, square);
  printf("Squares: \n");
  list_foreach(squares_lst, print_element_space); printf("\n");

  llist* cubes_lst = list_map(lst, cube);
  printf("Cubes: \n");
  list_foreach(cubes_lst, print_element_space); printf("\n");

  printf("Do you wish to save the list to a file?\n");
  printf("To [t]ext, to [b]inary, [n]o: ");
  scanf(" %c", &opt);
  
  if (opt == 't' || opt == 'b') {
    char filename[64]; // 64 chars is ought to be enough for anybody
    printf("Enter the target filename: ");
    scanf("%63s", filename);
    printf("\n");
    if (opt == 't') {
      return list_save(lst, filename);
    }
    return list_serialize(lst, filename);
  }

  list_free(&lst);
  assert(lst == NULL);

  return 0;
}
