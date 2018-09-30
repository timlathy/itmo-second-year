#include <Python.h>
#include "bisect.c"
#include "newton.c"
#include "primitives.h"

static char module_docstring[] =
  "This module provides a number of C routines for solving nonlinear equations.";
static char bisect_solve_interval_docstring[] =
  "Finds the root in an isolating interval using the bisection method.";
static char newton_solve_interval_docstring[] =
  "Finds the root in an isolating interval using the Newton method.";

static PyObject* nonlineq_bisect_solve_interval(PyObject *self, PyObject *args);
static PyObject* nonlineq_newton_solve_interval(PyObject *self, PyObject *args);

static PyMethodDef module_methods[] = {
    {"bisect_solve_interval", nonlineq_bisect_solve_interval, METH_VARARGS,
      bisect_solve_interval_docstring},
    {"newton_solve_interval", nonlineq_newton_solve_interval, METH_VARARGS,
      newton_solve_interval_docstring},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC PyInit__nonlineq(void) {
  static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "nonlineq",
    module_docstring,
    0, /* no global state, no per-module state */
    module_methods,
    NULL, /* no multi-phase initialization */
    NULL, /* no gc traversal fun */
    NULL, /* no gc clearing fun */
    NULL /* no deallocation fun */
  };

  PyObject* module = PyModule_Create(&moduledef);
  if (!module) return NULL;
  return module;
}

static PyObject* nonlineq_bisect_solve_interval(PyObject *self, PyObject *args) {
  /* Accepts *a*, *b*, *c*, *d* coefficients (C doubles),
   * interval start (*int_start*) and end (*int_end*) (C doubles),
   * and *delta* (C double). */
  double a, b, c, d, int_start, int_end, delta;
  if (!PyArg_ParseTuple(args, "ddddddd", &a, &b, &c, &d, &int_start, &int_end, &delta))
    return NULL;

  result res = find_root_bisect(a, b, c, d, int_start, int_end, delta);

  return Py_BuildValue("(ddi)", res.approx_root, res.approx_root_value, res.iter_num);
}

static PyObject* nonlineq_newton_solve_interval(PyObject *self, PyObject *args) {
  /* Accepts *a*, *b*, *c*, *d* coefficients (C doubles),
   * interval start (*int_start*) and end (*int_end*) (C doubles),
   * and *delta* (C double). */
  double a, b, c, d, int_start, int_end, delta;
  if (!PyArg_ParseTuple(args, "ddddddd", &a, &b, &c, &d, &int_start, &int_end, &delta))
    return NULL;

  result res = find_root_newton(a, b, c, d, int_start, int_end, delta);

  return Py_BuildValue("(ddi)", res.approx_root, res.approx_root_value, res.iter_num);
}
