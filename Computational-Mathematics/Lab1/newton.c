#include "primitives.h"

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

    double step = cubic(a, b, c, d, x) / cubic_deriv(a, b, c, x);

    x = x - step;
    double f_x = cubic(a, b, c, d, x);

    if (fabs(step) <= delta || fabs(f_x) <= delta)
      return (result) { x, f_x, iter_num };
  }
}

PyObject* find_root_newton_log(double a, double b, double c, double d,
                               double int_start, double int_end, double delta) {
  PyObject* log = PyList_New(0);
  int iter_num = 0;
  double x;

  if (cubic(a, b, c, d, int_start) * cubic_deriv2(a, b, int_start) > 0)
    x = int_start;
  else
    x = int_end;

  while (1) {
    iter_num++;

    double f_x = cubic(a, b, c, d, x);
    double f_prime_x = cubic_deriv(a, b, c, x);
    double new_x = x - f_x / f_prime_x;

    PyList_Append(log, Py_BuildValue("(ddddd)", x, f_x, f_prime_x, new_x, fabs(x - new_x)));

    if (fabs(cubic(a, b, c, d, new_x)) <= delta) return log;

    x = new_x;
  }
}
