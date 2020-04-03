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
    double x_scale;
    double y_scale;
    double z_scale;
    double x_offset;
    double y_offset;
    double z_offset;
    uint64_t utc_time;
} LASHeaderPython;

static void LASHeader_dealloc(LASHeaderPython * self) {
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static int LASHeader_init (LASHeaderPython * self, PyObject * args, PyObject *kwargs) {
    
    self->number_of_point_records = 0;
    self->x_scale = 1.0;
    self->y_scale = 1.0;
    self->z_scale = 1.0;
    self->x_offset = 0.0;
    self->y_offset = 0.0;
    self->z_offset = 0.0;
    self->utc_time = 0;
    return 0;
}

static PyMemberDef LASHeader_members[] = {
    {"number_of_points", T_UINT, offsetof(LASHeaderPython, number_of_point_records), 0, "Number of points"},
    {"x_scale", T_DOUBLE, offsetof(LASHeaderPython, x_scale), 0, "x scale factor."},
    {"y_scale", T_DOUBLE, offsetof(LASHeaderPython, y_scale), 0, "y scale factor."},
    {"z_scale", T_DOUBLE, offsetof(LASHeaderPython, z_scale), 0, "z scale factor."},
    {"x_offset", T_DOUBLE, offsetof(LASHeaderPython, x_offset), 0, "x offset factor."},
    {"y_offset", T_DOUBLE, offsetof(LASHeaderPython, y_offset), 0, "y offset factor."},
    {"z_offset", T_DOUBLE, offsetof(LASHeaderPython, z_offset), 0, "z offset factor."},
    {"utc_time", T_DOUBLE, offsetof(LASHeaderPython, utc_time), 0, "utc_time in us from epoch"},

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

    if (!PyArg_ParseTupleAndKeywords (args, kwargs, "|dddHbK", kwlist, &x, &y, &z, &intensity, &quality, &utc_time)) {
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
        self->header = (PyObject *) PyObject_New(LASHeaderPython, &LASHeaderPythonType);
        if (!self->header) {
            Py_DECREF(self);
            return NULL;
        }

        self->entries = PyList_New(0);
        if (!self->entries) {
            Py_DECREF(self);
            return NULL;
        }
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
    .tp_new = LASFile_new,
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
    int result = read_las(filename, &contents);

    if (result < 0) {
        PyErr_SetString(PyExc_RuntimeError, "read_las file failed.");
        return NULL;
    }

    //TODO put the contents into python types.
    PyObject * data_list = PyList_New(result);
    for (int i = 0; i<result; ++i) {
        LASFilePython * file_entry =  (LASFilePython *) PyObject_CallObject((PyObject *) &LASFilePythonType, NULL);
        if (!file_entry){
            PyErr_Print();
        }

        ((LASHeaderPython *)file_entry->header)->number_of_point_records = (*contents[i]).header->number_of_point_records;
        ((LASHeaderPython *)file_entry->header)->x_scale = (*contents[i]).header->x_scale_factor;
        ((LASHeaderPython *)file_entry->header)->y_scale = (*contents[i]).header->y_scale_factor;
        ((LASHeaderPython *)file_entry->header)->z_scale = (*contents[i]).header->z_scale_factor;
        ((LASHeaderPython *)file_entry->header)->x_offset = (*contents[i]).header->x_offset;
        ((LASHeaderPython *)file_entry->header)->y_offset = (*contents[i]).header->y_offset;
        ((LASHeaderPython *)file_entry->header)->z_offset = (*contents[i]).header->z_offset;
        ((LASHeaderPython *)file_entry->header)->utc_time = AdjustedGPSTimeToUTCTime((*contents[i]).header->guid_data_4);

        PyObject * temp = file_entry->entries;
        file_entry->entries = PyList_New( ((LASHeaderPython *)file_entry->header)->number_of_point_records );
        if (!file_entry->entries){
            PyErr_Print();
        }
        Py_DECREF (temp);

        for (unsigned int point = 0; point < ((LASHeaderPython *)file_entry->header)->number_of_point_records; ++point) {
            PyObject * point_init = Py_BuildValue("dddHbK", 
                                                    ((LASHeaderPython *)file_entry->header)->x_scale * (double)(*contents[i]).entries[point].x, 
                                                    ((LASHeaderPython *)file_entry->header)->y_scale * (double)(*contents[i]).entries[point].y, 
                                                    ((LASHeaderPython *)file_entry->header)->z_scale * (double)(*contents[i]).entries[point].z, 
                                                    (*contents[i]).entries[point].intensity, 
                                                    (*contents[i]).entries[point].user_data, 
                                                    AdjustedGPSTimeToUTCTime((uint64_t)(*contents[i]).entries[point].gps_time));
            if (!point_init){
                PyErr_Print();
            }

            LASEntryPython * point_entry = (LASEntryPython * ) PyObject_CallObject((PyObject *) &LASEntryPythonType, point_init);
            if (!point_entry){
               PyErr_Print();
            }
            Py_DECREF(point_init);

            //PyList_Append(file_entry->entries, (PyObject *) point_entry );
            //Py_DECREF(point_entry);
            if (PyList_SetItem(file_entry->entries,  point, (PyObject *) point_entry ) < 0) {
                PyErr_Print();
            }

            
        }

        free ((*contents[i]).header);
        free ((*contents[i]).entries);
        
        int ret = PyList_SetItem(data_list, i, (PyObject *)file_entry);
        //Py_DECREF (file_entry); Don't do this, set item is an exception that doesn't increment the reference count.

        if (ret<0) {
            for (unsigned int j = i+1; j < ((LASHeaderPython *)file_entry->header)->number_of_point_records; ++j) {
                free((*contents[j]).header);
                free((*contents[j]).entries);
            }
            free (contents);
            Py_DECREF(data_list);
            return NULL;
        }
    }

    free (contents);

    return data_list;

};

static PyObject * write_las_wrapper(PyObject * self, PyObject * args) {
    char * filename;
    PyObject * las_files = NULL;
    //parse arguments
    if (!PyArg_ParseTuple(args, "sO", &filename, &las_files)) {
        return NULL;
    }

    if (!PyList_Check (las_files) ) {
        PyErr_SetString(PyExc_TypeError, "write_las requires a list of LASFiles as input.");
        Py_DECREF(las_files);
        return NULL;
    }
    
    Py_ssize_t num_of_files = PyList_Size(las_files);
    if (num_of_files <= 0) {
        PyErr_SetString(PyExc_RuntimeError, "Require at least on LASFile to write.");
        Py_DECREF(las_files);
        return NULL;
    }

    LASFile * las_files_c = NULL;
    las_files_c = (LASFile * )malloc(num_of_files * sizeof(LASFile));

    for (Py_ssize_t i = 0; i < num_of_files; ++i) {
        LASFilePython * las_file = (LASFilePython* )PyList_GetItem( (PyObject *)las_files, i);
        Py_ssize_t number_of_points = ((LASHeaderPython *)((LASFilePython*)las_file)->header)->number_of_point_records;

        las_files_c[i].header = initLASHeader( ((LASHeaderPython *)((LASFilePython*)las_file)->header)->utc_time, 
                                            (uint32_t)number_of_points);

        
        las_files_c[i].entries = (LASEntry *) malloc (number_of_points * sizeof (LASEntry));
        
        for (Py_ssize_t point = 0; point < PyList_Size(las_file->entries); ++point) {
            LASEntryPython * entry = (LASEntryPython *)PyList_GetItem(las_file->entries, point);

            las_files_c[i].entries[point] = initLASEntry( entry->utc_time,
                                            entry->x,
                                            entry->y,
                                            entry->z,
                                            entry->intensity,
                                            entry->quality );
        }
    }

    int result = write_las(filename, las_files_c, num_of_files); 

    for (Py_ssize_t i = 0; i < num_of_files; ++i) {
        free (las_files_c[i].header);
        free (las_files_c[i].entries);
    }
    free (las_files_c);
    
    if (result < 0) {
        PyErr_SetString(PyExc_RuntimeError, "write_las failed.");
        return NULL;
    }

    Py_INCREF (Py_None);
    return Py_None;

};


//-----------------------------------------------------------------
// Module setup
//-----------------------------------------------------------------

PyDoc_STRVAR(read_las_doc,
	"read_las(filename) -> List of Files\n\n"
	"Reads in a LAS File and returns a list of every individual LAS \n"
    "file/profile in the set.\n"
	"Returns a list of LASFiles\n");

PyDoc_STRVAR(write_las_doc,
    "write_las(filename, list_of_LASFiles)\n\n"
    "Write a las file to the hard drive given the filename and \n"
    "a list of LASFiles.");

static PyMethodDef LASMethods[] = {
    {"read_las", read_las_wrapper, METH_VARARGS, read_las_doc},
    {"write_las", write_las_wrapper, METH_VARARGS, write_las_doc},
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