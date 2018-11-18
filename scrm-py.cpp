#include <Python.h>
#include "structmember.h" // fix something array have incomplete type
#include <string>  /* string */
#include <exception>
#include <stdexcept>

#include "lib/src/param.h"
#include "lib/src/forest.h"
#include "lib/src/random/random_generator.h"
#include "lib/src/random/mersenne_twister.h"



#define MODULE_DOC \
"Low level interface for scrm"



static PyObject *
run_scrm(PyObject *self, PyObject *args)
{
    PyObject *ret = NULL;

    int ok;
    char *s;
    ok = PyArg_ParseTuple(args, "s", &s);
    if (ok == 0){ // 0 is false/fail, 1 is success
        PyErr_SetString(PyExc_SystemError, "Bad cmd");
        return ret;
    }
    std::string cmd(s);


    try {
        // Organize output
        std::ostream *output = &std::cout;

        // Parse command line arguments
        Param user_para(cmd);
        Model model = user_para.parse();
        output->precision(user_para.precision());

        // Print help if user asked for it
        if (user_para.help()) {
          user_para.printHelp(*output);
          return ret;
        }
        if (user_para.version()) {
          *output << "scrm " << VERSION << std::endl;
          return ret;
        }

        MersenneTwister rg(user_para.seed_is_set(), user_para.random_seed());
        *output << user_para << std::endl;
        *output << rg.seed() << std::endl;

        if (user_para.print_model()) {
          *output << model << std::endl;
        }

        // Create the forest
        Forest forest = Forest(&model, &rg);

        // Loop over the independent loci/chromosomes
        for (size_t rep_i=0; rep_i < model.loci_number(); ++rep_i) {
          // Mark the start of a new independent sample
          *output << std::endl << "//" << std::endl;

          // Now set up the ARG, and sample the initial tree
          if ( user_para.read_init_genealogy() )
            forest.readNewick ( user_para.init_genealogy[ rep_i % user_para.init_genealogy.size()] );
          else forest.buildInitialTree();
          forest.printSegmentSumStats(*output);

          while (forest.next_base() < model.loci_length()) {
            // Sample next genealogy
            forest.sampleNextGenealogy();
            forest.printSegmentSumStats(*output);
          }
          assert(forest.next_base() == model.loci_length());

          forest.printLocusSumStats(*output);
          forest.clear();
        }

    } catch (const std::exception &e) {
        PyErr_SetString(PyExc_SystemError, e.what());
        return ret;
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Try 'scrm --help' for more information." << std::endl;
    }
    return ret;
}


static PyMethodDef scrm_methods[] = {
    {"run_scrm", (PyCFunction) run_scrm, METH_VARARGS,
         "scrm."},
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
