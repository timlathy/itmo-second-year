#include "primitives.h"

#include <Python.h>

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
    double f_x_0 = cubic(a, b, c, d, x_0);
    if (fabs(int_start - int_end) <= delta || fabs(f_x_0) <= delta)
      return (result) { x_0, f_x_0, iter_num };
  }
}

PyObject* find_root_bisect_log(double a, double b, double c, double d,
                               double int_start, double int_end, double delta) {
  PyObject* log = PyList_New(0);
  int iter_num = 0;

  while (1) {
    iter_num++;

    double x_0 = (int_start + int_end) / 2;

    double int_start_old = int_start;
    double int_end_old = int_end;

    if (cubic(a, b, c, d, int_start) * cubic(a, b, c, d, x_0) < 0)
      int_end = x_0;
    else
      int_start = x_0;

    double f_x = cubic(a, b, c, d, x_0);
    double f_start = cubic(a, b, c, d, int_start);
    double f_end = cubic(a, b, c, d, int_end);

    PyList_Append(log, Py_BuildValue("(ddddddd)",
      int_start_old, int_end_old, x_0, f_start, f_end, f_x, fabs(int_start_old - int_end_old)));

    if (fabs(int_start - int_end) <= delta || fabs(f_x) <= delta) return log;
  }
}
