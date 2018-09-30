#include "primitives.h"

result find_root_newton(double a, double b, double c, double d,
                        double int_start, double int_end, double delta);

result find_root_newton(double a, double b, double c, double d,
                        double int_start, double int_end, double delta) {
  int iter_num = 0;
  double x;

  if (cubic(a, b, c, d, int_start) * cubic_deriv2(a, b, int_start) > 0)
    x = int_start;
  else
    x = int_end;

  while (1) {
    iter_num++;

    x = x - cubic(a, b, c, d, x) / cubic_deriv(a, b, c, x);
    double f_x = cubic(a, b, c, d, x);

    if (fabs(f_x) <= delta)
      return (result) { x, f_x, iter_num };
  }
}
