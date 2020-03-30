/**
 * @file las_2g_python_module.c
 * @author Ryan Wicks
 * @brief This is the boilerplate code for setting up the python module
 * @version 0.1
 * @date 2020-03-07
 * 
 * @copyright 2G Robotics Inc., Copyright (c) 2020
 * 
 */
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"
#include "las_2g_python.h"

//-----------------------------------------------------------------
// LAS types definitions
//-----------------------------------------------------------------

// LAS Header Definitions
//-----------------------------------------------------------------
typedef struct {
    PyObject_HEAD
    uint32_t number_of_point_records;
} LASHeaderPython;

static void LASHeader_dealloc(LASHeaderPython * self) {
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static int LASHeader_init (LASHeaderPython * self, PyObject * args, PyObject *kwargs) {
    
    self->header = 0;

    return 0;
}

static PyMemberDef LASHeader_members[] = {
    {"number_of_points", T_UINT, offsetof(LASHeaderPython, number_of_point_records), 0, "Number of points"},
    {NULL} //sentinel
};

static PyTypeObject LASHeaderPythonType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "las_2g.LASHeader",
    .tp_doc = "A LAS header.",
    .tp_basicsize = sizeof(LASHeaderPython),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc) LASHeader_init,
    .tp_dealloc = (destructor) LASHeader_dealloc,
    .tp_members = LASHeader_members,
    //.tg_methods = LASEntry_methods,
};

// LAS Entry Definitions
//-----------------------------------------------------------------
typedef struct {
    PyObject_HEAD
    double x;
    double y;
    double z;
    uint16_t intensity;
    uint8_t quality;
    uint64_t utc_time;
} LASEntryPython;

static void LASEntry_dealloc(LASEntryPython * self) {
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static int LASEntry_init (LASEntryPython * self, PyObject * args, PyObject *kwargs) {
    
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    uint16_t intensity = 0;
    uint8_t quality = 0;
    uint64_t utc_time = 0;

    static char * kwlist [] = {"x", "y", "z", "intensity", "quality", "utc_time_us"};

    if (!PyArg_ParseTupleAndKeywords (args, kwargs, "|dddhbK", kwlist, &x, &y, &z, &intensity, &quality, &utc_time)) {
        return -1;
    }

    self->x = x;
    self->y = y; 
    self->z = z;
    self->intensity = intensity;
    self->quality = quality;
    self->utc_time = utc_time;

    return 0;
}

static PyMemberDef LASEntry_members[] = {
    {"x", T_DOUBLE, offsetof(LASEntryPython, x), 0, "x co-ordinate in m."},
    {"y", T_DOUBLE, offsetof(LASEntryPython, y), 0, "y co-ordinate in m."},
    {"z", T_DOUBLE, offsetof(LASEntryPython, z), 0, "z co-ordinate in m."},
    {"intensity", T_SHORT, offsetof(LASEntryPython, intensity), 0, "intensity"},
    {"quality", T_BYTE, offsetof(LASEntryPython, quality), 0, "quality"},
    {"utc_time", T_ULONGLONG, offsetof(LASEntryPython, utc_time), 0, "utc time in microseconds from the unix epoch epoch"},
    {NULL} //sentinel
};

static PyTypeObject LASEntryPythonType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "las_2g.LASEntry",
    .tp_doc = "A single LAS entry/point.",
    .tp_basicsize = sizeof(LASEntryPython),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc) LASEntry_init,
    .tp_dealloc = (destructor) LASEntry_dealloc,
    .tp_members = LASEntry_members,
    //.tg_methods = LASEntry_methods,
};


// LAS File Definitions
//-----------------------------------------------------------------
typedef struct {
    PyObject_HEAD
    PyObject * header; //LASHeaderPython object
    PyObject * entries; //List of LASEntryPython objects
} LASFilePython;

