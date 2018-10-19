#include <assert.h>
#include <stdio.h>

#include "llist.h"
#include "llist_iter.h"

void print_element_space(int el) { printf("%d ", el); }
void print_element_newline(int el) { printf("%d\n", el); }

int plus(int a, int b) { return a + b; }
int square(int a) { return a * a; }
int cube(int a) { return a * a * a; }

int main() {
  llist* lst = NULL;
  int el;

  printf("Enter list elements, delimited by space. Finish the input with ^D\n");
  while (scanf("%d", &el) != EOF)
    list_add_front(&lst, el);

  list_foreach(lst, print_element_space); printf("\n");

  list_foreach(lst, print_element_newline);
  printf("Sum of elements: %d\n", list_foldl(lst, 0, plus));

  llist* squares_lst = list_map(lst, square);
  printf("Squares: \n");
  list_foreach(squares_lst, print_element_space); printf("\n");

  llist* cubes_lst = list_map(lst, cube);
  printf("Cubes: \n");
  list_foreach(cubes_lst, print_element_space); printf("\n");

  list_free(&lst);
  assert(lst == NULL);

  return 0;
}
