// Unless explicitly stated otherwise all files in this repository are licensed
// under the Apache License Version 2.0.
// This product includes software developed at Datadog (https://www.datadoghq.com/).
// Copyright 2018 Datadog, Inc.

// +build cpython,kubelet

#include "kubeutil.h"

// Functions
PyObject* GetKubeletConnectionInfo();

static PyMethodDef kubeutilMethods[] = {
  {"get_connection_info", GetKubeletConnectionInfo, METH_NOARGS, "Get kubelet connection information."},
  {NULL, NULL}
};

static struct PyModuleDef kubeutilDef = {
  PyModuleDef_HEAD_INIT,
  "kubeutil",        /* m_name */
  "kubeutil module", /* m_doc */
  -1,                /* m_size */
  kubeutilMethods,   /* m_methods */
  NULL,              /* m_reload */
  NULL,              /* m_traverse */
  NULL,              /* m_clear */
  NULL,              /* m_free */
};

void initkubeutil()
{
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();

  PyObject *ku = PyModule_Create(&kubeutilDef);

  PyGILState_Release(gstate);
}
