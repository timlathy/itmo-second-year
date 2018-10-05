#include "dot.h"

long dot_product(const int a[], const int b[], size_t vec_len) {
  size_t i;
  long result = 0;

  for (i = 0; i < vec_len; i++) result += a[i] * b[i];

  return result;
}
