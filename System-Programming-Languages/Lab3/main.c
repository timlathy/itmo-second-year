#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "prime.h"
#include "dot.h"

const int a[] = { 1, 3, 5, 7 };
const int b[] = { 2, 4, 6, 8 };

void print_array(const char* prelude, const int a[], size_t len) {
  size_t i;

  printf(prelude);
  for (i = 0; i < len; i++) printf("%d ", a[i]);
  printf("\n");
}

int main() {
  unsigned long primality_test_input;
  const size_t dot_vec_len = sizeof(a) / sizeof(int);
  char input_buffer[64]; /* 64 chars ought to be enough for anybody */
  char* input_end_char;

  print_array("a[]: ", a, dot_vec_len);
  print_array("b[]: ", b, dot_vec_len);
  printf("dot(a, b) = %ld\n", dot_product(a, b, dot_vec_len));

  printf("\nEnter a number to check for primality: ");

  if (!scanf("%63s", input_buffer)) {
    fprintf(stderr, "Input is too long\n");
    return 1;
  }
  errno = 0;
  primality_test_input = strtoul((const char*) input_buffer, &input_end_char, 10);
  if (errno == ERANGE || *input_end_char != '\0' || input_buffer[0] == '-') {
    fprintf(stderr, "Input is not a number\n");
    return 1;
  }

  if (is_prime(primality_test_input))
    printf("%lu is a prime number\n", primality_test_input);
  else
    printf("%lu is not a prime number\n", primality_test_input);

  return 0;
}
