#include "primitives.h"

#include <Python.h>

result find_root_bisect(double a, double b, double c, double d,
                        double int_start, double int_end, double delta);

result find_root_bisect(double a, double b, double c, double d,
                        double int_start, double int_end, double delta) {
  int iter_num = 0;

  while (1) {
    iter_num++;

    /* === Step 1. Divide the interval */
    double x_0 = (int_start + int_end) / 2;

    if (cubic(a, b, c, d, int_start) * cubic(a, b, c, d, x_0) < 0)
      int_end = x_0; /* if sign(f(int_start)) = sign(f(x_0)), then put [int_start, x_0] as the new interval */
    else
      int_start = x_0; /* otherwise, put [x_0, int_end] as the new interval */

    /* === Step 2. Check if we have arrived at the answer */
    double f_start = cubic(a, b, c, d, int_start);
    if (fabs(f_start) <= delta) return (result) { int_start, f_start, iter_num };

    double f_end = cubic(a, b, c, d, int_end);
    if (fabs(f_end) <= delta) return (result) { int_end, f_end, iter_num };
  }
}
