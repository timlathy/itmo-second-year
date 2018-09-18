#include "calc_result.h"

double f(double k_0, double k_1, double k_2, double k_3, double x);

calc_result find_root_bisect(double k_0, double k_1, double k_2, double k_3,
                             double a, double b, double delta);

double f(double k_0, double k_1, double k_2, double k_3, double x) {
  return (k_0 * x * x * x) + (k_1 * x * x) + (k_2 * x) + k_3;
}

calc_result find_root_bisect(double k_0, double k_1, double k_2, double k_3,
                             double a, double b, double delta) {
  int iter_num = 0;
  while (1) {
    iter_num++;
    /* === Step 1. Divide the interval */
    double x_0 = (a + b) / 2;
    if (f(k_0, k_1, k_2, k_3, a) * f(k_0, k_1, k_2, k_3, x_0) < 0) {
      /* If f(a) and f(x_0) have different signs, put [a; x_0] as the new interval */
      b = x_0;
    }
    else {
      /* Otherwise, put [x_0; b] as the new interval */
      a = x_0;
    }
    /* === Step 2. Check if we have arrived at the answer */
    double f_a = f(k_0, k_1, k_2, k_3, a);
    if (abs(f_a) - delta <= 0) {
      return (calc_result) { a, f_a, iter_num };
    }
    double f_b = f(k_0, k_1, k_2, k_3, b);
    if (abs(f_b) - delta <= 0) {
      return (calc_result) { b, f_b, iter_num };
    }
  }
}
