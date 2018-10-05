#include "prime.h"

int is_prime(unsigned long n) {
  unsigned long i;

  if (n <= 1) return 0; /* 0 and 1 are not primes */
  if (n <= 3) return 1; /* 2 and 3 are primes */

  /* Given a number n, check whether any prime integer from 2 to sqrt(n)
   * evenly divides it.
   * This works because if n is divisible by some number p, then n = p * q,
   * and if n is larger than sqrt(n), then q is smaller and would have been
   * detected earlier. */

  if (n % 2 == 0) return 0;

  for (i = 3; i * i <= n; i += 2)
    if (n % i == 0) return 0;

  return 1;
}