static void LASFile_dealloc(LASFilePython * self) {
    Py_XDECREF(self->header);
    Py_XDECREF(self->entries); //list has responsibility for releasing entry elements.
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject * LASFile_new(PyTypeObject * type, PyObject * args, PyObject * kwargs) {
    LASFilePython * self;
    self = (LASFilePython *) type->tp_alloc(type,0);
    if (self != NULL) {
        self->header = ;
    }

    return (PyObject * ) self;
}

static int LASFile_init (LASFilePython * self, PyObject * args, PyObject *kwargs) {
    
    //code for the init function

    return 0;
}

static PyMemberDef LASFile_members[] = {
    {"header", T_OBJECT_EX, offsetof(LASFilePython, header), 0, "File Header"},
    {"entries", T_OBJECT_EX, offsetof(LASFilePython, entries), 0, "List of Entries"},
    {NULL} //sentinel
};

static PyTypeObject LASFilePythonType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "las_2g.LASFile",
    .tp_doc = "A single LAS File containing a header and multiple entries",
    .tp_basicsize = sizeof(LASFilePython),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_init = (initproc) LASFile_init,
    .tp_new = PyType_GenericNew,
    .tp_dealloc = (destructor) LASFile_dealloc,
    .tp_members = LASFile_members,
    //.tg_methods = LASFile_methods,
};

//-----------------------------------------------------------------
// Methods definitions
//-----------------------------------------------------------------

static PyObject * read_las_wrapper(PyObject * self, PyObject * args) {
    char * filename;

    //parse arguments
    if (!PyArg_ParseTuple(args, "s", &filename)) {
        return NULL;
    }

    //run the function
    LASFile ** contents = NULL;
    int result = read_las(filename, contents);

    if (result < 0) {
        PyErr_SetString(PyExc_RuntimeError, "read_las file failed.");
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;

};

static PyObject * write_las_wrapper(PyObject * self, PyObject * args) {
    char * filename;

    //parse arguments
    if (!PyArg_ParseTuple(args, "s", &filename)) {
        return NULL;
    }

    //run the function

    int result = -1;

    if (result < 0) {
        PyErr_SetString(PyExc_RuntimeError, "write_las failed.");
        return NULL;
    }

    Py_INCREF (Py_None);
    return Py_None;

};


static PyObject * hello_wrapper(PyObject * self, PyObject * args) {
    char * input;
    char * result;
    PyObject * ret;

    //parse arguments
    if (!PyArg_ParseTuple(args, "s", &input)) {
        return NULL;
    }

    //run the function
    result = hello(input);

    if (!result) {
        PyErr_SetString(PyExc_RuntimeError, "Hello failed.");
        return NULL;
    }

    ret  = PyUnicode_FromString(result);
    free (result);

    return ret;

};

//-----------------------------------------------------------------
// Module setup
//-----------------------------------------------------------------

static PyMethodDef LASMethods[] = {
    {"hello", hello_wrapper, METH_VARARGS, "Say Hello!"},
    {"read_las", read_las_wrapper, METH_VARARGS, "Read a LAS file."},
    {"write_las", write_las_wrapper, METH_VARARGS, "Write the LAS file."},
    {NULL, NULL, 0, NULL} //sentinel
};

static struct PyModuleDef las_2g_module = {
    PyModuleDef_HEAD_INIT,
    "las_2g_python",
    "Module that loads and writes LAS files produced by the 2G API.",
    -1,
    LASMethods
};

PyMODINIT_FUNC PyInit_las_2g_python(void) {
    PyObject *m;

    if (PyType_Ready(&LASEntryPythonType) <  0) {
        return NULL;
    }
    if (PyType_Ready(&LASHeaderPythonType) <  0) {
        return NULL;
    }
    if (PyType_Ready(&LASFilePythonType) <  0) {
        return NULL;
    }

    m = PyModule_Create(&las_2g_module);
    if (m == NULL) {
        return NULL;
    }

    Py_INCREF(&LASEntryPythonType);
    if (PyModule_AddObject(m, "LASEntry", (PyObject *) &LASEntryPythonType) < 0) {
        Py_DECREF(&LASEntryPythonType);
        Py_DECREF(m);
        return NULL;
    }

    Py_INCREF(&LASHeaderPythonType);
    if (PyModule_AddObject(m, "LASHeader", (PyObject *) &LASHeaderPythonType) < 0) {
        Py_DECREF(&LASEntryPythonType);
        Py_DECREF(&LASHeaderPythonType);
        Py_DECREF(m);
        return NULL;
    }

    Py_INCREF(&LASFilePythonType);
    if (PyModule_AddObject(m, "LASFile", (PyObject *) &LASFilePythonType) < 0) {
        Py_DECREF(&LASFilePythonType);
        Py_DECREF(&LASHeaderPythonType);
        Py_DECREF(&LASEntryPythonType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
};