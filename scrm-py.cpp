#include <Python.h>
#include "structmember.h" // fix something array have incomplete type
#include <string>  /* string */
#include <exception>
#include <stdexcept>



#define MODULE_DOC \
"Low level interface for scrm"


static PyObject *
scrm_add(PyObject *self, PyObject *args)
{
    PyObject *ret = NULL;
    double x, y, result;

    if (!PyArg_ParseTuple(args, "dd", &x, &y)) {
        goto out;
    }
    result = x + y;
    ret = Py_BuildValue("d", result);
out:
    return ret;
}


static PyMethodDef scrm_methods[] = {
    {"add", (PyCFunction) scrm_add, METH_VARARGS,
         "Print a lovely skit to standard output."},
    {NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static struct PyModuleDef scrmmodule = {
    PyModuleDef_HEAD_INIT,
    "_scrm",
    MODULE_DOC,
    -1,
    scrm_methods,
    NULL, NULL, NULL, NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC
PyInit__scrm(void)

#else
#define INITERROR return

void
init__scrm(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&scrmmodule);
#else
    PyObject *module = Py_InitModule3("_scrm", scrm_methods, MODULE_DOC);
#endif


// Stuff goes here
    //std::cout << "do something here" << std::endl;

#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}
