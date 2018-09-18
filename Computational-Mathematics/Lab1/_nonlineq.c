#include <Python.h>
#include "bisect.c"
#include "calc_result.h"

static char module_docstring[] =
  "This module provides a number of C routines for solving nonlinear equations.";
static char bisect_solve_interval_docstring[] =
  "Finds the root in an isolating interval using the bisection method.";

static PyObject* nonlineq_bisect_solve_interval(PyObject *self, PyObject *args);

static PyMethodDef module_methods[] = {
    {"bisect_solve_interval", nonlineq_bisect_solve_interval, METH_VARARGS,
      bisect_solve_interval_docstring},
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
  /* Accepts *k_0*, *k_1*, *k_2*, *k_3* coefficients (C doubles),
   * interval start (*a*) and end (*b*) (C doubles),
   * and *delta* (C double). */
  double k_0, k_1, k_2, k_3, a, b, delta;
  if (!PyArg_ParseTuple(args, "ddddddd", &k_0, &k_1, &k_2, &k_3, &a, &b, &delta))
    return NULL;

  calc_result calc_res = find_root_bisect(k_0, k_1, k_2, k_3, a, b, delta);

  return Py_BuildValue("(ddi)",
      calc_res.root, calc_res.value_in_root, calc_res.iter_num);
}
