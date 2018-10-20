#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <linux/limits.h>

#include "llist.h"
#include "llist_iter.h"
#include "llist_io.h"

void print_element_space(int el) { printf("%d ", el); }
void print_element_newline(int el) { printf("%d\n", el); }

int plus(int a, int b) { return a + b; }
int min(int a, int b) { return a < b ? a : b; }
int max(int a, int b) { return a > b ? a : b; }
int square(int a) { return a * a; }
int cube(int a) { return a * a * a; }

char* read_filename(const char* prompt) {
  char* filename = (char*) malloc(PATH_MAX + 1);
  fputs(prompt, stdout);
  scanf("\n");
  fgets(filename, PATH_MAX + 1, stdin);
  fputs("\n", stdout);
  filename[strcspn(filename, "\n")] = 0; // remove the trailing newline preserved by fgets
  return filename;
}

int main() {
  char opt = 0;
  while (opt != 't' && opt != 'b' && opt != 'i') {
    puts("Do you wish to load the list from a file or enter it interactively?");
    fputs("Load [t]ext, load [b]inary, [i]nteractive: ", stdout);
    scanf("%c", &opt);
  }

  llist* lst = NULL;
  if (opt == 't' || opt == 'b') {
    char* filename = read_filename("Enter path to the source file: ");

    if (!( (opt == 't' && list_load(&lst, filename))
        || (opt == 'b' && list_deserialize(&lst, filename)))) {
      fprintf(stderr, "Unable to read the specified file\n");
      return 1;
    }

    free(filename);
  }
  else {
    puts("Enter list elements delimited by space; finish the input with ^D");
    list_read_stdin(&lst);
  }

  
  puts("Row view:");
  list_foreach(lst, print_element_space); printf("\n");
  puts("Column view:");
  list_foreach(lst, print_element_newline);
  printf("Number of elements: %d\n", list_length(lst));
  printf("Sum of elements: %d\n", list_foldl(lst, 0, plus));
  printf("Smallest element: %d\n", list_foldl(lst, INT_MAX, min));
  printf("Largest element: %d\n", list_foldl(lst, INT_MIN, max));
  puts("Absolute values:");
  list_map_mut(lst, abs);
  list_foreach(lst, print_element_space); printf("\n");

  llist* squares_lst = list_map(lst, square);
  puts("Squares:");
  list_foreach(squares_lst, print_element_space); printf("\n");

  llist* cubes_lst = list_map(lst, cube);
  puts("Cubes:");
  list_foreach(cubes_lst, print_element_space); printf("\n");

  opt = 0;
  puts("Do you wish to save the list to a file?");
  fputs("To [t]ext, to [b]inary, [n]o: ", stdout);
  scanf(" %c", &opt);
  
  if (opt == 't' || opt == 'b') {
    char* filename = read_filename("Enter destination path: ");

    if (opt == 't') list_save(lst, filename);
    else list_serialize(lst, filename);
    
    free(filename);
  }

  list_free(&lst);
  assert(lst == NULL);

  list_free(&squares_lst);
  assert(squares_lst == NULL);

  list_free(&cubes_lst);
  assert(cubes_lst == NULL);

  return 0;
}
