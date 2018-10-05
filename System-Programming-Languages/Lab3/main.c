#include <stdio.h>
#include "prime.h"
#include "dot.h"

const int a[] = { 1, 3, 5, 7 };
const int b[] = { 2, 4, 6, 8 };

void print_array(const char* prelude, const int a[], size_t len) {
  size_t i;

  printf(prelude);
  for (i = 0; i < len - 1; i++) printf("%d ", a[i]);
  printf("\n");
}

int main() {
  unsigned long d;
  const size_t len = sizeof(a) / sizeof(int);

  print_array("a[]: ", a, len);
  print_array("b[]: ", b, len);
  printf("dot(a, b) = %ld\n", dot_product(a, b, len));

  printf("\nEnter a number to check for primality: ");

  if (!scanf("%lu", &d)) {
    fprintf(stderr, "Input is not a number\n");
    return 1;
  }

  if (is_prime(d)) printf("%lu is a prime number\n", d);
  else printf("%lu is not a prime number\n", d);

  return 0;
}
