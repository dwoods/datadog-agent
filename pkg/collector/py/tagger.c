// Unless explicitly stated otherwise all files in this repository are licensed
// under the Apache License Version 2.0.
// This product includes software developed at Datadog (https://www.datadoghq.com/).
// Copyright 2018 Datadog, Inc.

// +build cpython

#include "tagger.h"

// Functions
PyObject* GetTags(char *id, int highCard);

static PyObject *get_tags(PyObject *self, PyObject *args) {
    char *entity;
    int  high_card;

    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();

    if (!PyArg_ParseTuple(args, "si", &entity, &high_card)) {
      PyGILState_Release(gstate);
      return NULL;
    }

    PyGILState_Release(gstate);
    return GetTags(entity, high_card);
}

static PyMethodDef taggerMethods[] = {
  {"get_tags", get_tags, METH_VARARGS, "Get tags for an entity."},
  {NULL, NULL}
};

static struct PyModuleDef taggerDef = {
  PyModuleDef_HEAD_INIT,
  "tagger",        /* m_name */
  "tagger module", /* m_doc */
  -1,              /* m_size */
  taggerMethods,   /* m_methods */
  NULL,            /* m_reload */
  NULL,            /* m_traverse */
  NULL,            /* m_clear */
  NULL,            /* m_free */
};

void inittagger()
{
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();

  PyObject *tagger = PyModule_Create(&taggerDef);

  PyGILState_Release(gstate);
}
