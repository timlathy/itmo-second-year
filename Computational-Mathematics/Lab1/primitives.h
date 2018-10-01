#ifndef PRIMITIVES
#define PRIMITIVES

/* cubic(x) = ax^3 + bx^2 + cx + d */
static inline double cubic(double a, double b, double c, double d, double x) {
  return (a * x * x * x) + (b * x * x) + (c * x) + d;
}

/* cubic'(x) = 3ax^2 + 2bx + c */
static inline double cubic_deriv(double a, double b, double c, double x) {
  return (3 * a * x * x) + (2 * b * x) + c;
}

/* cubic''(x) = 6ax + 2b */
static inline double cubic_deriv2(double a, double b, double x) {
  return (6 * a * x) + (2 * b);
}

typedef struct {
  double approx_root; double approx_root_value; int iter_num;
} result;

#endif
