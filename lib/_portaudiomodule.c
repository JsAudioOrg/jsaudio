/**
   JsAudio : Node Bindings for PortAudio.

   MIT License
   Copyright (c) 2016 Andrew Carpenter

   Permission is hereby granted, free of charge, to any person obtaining a copy of
   this software and associated documentation files (the "Software"), to deal in
   the Software without restriction, including without limitation the rights to
   use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
   the Software, and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
   FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
   COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_object_wrap.h>
#include <stdlib.h>
#include <stdio.h>
#include "portaudio.h"
#include "_portaudiomodule.h"

#ifdef MACOSX
# include "pa_mac_core.h"
#endif

#define DEFAULT_FRAMES_PER_BUFFER 1024
/* #define VERBOSE */

#define min(a, b)           \
  ({                        \
    __typeof__(a)_a = (a); \
    __typeof__(b)_b = (b); \
    _a < _b ? _a : _b;      \
  })

/************************************************************
*
* Table of Contents
*
* I. Exportable PortAudio Method Definitions
* II. V8 Object Wrappers
*     - PaDeviceInfo
*     - PaHostInfo
*     - PaStream
* III. PortAudio Method Implementations
*     - Initialization/Termination
*     - HostAPI
*     - DeviceAPI
*     - Stream Open/Close
*     - Stream Start/Stop/Info
*     - Stream Read/Write
* IV. V8 Module Init
*     - PaHostApiTypeId enum constants
*
************************************************************/

/************************************************************
*
* I. Exportable V8 Methods
*
************************************************************/

static JsMethodDef paMethods[] = {
  /* version */
  {"get_version", pa_get_version, METH_VARARGS, "get version"},
  {"get_version_text", pa_get_version_text, METH_VARARGS, "get version text"},

  /* inits */
  {"initialize", pa_initialize, METH_VARARGS, "initialize portaudio"},
  {"terminate", pa_terminate, METH_VARARGS, "terminate portaudio"},

  /* host api */
  {"get_host_api_count", pa_get_host_api_count, METH_VARARGS,
   "get host API count"},

  {"get_default_host_api", pa_get_default_host_api, METH_VARARGS,
   "get default host API index"},

  {"host_api_type_id_to_host_api_index",
   pa_host_api_type_id_to_host_api_index, METH_VARARGS,
   "get default host API index"},

  {"host_api_device_index_to_device_index",
   pa_host_api_device_index_to_device_index, METH_VARARGS,
   "get default host API index"},

  {"get_host_api_info", pa_get_host_api_info, METH_VARARGS,
   "get host api information"},

  /* device api */
  {"get_device_count", pa_get_device_count, METH_VARARGS,
   "get host API count"},

  {"get_default_input_device", pa_get_default_input_device, METH_VARARGS,
   "get default input device index"},

  {"get_default_output_device", pa_get_default_output_device, METH_VARARGS,
   "get default output device index"},

  {"get_device_info", pa_get_device_info, METH_VARARGS,
   "get device information"},

  /* stream open/close */
  {"open", (JsCFunction) pa_open, METH_VARARGS | METH_KEYWORDS,
   "open port audio stream"},
  {"close", pa_close, METH_VARARGS, "close port audio stream"},
  {"get_sample_size", pa_get_sample_size, METH_VARARGS,
   "get sample size of a format in bytes"},
  {"is_format_supported", (JsCFunction) pa_is_format_supported,
   METH_VARARGS | METH_KEYWORDS,
   "returns whether specified format is supported"},

  /* stream start/stop */
  {"start_stream", pa_start_stream, METH_VARARGS, "starts port audio stream"},
  {"stop_stream", pa_stop_stream, METH_VARARGS, "stops  port audio stream"},
  {"abort_stream", pa_abort_stream, METH_VARARGS, "aborts port audio stream"},
  {"is_stream_stopped", pa_is_stream_stopped, METH_VARARGS,
   "returns whether stream is stopped"},
  {"is_stream_active", pa_is_stream_active, METH_VARARGS,
   "returns whether stream is active"},
  {"get_stream_time", pa_get_stream_time, METH_VARARGS,
   "returns stream time"},
  {"get_stream_cpu_load", pa_get_stream_cpu_load, METH_VARARGS,
   "returns stream CPU load -- always 0 for blocking mode"},

  /* stream read/write */
  {"write_stream", pa_write_stream, METH_VARARGS, "write to stream"},
  {"read_stream", pa_read_stream, METH_VARARGS, "read from stream"},

  {"get_stream_write_available", pa_get_stream_write_available, METH_VARARGS,
   "get buffer available for writing"},

  {"get_stream_read_available", pa_get_stream_read_available, METH_VARARGS,
   "get buffer available for reading"},

  {NULL, NULL, 0, NULL}
};

/************************************************************
*
* II. V8 Object Wrappers
*
************************************************************/

/*************************************************************
* PaDeviceInfo Type : V8 object wrapper for PaDeviceInfo
*************************************************************/

typedef struct
{
  // clang-format off
  JsObject_HEAD
  PaDeviceInfo *devInfo;
  // clang-format on
} _jsAudio_paDeviceInfo;

static JsObject *_jsAudio_paDeviceInfo_get_structVersion(
  _jsAudio_paDeviceInfo *self, void *closure)
{
  if (!self->devInfo)
    {
      JsErr_SetString(JsExc_AttributeError, "No Device Info available");
      return NULL;
    }

  return JsLong_FromLong(self->devInfo->structVersion);
}

static JsObject *_jsAudio_paDeviceInfo_get_name(_jsAudio_paDeviceInfo *self,
                                                void *closure)
{
  if ((!self->devInfo) || (self->devInfo->name == NULL))
    {
      JsErr_SetString(JsExc_AttributeError, "No Device Info available");
      return NULL;
    }

  return JsBytes_FromString(self->devInfo->name);
}

static JsObject *_jsAudio_paDeviceInfo_get_hostApi(_jsAudio_paDeviceInfo *self,
                                                   void *closure)
{
  if (!self->devInfo)
    {
      JsErr_SetString(JsExc_AttributeError, "No Device Info available");
      return NULL;
    }

  return JsLong_FromLong(self->devInfo->hostApi);
}

static JsObject *_jsAudio_paDeviceInfo_get_maxInputChannels(
  _jsAudio_paDeviceInfo *self, void *closure)
{
  if (!self->devInfo)
    {
      JsErr_SetString(JsExc_AttributeError, "No Device Info available");
      return NULL;
    }

  return JsLong_FromLong(self->devInfo->maxInputChannels);
}

static JsObject *_jsAudio_paDeviceInfo_get_maxOutputChannels(
  _jsAudio_paDeviceInfo *self, void *closure)
{
  if (!self->devInfo)
    {
      JsErr_SetString(JsExc_AttributeError, "No Device Info available");
      return NULL;
    }

  return JsLong_FromLong(self->devInfo->maxOutputChannels);
}

static JsObject *_jsAudio_paDeviceInfo_get_defaultLowInputLatency(
  _jsAudio_paDeviceInfo *self, void *closure)
{
  if (!self->devInfo)
    {
      JsErr_SetString(JsExc_AttributeError, "No Device Info available");
      return NULL;
    }

  return JsFloat_FromDouble(self->devInfo->defaultLowInputLatency);
}

static JsObject *_jsAudio_paDeviceInfo_get_defaultLowOutputLatency(
  _jsAudio_paDeviceInfo *self, void *closure)
{
  if (!self->devInfo)
    {
      JsErr_SetString(JsExc_AttributeError, "No Device Info available");
      return NULL;
    }

  return JsFloat_FromDouble(self->devInfo->defaultLowOutputLatency);
}

static JsObject *_jsAudio_paDeviceInfo_get_defaultHighInputLatency(
  _jsAudio_paDeviceInfo *self, void *closure)
{
  if (!self->devInfo)
    {
      JsErr_SetString(JsExc_AttributeError, "No Device Info available");
      return NULL;
    }

  return JsFloat_FromDouble(self->devInfo->defaultHighInputLatency);
}

static JsObject *_jsAudio_paDeviceInfo_get_defaultHighOutputLatency(
  _jsAudio_paDeviceInfo *self, void *closure)
{
  if (!self->devInfo)
    {
      JsErr_SetString(JsExc_AttributeError, "No Device Info available");
      return NULL;
    }

  return JsFloat_FromDouble(self->devInfo->defaultHighOutputLatency);
}

static JsObject *_jsAudio_paDeviceInfo_get_defaultSampleRate(
  _jsAudio_paDeviceInfo *self, void *closure)
{
  if (!self->devInfo)
    {
      JsErr_SetString(JsExc_AttributeError, "No Device Info available");
      return NULL;
    }

  return JsFloat_FromDouble(self->devInfo->defaultSampleRate);
}

static int _jsAudio_paDeviceInfo_antiset(_jsAudio_paDeviceInfo *self,
                                         JsObject *value, void *closure)
{
  /* read-only: do not allow users to change values */
  JsErr_SetString(JsExc_AttributeError,
                  "Fields read-only: cannot modify values");
  return -1;
}

static JsGetSetDef _jsAudio_paDeviceInfo_getseters[] = {
  {"name", (getter) _jsAudio_paDeviceInfo_get_name,
   (setter) _jsAudio_paDeviceInfo_antiset, "device name", NULL},

  {"structVersion", (getter) _jsAudio_paDeviceInfo_get_structVersion,
   (setter) _jsAudio_paDeviceInfo_antiset, "struct version", NULL},

  {"hostApi", (getter) _jsAudio_paDeviceInfo_get_hostApi,
   (setter) _jsAudio_paDeviceInfo_antiset, "host api index", NULL},

  {"maxInputChannels", (getter) _jsAudio_paDeviceInfo_get_maxInputChannels,
   (setter) _jsAudio_paDeviceInfo_antiset, "max input channels", NULL},

  {"maxOutputChannels", (getter) _jsAudio_paDeviceInfo_get_maxOutputChannels,
   (setter) _jsAudio_paDeviceInfo_antiset, "max output channels", NULL},

  {"defaultLowInputLatency",
   (getter) _jsAudio_paDeviceInfo_get_defaultLowInputLatency,
   (setter) _jsAudio_paDeviceInfo_antiset, "default low input latency", NULL},

  {"defaultLowOutputLatency",
   (getter) _jsAudio_paDeviceInfo_get_defaultLowOutputLatency,
   (setter) _jsAudio_paDeviceInfo_antiset, "default low output latency", NULL},

  {"defaultHighInputLatency",
   (getter) _jsAudio_paDeviceInfo_get_defaultHighInputLatency,
   (setter) _jsAudio_paDeviceInfo_antiset, "default high input latency", NULL},

  {"defaultHighOutputLatency",
   (getter) _jsAudio_paDeviceInfo_get_defaultHighOutputLatency,
   (setter) _jsAudio_paDeviceInfo_antiset, "default high output latency",
   NULL},

  {"defaultSampleRate", (getter) _jsAudio_paDeviceInfo_get_defaultSampleRate,
   (setter) _jsAudio_paDeviceInfo_antiset, "default sample rate", NULL},

  {NULL}
};

static void _jsAudio_paDeviceInfo_dealloc(_jsAudio_paDeviceInfo *self)
{
  self->devInfo = NULL;
  Js_TYPE(self)->tp_free((JsObject *) self);
}

static JsTypeObject _jsAudio_paDeviceInfoType = {
  // clang-format off
  JsVarObject_HEAD_INIT(NULL, 0)
  // clang-format on
  "_portaudio.paDeviceInfo",                 /*tp_name*/
  sizeof(_jsAudio_paDeviceInfo),             /*tp_basicsize*/
  0,                                         /*tp_itemsize*/
  (destructor) _jsAudio_paDeviceInfo_dealloc, /*tp_dealloc*/
  0,                                         /*tp_print*/
  0,                                         /*tp_getattr*/
  0,                                         /*tp_setattr*/
  0,                                         /*tp_compare*/
  0,                                         /*tp_repr*/
  0,                                         /*tp_as_number*/
  0,                                         /*tp_as_sequence*/
  0,                                         /*tp_as_mapping*/
  0,                                         /*tp_hash */
  0,                                         /*tp_call*/
  0,                                         /*tp_str*/
  0,                                         /*tp_getattro*/
  0,                                         /*tp_setattro*/
  0,                                         /*tp_as_buffer*/
  Js_TPFLAGS_DEFAULT,                        /*tp_flags*/
  "Port Audio Device Info",                  /* tp_doc */
  0,                                         /* tp_traverse */
  0,                                         /* tp_clear */
  0,                                         /* tp_richcompare */
  0,                                         /* tp_weaklistoffset */
  0,                                         /* tp_iter */
  0,                                         /* tp_iternext */
  0,                                         /* tp_methods */
  0,                                         /* tp_members */
  _jsAudio_paDeviceInfo_getseters,           /* tp_getset */
  0,                                         /* tp_base */
  0,                                         /* tp_dict */
  0,                                         /* tp_descr_get */
  0,                                         /* tp_descr_set */
  0,                                         /* tp_dictoffset */
  0,                                         /* tp_init */
  0,                                         /* tp_alloc */
  0,                                         /* tp_new */
};

static _jsAudio_paDeviceInfo *_create_paDeviceInfo_object(void)
{
  _jsAudio_paDeviceInfo *obj;

  /* don't allow subclassing */
  obj = (_jsAudio_paDeviceInfo *) JsObject_New(_jsAudio_paDeviceInfo,
                                               &_jsAudio_paDeviceInfoType);
  return obj;
}

/*************************************************************
* PaHostApi Info V8 Object
*************************************************************/

typedef struct
{
  // clang-format off
  JsObject_HEAD
  // clang-format on
  PaHostApiInfo *apiInfo;
} _jsAudio_paHostApiInfo;

static JsObject *_jsAudio_paHostApiInfo_get_structVersion(
  _jsAudio_paHostApiInfo *self, void *closure)
{
  if ((!self->apiInfo))
    {
      JsErr_SetString(JsExc_AttributeError, "No HostApi Info available");
      return NULL;
    }

  return JsLong_FromLong(self->apiInfo->structVersion);
}

static JsObject *_jsAudio_paHostApiInfo_get_type(_jsAudio_paHostApiInfo *self,
                                                 void *closure)
{
  if ((!self->apiInfo))
    {
      JsErr_SetString(JsExc_AttributeError, "No HostApi Info available");
      return NULL;
    }

  return JsLong_FromLong((long) self->apiInfo->type);
}

static JsObject *_jsAudio_paHostApiInfo_get_name(_jsAudio_paHostApiInfo *self,
                                                 void *closure)
{
  if ((!self->apiInfo) || (self->apiInfo->name == NULL))
    {
      JsErr_SetString(JsExc_AttributeError, "No HostApi Info available");
      return NULL;
    }

  return JsUnicode_FromString(self->apiInfo->name);
}

static JsObject *_jsAudio_paHostApiInfo_get_deviceCount(
  _jsAudio_paHostApiInfo *self, void *closure)
{
  if ((!self->apiInfo))
    {
      JsErr_SetString(JsExc_AttributeError, "No HostApi Info available");
      return NULL;
    }

  return JsLong_FromLong(self->apiInfo->deviceCount);
}

static JsObject *_jsAudio_paHostApiInfo_get_defaultInputDevice(
  _jsAudio_paHostApiInfo *self, void *closure)
{
  if ((!self->apiInfo))
    {
      JsErr_SetString(JsExc_AttributeError, "No HostApi Info available");
      return NULL;
    }

  return JsLong_FromLong(self->apiInfo->defaultInputDevice);
}

static JsObject *_jsAudio_paHostApiInfo_get_defaultOutputDevice(
  _jsAudio_paHostApiInfo *self, void *closure)
{
  if ((!self->apiInfo))
    {
      JsErr_SetString(JsExc_AttributeError, "No HostApi Info available");
      return NULL;
    }

  return JsLong_FromLong(self->apiInfo->defaultOutputDevice);
}

static int _jsAudio_paHostApiInfo_antiset(_jsAudio_paDeviceInfo *self,
                                          JsObject *value, void *closure)
{
  /* read-only: do not allow users to change values */
  JsErr_SetString(JsExc_AttributeError,
                  "Fields read-only: cannot modify values");
  return -1;
}

static void _jsAudio_paHostApiInfo_dealloc(_jsAudio_paHostApiInfo *self)
{
  self->apiInfo = NULL;
  Js_TYPE(self)->tp_free((JsObject *) self);
}

static JsGetSetDef _jsAudio_paHostApiInfo_getseters[] = {
  {"name", (getter) _jsAudio_paHostApiInfo_get_name,
   (setter) _jsAudio_paHostApiInfo_antiset, "host api name", NULL},

  {"structVersion", (getter) _jsAudio_paHostApiInfo_get_structVersion,
   (setter) _jsAudio_paHostApiInfo_antiset, "struct version", NULL},

  {"type", (getter) _jsAudio_paHostApiInfo_get_type,
   (setter) _jsAudio_paHostApiInfo_antiset, "host api type", NULL},

  {"deviceCount", (getter) _jsAudio_paHostApiInfo_get_deviceCount,
   (setter) _jsAudio_paHostApiInfo_antiset, "number of devices", NULL},

  {"defaultInputDevice",
   (getter) _jsAudio_paHostApiInfo_get_defaultInputDevice,
   (setter) _jsAudio_paHostApiInfo_antiset, "default input device index",
   NULL},

  {"defaultOutputDevice",
   (getter) _jsAudio_paHostApiInfo_get_defaultOutputDevice,
   (setter) _jsAudio_paDeviceInfo_antiset, "default output device index",
   NULL},

  {NULL}
};

static JsTypeObject _jsAudio_paHostApiInfoType = {
  // clang-format off
  JsVarObject_HEAD_INIT(NULL, 0)
  // clang-format on
  "_portaudio.paHostApiInfo",                 /*tp_name*/
  sizeof(_jsAudio_paHostApiInfo),             /*tp_basicsize*/
  0,                                          /*tp_itemsize*/
  (destructor) _jsAudio_paHostApiInfo_dealloc, /*tp_dealloc*/
  0,                                          /*tp_print*/
  0,                                          /*tp_getattr*/
  0,                                          /*tp_setattr*/
  0,                                          /*tp_compare*/
  0,                                          /*tp_repr*/
  0,                                          /*tp_as_number*/
  0,                                          /*tp_as_sequence*/
  0,                                          /*tp_as_mapping*/
  0,                                          /*tp_hash */
  0,                                          /*tp_call*/
  0,                                          /*tp_str*/
  0,                                          /*tp_getattro*/
  0,                                          /*tp_setattro*/
  0,                                          /*tp_as_buffer*/
  Js_TPFLAGS_DEFAULT,                         /*tp_flags*/
  "Port Audio HostApi Info",                  /* tp_doc */
  0,                                          /* tp_traverse */
  0,                                          /* tp_clear */
  0,                                          /* tp_richcompare */
  0,                                          /* tp_weaklistoffset */
  0,                                          /* tp_iter */
  0,                                          /* tp_iternext */
  0,                                          /* tp_methods */
  0,                                          /* tp_members */
  _jsAudio_paHostApiInfo_getseters,           /* tp_getset */
  0,                                          /* tp_base */
  0,                                          /* tp_dict */
  0,                                          /* tp_descr_get */
  0,                                          /* tp_descr_set */
  0,                                          /* tp_dictoffset */
  0,                                          /* tp_init */
  0,                                          /* tp_alloc */
  0,                                          /* tp_new */
};

static _jsAudio_paHostApiInfo *_create_paHostApiInfo_object(void)
{
  _jsAudio_paHostApiInfo *obj;

  /* don't allow subclassing */
  obj = (_jsAudio_paHostApiInfo *) JsObject_New(_jsAudio_paHostApiInfo,
                                                &_jsAudio_paHostApiInfoType);
  return obj;
}

/*************************************************************
* Host-Specific Objects
*************************************************************/

/*************************************************************
* --> Mac OS X
*************************************************************/

#ifdef MACOSX
typedef struct
{
  // clang-format off
  JsObject_HEAD
  // clang-format on
  PaMacCoreStreamInfo *paMacCoreStreamInfo;
  int flags;
  SInt32 *channelMap;
  int channelMapSize;
} _jsAudio_MacOSX_hostApiSpecificStreamInfo;

typedef _jsAudio_MacOSX_hostApiSpecificStreamInfo _jsAudio_Mac_HASSI;

static void _jsAudio_MacOSX_hostApiSpecificStreamInfo_cleanup(
  _jsAudio_Mac_HASSI *self)
{
  if (self->paMacCoreStreamInfo != NULL)
    {
      free(self->paMacCoreStreamInfo);
      self->paMacCoreStreamInfo = NULL;
    }

  if (self->channelMap != NULL)
    {
      free(self->channelMap);
      self->channelMap = NULL;
    }

  self->flags = paMacCorePlayNice;
  self->channelMapSize = 0;
}

static void _jsAudio_MacOSX_hostApiSpecificStreamInfo_dealloc(
  _jsAudio_Mac_HASSI *self)
{
  _jsAudio_MacOSX_hostApiSpecificStreamInfo_cleanup(self);
  Js_TYPE(self)->tp_free((JsObject *) self);
}

static int _jsAudio_MacOSX_hostApiSpecificStreamInfo_init(JsObject *_self,
                                                          JsObject *args,
                                                          JsObject *kwargs)
{
  _jsAudio_Mac_HASSI *self = (_jsAudio_Mac_HASSI *) _self;
  JsObject *channel_map = NULL;
  int flags = paMacCorePlayNice;

  static char *kwlist[] = {"flags", "channel_map", NULL};

  if (!JsArg_ParseTupleAndKeywords(args, kwargs, "|iO", kwlist, &flags,
                                   &channel_map))
    {
      return -1;
    }

  _jsAudio_MacOSX_hostApiSpecificStreamInfo_cleanup(self);

  if (channel_map != NULL)
    {
      if (!JsTuple_Check(channel_map))
        {
          JsErr_SetString(JsExc_ValueError, "Channel map must be a tuple");
          return -1;
        }

      // generate SInt32 channelMap
      self->channelMapSize = (int) JsTuple_Size(channel_map);
      self->channelMap = (SInt32 *) malloc(sizeof(SInt32) * self->channelMapSize);

      if (self->channelMap == NULL)
        {
          JsErr_SetString(JsExc_SystemError, "Out of memory");
          _jsAudio_MacOSX_hostApiSpecificStreamInfo_cleanup(self);
          return -1;
        }

      JsObject *element;
      int i;
      for (i = 0; i < self->channelMapSize; ++i)
        {
          element = JsTuple_GetItem(channel_map, i);
          if (element == NULL)
            {
              JsErr_SetString(JsExc_ValueError,
                              "Internal error: out of bounds index");
              _jsAudio_MacOSX_hostApiSpecificStreamInfo_cleanup(self);
              return -1;
            }

          if (!JsNumber_Check(element))
            {
              JsErr_SetString(JsExc_ValueError,
                              "Channel Map must consist of integer elements");
              _jsAudio_MacOSX_hostApiSpecificStreamInfo_cleanup(self);
              return -1;
            }

          JsObject *long_element = JsNumber_Long(element);
          self->channelMap[i] = (SInt32) JsLong_AsLong(long_element);
          Js_DECREF(long_element);
        }
    }

  self->paMacCoreStreamInfo
    = (PaMacCoreStreamInfo *) malloc(sizeof(PaMacCoreStreamInfo));

  if (self->paMacCoreStreamInfo == NULL)
    {
      JsErr_SetString(JsExc_SystemError, "Out of memeory");
      _jsAudio_MacOSX_hostApiSpecificStreamInfo_cleanup(self);
      return -1;
    }

  PaMacCore_SetupStreamInfo(self->paMacCoreStreamInfo, flags);

  if (self->channelMap)
    {
      PaMacCore_SetupChannelMap(self->paMacCoreStreamInfo, self->channelMap,
                                self->channelMapSize);
    }

  self->flags = flags;
  return 0;
}

static JsObject *_jsAudio_MacOSX_hostApiSpecificStreamInfo_get_flags(
  _jsAudio_Mac_HASSI *self, void *closure)
{
  return JsLong_FromLong(self->flags);
}

static JsObject *_jsAudio_MacOSX_hostApiSpecificStreamInfo_get_channel_map(
  _jsAudio_Mac_HASSI *self, void *closure)
{
  if (self->channelMap == NULL || self->channelMapSize == 0)
    {
      Js_INCREF(Js_None);
      return Js_None;
    }

  int i;
  JsObject *channelMapTuple = JsTuple_New(self->channelMapSize);
  for (i = 0; i < self->channelMapSize; ++i)
    {
      JsObject *element = JsLong_FromLong(self->channelMap[i]);
      if (!element)
        {
          JsErr_SetString(JsExc_SystemError, "Invalid channel map");
          return NULL;
        }

      if (JsTuple_SetItem(channelMapTuple, i,
                          JsLong_FromLong(self->channelMap[i])))
        {
          // non-zero on error
          JsErr_SetString(JsExc_SystemError, "Can't create channel map.");
          return NULL;
        }
    }
  return channelMapTuple;
}

static int _jsAudio_MacOSX_hostApiSpecificStreamInfo_antiset(
  _jsAudio_Mac_HASSI *self, JsObject *value, void *closure)
{
  /* read-only: do not allow users to change values */
  JsErr_SetString(JsExc_AttributeError,
                  "Fields read-only: cannot modify values");
  return -1;
}

static JsGetSetDef _jsAudio_MacOSX_hostApiSpecificStreamInfo_getseters[] = {
  {"flags", (getter) _jsAudio_MacOSX_hostApiSpecificStreamInfo_get_flags,
   (setter) _jsAudio_MacOSX_hostApiSpecificStreamInfo_antiset, "flags", NULL},

  {"channel_map",
   (getter) _jsAudio_MacOSX_hostApiSpecificStreamInfo_get_channel_map,
   (setter) _jsAudio_MacOSX_hostApiSpecificStreamInfo_antiset, "channel map",
   NULL},

  {NULL}
};

static JsTypeObject _jsAudio_MacOSX_hostApiSpecificStreamInfoType = {
  // clang-format off
  JsVarObject_HEAD_INIT(NULL, 0)
  // clang-format on
  "_portaudio.PaMacCoreStreamInfo",                  /*tp_name*/
  sizeof(_jsAudio_MacOSX_hostApiSpecificStreamInfo), /*tp_basicsize*/
  0,                                                 /*tp_itemsize*/
  /*tp_dealloc*/
  (destructor) _jsAudio_MacOSX_hostApiSpecificStreamInfo_dealloc,
  0,                                                   /*tp_print*/
  0,                                                   /*tp_getattr*/
  0,                                                   /*tp_setattr*/
  0,                                                   /*tp_compare*/
  0,                                                   /*tp_repr*/
  0,                                                   /*tp_as_number*/
  0,                                                   /*tp_as_sequence*/
  0,                                                   /*tp_as_mapping*/
  0,                                                   /*tp_hash */
  0,                                                   /*tp_call*/
  0,                                                   /*tp_str*/
  0,                                                   /*tp_getattro*/
  0,                                                   /*tp_setattro*/
  0,                                                   /*tp_as_buffer*/
  Js_TPFLAGS_DEFAULT,                                  /*tp_flags*/
  "Mac OS X Specific HostAPI configuration",           /* tp_doc */
  0,                                                   /* tp_traverse */
  0,                                                   /* tp_clear */
  0,                                                   /* tp_richcompare */
  0,                                                   /* tp_weaklistoffset */
  0,                                                   /* tp_iter */
  0,                                                   /* tp_iternext */
  0,                                                   /* tp_methods */
  0,                                                   /* tp_members */
  _jsAudio_MacOSX_hostApiSpecificStreamInfo_getseters, /* tp_getset */
  0,                                                   /* tp_base */
  0,                                                   /* tp_dict */
  0,                                                   /* tp_descr_get */
  0,                                                   /* tp_descr_set */
  0,                                                   /* tp_dictoffset */
  (int (*)(JsObject *, JsObject *, JsObject *))
  _jsAudio_MacOSX_hostApiSpecificStreamInfo_init, /* tp_init */
  0,                                                  /* tp_alloc */
  0,                                                  /* tp_new */
};
#endif

/*************************************************************
* Stream Wrapper V8 Object
*************************************************************/

typedef struct
{
  JsObject *callback;
  long main_thread_id;
  unsigned int frame_size;
} JsAudioCallbackContext;

typedef struct
{
  // clang-format off
  JsObject_HEAD
  // clang-format on
  PaStream *stream;
  PaStreamParameters *inputParameters;
  PaStreamParameters *outputParameters;
  PaStreamInfo *streamInfo;
  JsAudioCallbackContext *callbackContext;
  int is_open;
} _jsAudio_Stream;

static int _is_open(_jsAudio_Stream *obj)
{
  return (obj) && (obj->is_open);
}

static void _cleanup_Stream_object(_jsAudio_Stream *streamObject)
{
  if (streamObject->stream != NULL)
    {
      // clang-format off
      Js_BEGIN_ALLOW_THREADS Pa_CloseStream(streamObject->stream);
      Js_END_ALLOW_THREADS
      // clang-format on
      streamObject->stream = NULL;
    }

  if (streamObject->streamInfo)
    streamObject->streamInfo = NULL;

  if (streamObject->inputParameters != NULL)
    {
      free(streamObject->inputParameters);
      streamObject->inputParameters = NULL;
    }

  if (streamObject->outputParameters != NULL)
    {
      free(streamObject->outputParameters);
      streamObject->outputParameters = NULL;
    }

  if (streamObject->callbackContext != NULL)
    {
      Js_XDECREF(streamObject->callbackContext->callback);
      free(streamObject->callbackContext);
      streamObject->callbackContext = NULL;
    }

  streamObject->is_open = 0;
}

static void _jsAudio_Stream_dealloc(_jsAudio_Stream *self)
{
  _cleanup_Stream_object(self);
  Js_TYPE(self)->tp_free((JsObject *) self);
}

static JsObject *_jsAudio_Stream_get_structVersion(_jsAudio_Stream *self,
                                                   void *closure)
{
  if (!_is_open(self))
    {
      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", paBadStreamPtr, "Stream closed"));
      return NULL;
    }

  if ((!self->streamInfo))
    {
      JsErr_SetObject(JsExc_IOError, Js_BuildValue("(i,s)", paBadStreamPtr,
                                                   "No StreamInfo available"));
      return NULL;
    }

  return JsLong_FromLong(self->streamInfo->structVersion);
}

static JsObject *_jsAudio_Stream_get_inputLatency(_jsAudio_Stream *self,
                                                  void *closure)
{
  if (!_is_open(self))
    {
      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", paBadStreamPtr, "Stream closed"));
      return NULL;
    }

  if ((!self->streamInfo))
    {
      JsErr_SetObject(JsExc_IOError, Js_BuildValue("(i,s)", paBadStreamPtr,
                                                   "No StreamInfo available"));
      return NULL;
    }

  return JsFloat_FromDouble(self->streamInfo->inputLatency);
}

static JsObject *_jsAudio_Stream_get_outputLatency(_jsAudio_Stream *self,
                                                   void *closure)
{
  if (!_is_open(self))
    {
      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", paBadStreamPtr, "Stream closed"));
      return NULL;
    }

  if ((!self->streamInfo))
    {
      JsErr_SetObject(JsExc_IOError, Js_BuildValue("(i,s)", paBadStreamPtr,
                                                   "No StreamInfo available"));
      return NULL;
    }

  return JsFloat_FromDouble(self->streamInfo->outputLatency);
}

static JsObject *_jsAudio_Stream_get_sampleRate(_jsAudio_Stream *self,
                                                void *closure)
{
  if (!_is_open(self))
    {
      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", paBadStreamPtr, "Stream closed"));
      return NULL;
    }

  if ((!self->streamInfo))
    {
      JsErr_SetObject(JsExc_IOError, Js_BuildValue("(i,s)", paBadStreamPtr,
                                                   "No StreamInfo available"));
      return NULL;
    }

  return JsFloat_FromDouble(self->streamInfo->sampleRate);
}

static int _jsAudio_Stream_antiset(_jsAudio_Stream *self, JsObject *value,
                                   void *closure)
{
  /* read-only: do not allow users to change values */
  JsErr_SetString(JsExc_AttributeError,
                  "Fields read-only: cannot modify values");
  return -1;
}

static JsGetSetDef _jsAudio_Stream_getseters[] = {
  {"structVersion", (getter) _jsAudio_Stream_get_structVersion,
   (setter) _jsAudio_Stream_antiset, "struct version", NULL},

  {"inputLatency", (getter) _jsAudio_Stream_get_inputLatency,
   (setter) _jsAudio_Stream_antiset, "input latency", NULL},

  {"outputLatency", (getter) _jsAudio_Stream_get_outputLatency,
   (setter) _jsAudio_Stream_antiset, "output latency", NULL},

  {"sampleRate", (getter) _jsAudio_Stream_get_sampleRate,
   (setter) _jsAudio_Stream_antiset, "sample rate", NULL},

  {NULL}
};

static JsTypeObject _jsAudio_StreamType = {
  // clang-format off
  JsVarObject_HEAD_INIT(NULL, 0)
  // clang-format on
  "_portaudio.Stream",                 /*tp_name*/
  sizeof(_jsAudio_Stream),             /*tp_basicsize*/
  0,                                   /*tp_itemsize*/
  (destructor) _jsAudio_Stream_dealloc, /*tp_dealloc*/
  0,                                   /*tp_print*/
  0,                                   /*tp_getattr*/
  0,                                   /*tp_setattr*/
  0,                                   /*tp_compare*/
  0,                                   /*tp_repr*/
  0,                                   /*tp_as_number*/
  0,                                   /*tp_as_sequence*/
  0,                                   /*tp_as_mapping*/
  0,                                   /*tp_hash */
  0,                                   /*tp_call*/
  0,                                   /*tp_str*/
  0,                                   /*tp_getattro*/
  0,                                   /*tp_setattro*/
  0,                                   /*tp_as_buffer*/
  Js_TPFLAGS_DEFAULT,                  /*tp_flags*/
  "Port Audio Stream",                 /* tp_doc */
  0,                                   /* tp_traverse */
  0,                                   /* tp_clear */
  0,                                   /* tp_richcompare */
  0,                                   /* tp_weaklistoffset */
  0,                                   /* tp_iter */
  0,                                   /* tp_iternext */
  0,                                   /* tp_methods */
  0,                                   /* tp_members */
  _jsAudio_Stream_getseters,           /* tp_getset */
  0,                                   /* tp_base */
  0,                                   /* tp_dict */
  0,                                   /* tp_descr_get */
  0,                                   /* tp_descr_set */
  0,                                   /* tp_dictoffset */
  0,                                   /* tp_init */
  0,                                   /* tp_alloc */
  0,                                   /* tp_new */
};

static _jsAudio_Stream *_create_Stream_object(void)
{
  _jsAudio_Stream *obj;

  /* don't allow subclassing */
  obj = (_jsAudio_Stream *) JsObject_New(_jsAudio_Stream, &_jsAudio_StreamType);
  return obj;
}

/************************************************************
*
* III. PortAudio Method Implementations
*
************************************************************/

/*************************************************************
* Version Info
*************************************************************/

static JsObject *pa_get_version(JsObject *self, JsObject *args)
{
  if (!JsArg_ParseTuple(args, ""))
    {
      return NULL;
    }

  return JsLong_FromLong(Pa_GetVersion());
}

static JsObject *pa_get_version_text(JsObject *self, JsObject *args)
{
  if (!JsArg_ParseTuple(args, ""))
    {
      return NULL;
    }

  return JsUnicode_FromString(Pa_GetVersionText());
}

/*************************************************************
* Initialization/Termination
*************************************************************/

static JsObject *pa_initialize(JsObject *self, JsObject *args)
{
  int err;
  err = Pa_Initialize();
  if (err != paNoError)
    {
      Pa_Terminate();
#ifdef VERBOSE
      fprintf(stderr, "An error occured while using the portaudio stream\n");
      fprintf(stderr, "Error number: %d\n", err);
      fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
#endif
      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", err, Pa_GetErrorText(err)));
      return NULL;
    }

  Js_INCREF(Js_None);
  return Js_None;
}

static JsObject *pa_terminate(JsObject *self, JsObject *args)
{
  Pa_Terminate();
  Js_INCREF(Js_None);
  return Js_None;
}

/*************************************************************
* HostAPI
*************************************************************/

static JsObject *pa_get_host_api_count(JsObject *self, JsObject *args)
{
  PaHostApiIndex count;

  if (!JsArg_ParseTuple(args, ""))
    {
      return NULL;
    }

  count = Pa_GetHostApiCount();

  if (count < 0)
    {
#ifdef VERBOSE
      fprintf(stderr, "An error occured while using the portaudio stream\n");
      fprintf(stderr, "Error number: %d\n", count);
      fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(count));
#endif

      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", count, Pa_GetErrorText(count)));
      return NULL;
    }

  return JsLong_FromLong(count);
}

static JsObject *pa_get_default_host_api(JsObject *self, JsObject *args)
{
  PaHostApiIndex index;

  if (!JsArg_ParseTuple(args, ""))
    {
      return NULL;
    }

  index = Pa_GetDefaultHostApi();

  if (index < 0)
    {
#ifdef VERBOSE
      fprintf(stderr, "An error occured while using the portaudio stream\n");
      fprintf(stderr, "Error number: %d\n", index);
      fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(index));
#endif

      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", index, Pa_GetErrorText(index)));
      return NULL;
    }

  return JsLong_FromLong(index);
}

static JsObject *pa_host_api_type_id_to_host_api_index(JsObject *self,
                                                       JsObject *args)
{
  PaHostApiTypeId typeid;
  PaHostApiIndex index;

  if (!JsArg_ParseTuple(args, "i", &typeid ))
    {
      return NULL;
    }

  index = Pa_HostApiTypeIdToHostApiIndex(typeid );

  if (index < 0)
    {
#ifdef VERBOSE
      fprintf(stderr, "An error occured while using the portaudio stream\n");
      fprintf(stderr, "Error number: %d\n", index);
      fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(index));
#endif

      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", index, Pa_GetErrorText(index)));
      return NULL;
    }

  return JsLong_FromLong(index);
}

static JsObject *pa_host_api_device_index_to_device_index(JsObject *self,
                                                          JsObject *args)
{
  PaHostApiIndex apiIndex;
  int hostApiDeviceindex;
  PaDeviceIndex devIndex;

  if (!JsArg_ParseTuple(args, "ii", &apiIndex, &hostApiDeviceindex))
    {
      return NULL;
    }

  devIndex = Pa_HostApiDeviceIndexToDeviceIndex(apiIndex, hostApiDeviceindex);
  if (devIndex < 0)
    {
#ifdef VERBOSE
      fprintf(stderr, "An error occured while using the portaudio stream\n");
      fprintf(stderr, "Error number: %d\n", devIndex);
      fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(devIndex));
#endif

      JsErr_SetObject(JsExc_IOError, Js_BuildValue("(i,s)", devIndex,
                                                   Pa_GetErrorText(devIndex)));
      return NULL;
    }

  return JsLong_FromLong(devIndex);
}

static JsObject *pa_get_host_api_info(JsObject *self, JsObject *args)
{
  PaHostApiIndex index;
  PaHostApiInfo *_info;
  _jsAudio_paHostApiInfo *js_info;

  if (!JsArg_ParseTuple(args, "i", &index))
    {
      return NULL;
    }

  _info = (PaHostApiInfo *) Pa_GetHostApiInfo(index);
  if (!_info)
    {
      JsErr_SetObject(JsExc_IOError, Js_BuildValue("(i,s)", paInvalidHostApi,
                                                   "Invalid host api info"));
      return NULL;
    }

  js_info = _create_paHostApiInfo_object();
  js_info->apiInfo = _info;
  return (JsObject *) js_info;
}

/*************************************************************
* Device API
*************************************************************/

static JsObject *pa_get_device_count(JsObject *self, JsObject *args)
{
  PaDeviceIndex count;

  if (!JsArg_ParseTuple(args, ""))
    {
      return NULL;
    }

  count = Pa_GetDeviceCount();
  if (count < 0)
    {
#ifdef VERBOSE
      fprintf(stderr, "An error occured while using the portaudio stream\n");
      fprintf(stderr, "Error number: %d\n", count);
      fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(count));
#endif

      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", count, Pa_GetErrorText(count)));
      return NULL;
    }

  return JsLong_FromLong(count);
}

static JsObject *pa_get_default_input_device(JsObject *self, JsObject *args)
{
  PaDeviceIndex index;

  if (!JsArg_ParseTuple(args, ""))
    {
      return NULL;
    }

  index = Pa_GetDefaultInputDevice();
  if (index == paNoDevice)
    {
      JsErr_SetString(JsExc_IOError, "No Default Input Device Available");
      return NULL;
    }
  else if (index < 0)
    {
#ifdef VERBOSE
      fprintf(stderr, "An error occured while using the portaudio stream\n");
      fprintf(stderr, "Error number: %d\n", index);
      fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(index));
#endif

      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", index, Pa_GetErrorText(index)));
      return NULL;
    }

  return JsLong_FromLong(index);
}

static JsObject *pa_get_default_output_device(JsObject *self, JsObject *args)
{
  PaDeviceIndex index;

  if (!JsArg_ParseTuple(args, ""))
    {
      return NULL;
    }

  index = Pa_GetDefaultOutputDevice();
  if (index == paNoDevice)
    {
      JsErr_SetString(JsExc_IOError, "No Default Output Device Available");
      return NULL;
    }
  else if (index < 0)
    {
#ifdef VERBOSE
      fprintf(stderr, "An error occured while using the portaudio stream\n");
      fprintf(stderr, "Error number: %d\n", index);
      fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(index));
#endif

      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", index, Pa_GetErrorText(index)));
      return NULL;
    }

  return JsLong_FromLong(index);
}

static JsObject *pa_get_device_info(JsObject *self, JsObject *args)
{
  PaDeviceIndex index;
  PaDeviceInfo *_info;
  _jsAudio_paDeviceInfo *js_info;

  if (!JsArg_ParseTuple(args, "i", &index))
    {
      return NULL;
    }

  _info = (PaDeviceInfo *) Pa_GetDeviceInfo(index);
  if (!_info)
    {
      JsErr_SetObject(JsExc_IOError, Js_BuildValue("(i,s)", paInvalidDevice,
                                                   "Invalid device info"));
      return NULL;
    }

  js_info = _create_paDeviceInfo_object();
  js_info->devInfo = _info;
  return (JsObject *) js_info;
}

/*************************************************************
* Stream Open / Close / Supported
*************************************************************/

int _stream_callback_cfunction(const void *input, void *output,
                               unsigned long frameCount,
                               const PaStreamCallbackTimeInfo *timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void *userData)
{
  int return_val = paAbort;
  JsGILState_STATE _state = JsGILState_Ensure();

#ifdef VERBOSE
  if (statusFlags != 0)
    {
      printf("Status flag set: ");
      if (statusFlags & paInputUnderflow)
        {
          printf("input underflow!\n");
        }
      if (statusFlags & paInputOverflow)
        {
          printf("input overflow!\n");
        }
      if (statusFlags & paOutputUnderflow)
        {
          printf("output underflow!\n");
        }
      if (statusFlags & paOutputUnderflow)
        {
          printf("output overflow!\n");
        }
      if (statusFlags & paPrimingOutput)
        {
          printf("priming output!\n");
        }
    }
#endif

  JsAudioCallbackContext *context = (JsAudioCallbackContext *) userData;
  JsObject *js_callback = context->callback;
  unsigned int bytes_per_frame = context->frame_size;
  long main_thread_id = context->main_thread_id;

  JsObject *js_frame_count = JsLong_FromUnsignedLong(frameCount);
  // clang-format off
  JsObject *js_time_info = Js_BuildValue("{s:d,s:d,s:d}",
                                         "input_buffer_adc_time",
                                         timeInfo->inputBufferAdcTime,
                                         "current_time",
                                         timeInfo->currentTime,
                                         "output_buffer_dac_time",
                                         timeInfo->outputBufferDacTime);
  // clang-format on
  JsObject *js_status_flags = JsLong_FromUnsignedLong(statusFlags);
  JsObject *js_input_data = Js_None;
  const char *pData;
  int output_len;
  JsObject *js_result;

  if (input)
    {
      js_input_data
        = JsBytes_FromStringAndSize(input, bytes_per_frame * frameCount);
    }

  js_result
    = JsObject_CallFunctionObjArgs(js_callback, js_input_data, js_frame_count,
                                   js_time_info, js_status_flags, NULL);

  if (js_result == NULL)
    {
#ifdef VERBOSE
      fprintf(stderr, "An error occured while using the portaudio stream\n");
      fprintf(stderr, "Error message: Could not call callback function\n");
#endif
      JsObject *err = JsErr_Occurred();

      if (err)
        {
          JsThreadState_SetAsyncExc(main_thread_id, err);
          // Print out a stack trace to help debugging.
          // TODO: make VERBOSE a runtime flag so users can control
          // the amount of logging.
          JsErr_Print();
        }

      goto end;
    }

  // clang-format off
  if (!JsArg_ParseTuple(js_result,
                        "z#i",
                        &pData,
                        &output_len,
                        &return_val))
    {
// clang-format on
#ifdef VERBOSE
      fprintf(stderr, "An error occured while using the portaudio stream\n");
      fprintf(stderr, "Error message: Could not parse callback return value\n");
#endif

      JsObject *err = JsErr_Occurred();

      if (err)
        {
          JsThreadState_SetAsyncExc(main_thread_id, err);
          // Print out a stack trace to help debugging.
          // TODO: make VERBOSE a runtime flag so users can control
          // the amount of logging.
          JsErr_Print();
        }

      Js_XDECREF(js_result);
      return_val = paAbort;
      goto end;
    }

  Js_DECREF(js_result);

  if ((return_val != paComplete) && (return_val != paAbort)
      && (return_val != paContinue))
    {
      JsErr_SetString(JsExc_ValueError,
                      "Invalid PaStreamCallbackResult from callback");
      JsThreadState_SetAsyncExc(main_thread_id, JsErr_Occurred());
      JsErr_Print();

      // Quit the callback loop
      return_val = paAbort;
      goto end;
    }

  // Copy bytes for playback only if this is an output stream:
  if (output)
    {
      char *output_data = (char *) output;
      memcpy(output_data, pData, min(output_len, bytes_per_frame * frameCount));
      // Pad out the rest of the buffer with 0s if callback returned
      // too few frames (and assume paComplete).
      if (output_len < (frameCount * bytes_per_frame))
        {
          memset(output_data + output_len, 0,
                 (frameCount * bytes_per_frame) - output_len);
          return_val = paComplete;
        }
    }

end:
  if (input)
    {
      // Decrement this at the end, after memcpy, in case the user
      // returns js_input_data back for playback.
      Js_DECREF(js_input_data);
    }

  Js_XDECREF(js_frame_count);
  Js_XDECREF(js_time_info);
  Js_XDECREF(js_status_flags);

  JsGILState_Release(_state);
  return return_val;
}

static JsObject *pa_open(JsObject *self, JsObject *args, JsObject *kwargs)
{
  int rate, channels;
  int input, output, frames_per_buffer;
  int input_device_index = -1;
  int output_device_index = -1;
  JsObject *input_device_index_arg = NULL;
  JsObject *output_device_index_arg = NULL;
  JsObject *stream_callback = NULL;
  PaSampleFormat format;
  PaError err;
  JsObject *input_device_index_long;
  JsObject *output_device_index_long;
  PaStreamParameters *outputParameters = NULL;
  PaStreamParameters *inputParameters = NULL;
  PaStream *stream = NULL;
  PaStreamInfo *streamInfo = NULL;
  JsAudioCallbackContext *context = NULL;
  _jsAudio_Stream *streamObject;

  static char *kwlist[] = {"rate",
                           "channels",
                           "format",
                           "input",
                           "output",
                           "input_device_index",
                           "output_device_index",
                           "frames_per_buffer",
                           "input_host_api_specific_stream_info",
                           "output_host_api_specific_stream_info",
                           "stream_callback",
                           NULL};

#ifdef MACOSX
  _jsAudio_MacOSX_hostApiSpecificStreamInfo *inputHostSpecificStreamInfo = NULL;
  _jsAudio_MacOSX_hostApiSpecificStreamInfo *outputHostSpecificStreamInfo
    = NULL;
#else
  /* mostly ignored...*/
  JsObject *inputHostSpecificStreamInfo = NULL;
  JsObject *outputHostSpecificStreamInfo = NULL;
#endif

  /* default to neither output nor input */
  input = 0;
  output = 0;
  frames_per_buffer = DEFAULT_FRAMES_PER_BUFFER;

  // clang-format off
  if (!JsArg_ParseTupleAndKeywords(args, kwargs,
#ifdef MACOSX
                                   "iik|iiOOiO!O!O",
#else
                                   "iik|iiOOiOOO",
#endif
                                   kwlist,
                                   &rate, &channels, &format,
                                   &input, &output,
                                   &input_device_index_arg,
                                   &output_device_index_arg,
                                   &frames_per_buffer,
#ifdef MACOSX
                                   &_jsAudio_MacOSX_hostApiSpecificStreamInfoType,
#endif
                                   &inputHostSpecificStreamInfo,
#ifdef MACOSX
                                   &_jsAudio_MacOSX_hostApiSpecificStreamInfoType,
#endif
                                   &outputHostSpecificStreamInfo,
                                   &stream_callback))
    {

      return NULL;
    }
  // clang-format on

  if (stream_callback && (JsCallable_Check(stream_callback) == 0))
    {
      JsErr_SetString(JsExc_TypeError, "stream_callback must be callable");
      return NULL;
    }

  if ((input_device_index_arg == NULL) || (input_device_index_arg == Js_None))
    {
#ifdef VERBOSE
      printf("Using default input device\n");
#endif

      input_device_index = -1;
    }
  else
    {
      if (!JsNumber_Check(input_device_index_arg))
        {
          JsErr_SetString(JsExc_ValueError,
                          "input_device_index must be integer (or None)");
          return NULL;
        }

      input_device_index_long = JsNumber_Long(input_device_index_arg);

      input_device_index = (int) JsLong_AsLong(input_device_index_long);
      Js_DECREF(input_device_index_long);

#ifdef VERBOSE
      printf("Using input device index number: %d\n", input_device_index);
#endif
    }

  if ((output_device_index_arg == NULL)
      || (output_device_index_arg == Js_None))
    {
#ifdef VERBOSE
      printf("Using default output device\n");
#endif

      output_device_index = -1;
    }
  else
    {
      if (!JsNumber_Check(output_device_index_arg))
        {
          JsErr_SetString(JsExc_ValueError,
                          "output_device_index must be integer (or None)");
          return NULL;
        }

      output_device_index_long = JsNumber_Long(output_device_index_arg);
      output_device_index = (int) JsLong_AsLong(output_device_index_long);
      Js_DECREF(output_device_index_long);

#ifdef VERBOSE
      printf("Using output device index number: %d\n", output_device_index);
#endif
    }

  if (input == 0 && output == 0)
    {
      JsErr_SetString(JsExc_ValueError, "Must specify either input or output");
      return NULL;
    }

  if (channels < 1)
    {
      JsErr_SetString(JsExc_ValueError, "Invalid audio channels");
      return NULL;
    }

  if (output)
    {
      outputParameters = (PaStreamParameters *) malloc(sizeof(PaStreamParameters));

      if (output_device_index < 0)
        {
          outputParameters->device = Pa_GetDefaultOutputDevice();
        }
      else
        {
          outputParameters->device = output_device_index;
        }

      /* final check -- ensure that there is a default device */
      if (outputParameters->device < 0
          || outputParameters->device >= Pa_GetDeviceCount())
        {
          free(outputParameters);
          JsErr_SetObject(JsExc_IOError,
                          Js_BuildValue("(i,s)", paInvalidDevice,
                                        "Invalid output device "
                                        "(no default output device)"));
          return NULL;
        }

      outputParameters->channelCount = channels;
      outputParameters->sampleFormat = format;
      outputParameters->suggestedLatency
        = Pa_GetDeviceInfo(outputParameters->device)->defaultLowOutputLatency;
      outputParameters->hostApiSpecificStreamInfo = NULL;

#ifdef MACOSX
      if (outputHostSpecificStreamInfo)
        {
          outputParameters->hostApiSpecificStreamInfo
            = outputHostSpecificStreamInfo->paMacCoreStreamInfo;
        }
#endif
    }

  if (input)
    {
      inputParameters = (PaStreamParameters *) malloc(sizeof(PaStreamParameters));

      if (input_device_index < 0)
        {
          inputParameters->device = Pa_GetDefaultInputDevice();
        }
      else
        {
          inputParameters->device = input_device_index;
        }

      /* final check -- ensure that there is a default device */
      if (inputParameters->device < 0)
        {
          free(inputParameters);
          JsErr_SetObject(JsExc_IOError,
                          Js_BuildValue("(i,s)", paInvalidDevice,
                                        "Invalid input device "
                                        "(no default output device)"));
          return NULL;
        }

      inputParameters->channelCount = channels;
      inputParameters->sampleFormat = format;
      inputParameters->suggestedLatency
        = Pa_GetDeviceInfo(inputParameters->device)->defaultLowInputLatency;
      inputParameters->hostApiSpecificStreamInfo = NULL;

#ifdef MACOSX
      if (inputHostSpecificStreamInfo)
        {
          inputParameters->hostApiSpecificStreamInfo
            = inputHostSpecificStreamInfo->paMacCoreStreamInfo;
        }
#endif
    }

  if (stream_callback)
    {
      Js_INCREF(stream_callback);
      context = (JsAudioCallbackContext *) malloc(sizeof(JsAudioCallbackContext));
      context->callback = (JsObject *) stream_callback;
      context->main_thread_id = JsThreadState_Get()->thread_id;
      context->frame_size = Pa_GetSampleSize(format) * channels;
    }

  err = Pa_OpenStream(&stream,
                      /* input/output parameters */
                      /* NULL values are ignored */
                      inputParameters, outputParameters,
                      /* samples per second */
                      rate,
                      /* frames in the buffer */
                      frames_per_buffer,
                      /* we won't output out of range samples
                         so don't bother clipping them */
                      paClipOff,
                      /* callback, if specified */
                      (stream_callback) ? (_stream_callback_cfunction) : (NULL),
                      /* callback userData, if applicable */
                      context);

  if (err != paNoError)
    {
#ifdef VERBOSE
      fprintf(stderr, "An error occured while using the portaudio stream\n");
      fprintf(stderr, "Error number: %d\n", err);
      fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
#endif

      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", err, Pa_GetErrorText(err)));
      return NULL;
    }

  streamInfo = (PaStreamInfo *) Pa_GetStreamInfo(stream);
  if (!streamInfo)
    {
      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", paInternalError,
                                    "Could not get stream information"));

      return NULL;
    }

  streamObject = _create_Stream_object();
  streamObject->stream = stream;
  streamObject->inputParameters = inputParameters;
  streamObject->outputParameters = outputParameters;
  streamObject->is_open = 1;
  streamObject->streamInfo = streamInfo;
  streamObject->callbackContext = context;
  return (JsObject *) streamObject;
}

static JsObject *pa_close(JsObject *self, JsObject *args)
{
  JsObject *stream_arg;
  _jsAudio_Stream *streamObject;

  if (!JsArg_ParseTuple(args, "O!", &_jsAudio_StreamType, &stream_arg))
    {
      return NULL;
    }

  streamObject = (_jsAudio_Stream *) stream_arg;

  _cleanup_Stream_object(streamObject);

  Js_INCREF(Js_None);
  return Js_None;
}

static JsObject *pa_get_sample_size(JsObject *self, JsObject *args)
{
  PaSampleFormat format;
  int size_in_bytes;

  if (!JsArg_ParseTuple(args, "k", &format))
    {
      return NULL;
    }

  size_in_bytes = Pa_GetSampleSize(format);

  if (size_in_bytes < 0)
    {
      JsErr_SetObject(
        JsExc_ValueError,
        Js_BuildValue("(s,i)", Pa_GetErrorText(size_in_bytes), size_in_bytes));
      return NULL;
    }

  return JsLong_FromLong(size_in_bytes);
}

static JsObject *pa_is_format_supported(JsObject *self, JsObject *args,
                                        JsObject *kwargs)
{
  // clang-format off
  static char *kwlist[] = {
    "sample_rate",
    "input_device",
    "input_channels",
    "input_format",
    "output_device",
    "output_channels",
    "output_format",
    NULL
  };
  // clang-format on

  int input_device, input_channels;
  int output_device, output_channels;
  float sample_rate;
  PaStreamParameters inputParams;
  PaStreamParameters outputParams;
  PaSampleFormat input_format, output_format;
  PaError error;

  input_device = input_channels = output_device = output_channels = -1;

  input_format = output_format = -1;

  // clang-format off
  if (!JsArg_ParseTupleAndKeywords(args, kwargs, "f|iikiik", kwlist,
                                   &sample_rate,
                                   &input_device,
                                   &input_channels,
                                   &input_format,
                                   &output_device,
                                   &output_channels,
                                   &output_format))
    {
      return NULL;
    }
  // clang-format on

  if (!(input_device < 0))
    {
      inputParams.device = input_device;
      inputParams.channelCount = input_channels;
      inputParams.sampleFormat = input_format;
      inputParams.suggestedLatency = 0;
      inputParams.hostApiSpecificStreamInfo = NULL;
    }

  if (!(output_device < 0))
    {
      outputParams.device = output_device;
      outputParams.channelCount = output_channels;
      outputParams.sampleFormat = output_format;
      outputParams.suggestedLatency = 0;
      outputParams.hostApiSpecificStreamInfo = NULL;
    }

  error = Pa_IsFormatSupported((input_device < 0) ? NULL : &inputParams,
                               (output_device < 0) ? NULL : &outputParams,
                               sample_rate);

  if (error == paFormatIsSupported)
    {
      Js_INCREF(Js_True);
      return Js_True;
    }
  else
    {
      JsErr_SetObject(JsExc_ValueError,
                      Js_BuildValue("(s,i)", Pa_GetErrorText(error), error));
      return NULL;
    }
}

/*************************************************************
* Stream Start / Stop / Info
*************************************************************/

static JsObject *pa_start_stream(JsObject *self, JsObject *args)
{
  int err;
  JsObject *stream_arg;
  _jsAudio_Stream *streamObject;
  PaStream *stream;

  if (!JsArg_ParseTuple(args, "O!", &_jsAudio_StreamType, &stream_arg))
    {
      return NULL;
    }

  streamObject = (_jsAudio_Stream *) stream_arg;

  if (!_is_open(streamObject))
    {
      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", paBadStreamPtr, "Stream closed"));
      return NULL;
    }

  stream = streamObject->stream;

  if (((err = Pa_StartStream(stream)) != paNoError)
      && (err != paStreamIsNotStopped))
    {
      _cleanup_Stream_object(streamObject);

#ifdef VERBOSE
      fprintf(stderr, "An error occured while using the portaudio stream\n");
      fprintf(stderr, "Error number: %d\n", err);
      fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
#endif

      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", err, Pa_GetErrorText(err)));
      return NULL;
    }

  Js_INCREF(Js_None);
  return Js_None;
}

static JsObject *pa_stop_stream(JsObject *self, JsObject *args)
{
  int err;
  JsObject *stream_arg;
  _jsAudio_Stream *streamObject;
  PaStream *stream;

  if (!JsArg_ParseTuple(args, "O!", &_jsAudio_StreamType, &stream_arg))
    {
      return NULL;
    }

  streamObject = (_jsAudio_Stream *) stream_arg;

  if (!_is_open(streamObject))
    {
      JsErr_SetString(JsExc_IOError, "Stream not open");
      return NULL;
    }

  stream = streamObject->stream;

  // clang-format off
  Js_BEGIN_ALLOW_THREADS
    err = Pa_StopStream(stream);
  Js_END_ALLOW_THREADS
  // clang-format on

  if ((err != paNoError) && (err != paStreamIsStopped))
    {
      _cleanup_Stream_object(streamObject);

#ifdef VERBOSE
      fprintf(stderr, "An error occured while using the portaudio stream\n");
      fprintf(stderr, "Error number: %d\n", err);
      fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
#endif

      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", err, Pa_GetErrorText(err)));
      return NULL;
    }

  Js_INCREF(Js_None);
  return Js_None;
}

static JsObject *pa_abort_stream(JsObject *self, JsObject *args)
{
  int err;
  JsObject *stream_arg;
  _jsAudio_Stream *streamObject;
  PaStream *stream;

  if (!JsArg_ParseTuple(args, "O!", &_jsAudio_StreamType, &stream_arg))
    {
      return NULL;
    }

  streamObject = (_jsAudio_Stream *) stream_arg;

  if (!_is_open(streamObject))
    {
      JsErr_SetString(JsExc_IOError, "Stream not open");
      return NULL;
    }

  stream = streamObject->stream;

  // clang-format off
  Js_BEGIN_ALLOW_THREADS
    err = Pa_AbortStream(stream);
  Js_END_ALLOW_THREADS
  // clang-format on

  if ((err != paNoError) && (err != paStreamIsStopped))
    {
      _cleanup_Stream_object(streamObject);

#ifdef VERBOSE
      fprintf(stderr, "An error occured while using the portaudio stream\n");
      fprintf(stderr, "Error number: %d\n", err);
      fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
#endif

      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", err, Pa_GetErrorText(err)));
      return NULL;
    }

  Js_INCREF(Js_None);
  return Js_None;
}

static JsObject *pa_is_stream_stopped(JsObject *self, JsObject *args)
{
  int err;
  JsObject *stream_arg;
  _jsAudio_Stream *streamObject;
  PaStream *stream;

  if (!JsArg_ParseTuple(args, "O!", &_jsAudio_StreamType, &stream_arg))
    {
      return NULL;
    }

  streamObject = (_jsAudio_Stream *) stream_arg;

  if (!_is_open(streamObject))
    {
      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", paBadStreamPtr, "Stream closed"));
      return NULL;
    }

  stream = streamObject->stream;

  if ((err = Pa_IsStreamStopped(stream)) < 0)
    {
      _cleanup_Stream_object(streamObject);

#ifdef VERBOSE
      fprintf(stderr, "An error occured while using the portaudio stream\n");
      fprintf(stderr, "Error number: %d\n", err);
      fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
#endif

      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", err, Pa_GetErrorText(err)));
      return NULL;
    }

  if (err)
    {
      Js_INCREF(Js_True);
      return Js_True;
    }

  Js_INCREF(Js_False);
  return Js_False;
}

static JsObject *pa_is_stream_active(JsObject *self, JsObject *args)
{
  int err;
  JsObject *stream_arg;
  _jsAudio_Stream *streamObject;
  PaStream *stream;

  if (!JsArg_ParseTuple(args, "O!", &_jsAudio_StreamType, &stream_arg))
    {
      return NULL;
    }

  streamObject = (_jsAudio_Stream *) stream_arg;

  if (!_is_open(streamObject))
    {
      JsErr_SetString(JsExc_IOError, "Stream not open");
      return NULL;
    }

  stream = streamObject->stream;

  if ((err = Pa_IsStreamActive(stream)) < 0)
    {
      _cleanup_Stream_object(streamObject);

#ifdef VERBOSE
      fprintf(stderr, "An error occured while using the portaudio stream\n");
      fprintf(stderr, "Error number: %d\n", err);
      fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
#endif

      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", err, Pa_GetErrorText(err)));
      return NULL;
    }

  if (err)
    {
      Js_INCREF(Js_True);
      return Js_True;
    }

  Js_INCREF(Js_False);
  return Js_False;
}

static JsObject *pa_get_stream_time(JsObject *self, JsObject *args)
{
  double time;
  JsObject *stream_arg;
  _jsAudio_Stream *streamObject;
  PaStream *stream;

  if (!JsArg_ParseTuple(args, "O!", &_jsAudio_StreamType, &stream_arg))
    {
      return NULL;
    }

  streamObject = (_jsAudio_Stream *) stream_arg;

  if (!_is_open(streamObject))
    {
      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", paBadStreamPtr, "Stream closed"));
      return NULL;
    }

  stream = streamObject->stream;

  if ((time = Pa_GetStreamTime(stream)) == 0)
    {
      _cleanup_Stream_object(streamObject);
      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", paInternalError, "Internal Error"));
      return NULL;
    }

  return JsFloat_FromDouble(time);
}

static JsObject *pa_get_stream_cpu_load(JsObject *self, JsObject *args)
{
  JsObject *stream_arg;
  _jsAudio_Stream *streamObject;
  PaStream *stream;

  if (!JsArg_ParseTuple(args, "O!", &_jsAudio_StreamType, &stream_arg))
    {
      return NULL;
    }

  streamObject = (_jsAudio_Stream *) stream_arg;

  if (!_is_open(streamObject))
    {
      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", paBadStreamPtr, "Stream closed"));
      return NULL;
    }

  stream = streamObject->stream;
  return JsFloat_FromDouble(Pa_GetStreamCpuLoad(stream));
}

/*************************************************************
* Stream Read/Write
*************************************************************/

static JsObject *pa_write_stream(JsObject *self, JsObject *args)
{
  const char *data;
  int total_size;
  int total_frames;
  int err;
  int should_throw_exception = 0;

  JsObject *stream_arg;
  _jsAudio_Stream *streamObject;
  PaStream *stream;

  // clang-format off
  if (!JsArg_ParseTuple(args, "O!s#i|i",
                        &_jsAudio_StreamType,
                        &stream_arg,
                        &data,
                        &total_size,
                        &total_frames,
                        &should_throw_exception))
    {
      return NULL;
    }
  // clang-format on

  if (total_frames < 0)
    {
      JsErr_SetString(JsExc_ValueError, "Invalid number of frames");
      return NULL;
    }

  streamObject = (_jsAudio_Stream *) stream_arg;

  if (!_is_open(streamObject))
    {
      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", paBadStreamPtr, "Stream closed"));
      return NULL;
    }

  stream = streamObject->stream;

  // clang-format off
  Js_BEGIN_ALLOW_THREADS
    err = Pa_WriteStream(stream, data, total_frames);
  Js_END_ALLOW_THREADS
  // clang-format on

  if (err != paNoError)
    {
      if (err == paOutputUnderflowed)
        {
          if (should_throw_exception)
            {
              goto error;
            }
        }
      else
        goto error;
    }

  Js_INCREF(Js_None);
  return Js_None;

error:
  _cleanup_Stream_object(streamObject);

#ifdef VERBOSE
  fprintf(stderr, "An error occured while using the portaudio stream\n");
  fprintf(stderr, "Error number: %d\n", err);
  fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
#endif

  JsErr_SetObject(JsExc_IOError,
                  Js_BuildValue("(i,s)", err, Pa_GetErrorText(err)));
  return NULL;
}

static JsObject *pa_read_stream(JsObject *self, JsObject *args)
{
  int err;
  int total_frames;
  short *sampleBlock;
  int num_bytes;
  JsObject *rv;
  int should_raise_exception = 0;

  JsObject *stream_arg;
  _jsAudio_Stream *streamObject;
  PaStream *stream;
  PaStreamParameters *inputParameters;

  // clang-format off
  if (!JsArg_ParseTuple(args, "O!i|i",
                        &_jsAudio_StreamType,
                        &stream_arg,
                        &total_frames,
                        &should_raise_exception))
    {
      return NULL;
    }
  // clang-format on

  if (total_frames < 0)
    {
      JsErr_SetString(JsExc_ValueError, "Invalid number of frames");
      return NULL;
    }

  streamObject = (_jsAudio_Stream *) stream_arg;

  if (!_is_open(streamObject))
    {
      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", paBadStreamPtr, "Stream closed"));
      return NULL;
    }

  stream = streamObject->stream;
  inputParameters = streamObject->inputParameters;
  num_bytes = (total_frames) * (inputParameters->channelCount)
              *(Pa_GetSampleSize(inputParameters->sampleFormat));

#ifdef VERBOSE
  fprintf(stderr, "Allocating %d bytes\n", num_bytes);
#endif

  rv = JsBytes_FromStringAndSize(NULL, num_bytes);
  sampleBlock = (short *) JsBytes_AsString(rv);

  if (sampleBlock == NULL)
    {
      JsErr_SetObject(JsExc_IOError, Js_BuildValue("(i,s)", paInsufficientMemory,
                                                   "Out of memory"));
      return NULL;
    }

  // clang-format off
  Js_BEGIN_ALLOW_THREADS
    err = Pa_ReadStream(stream, sampleBlock, total_frames);
  Js_END_ALLOW_THREADS
  // clang-format on

  if (err != paNoError)
    {
      if (err == paInputOverflowed)
        {
          if (should_raise_exception)
            {
              goto error;
            }
        }
      else
        {
          goto error;
        }
    }

  return rv;

error:
  _cleanup_Stream_object(streamObject);
  Js_XDECREF(rv);
  JsErr_SetObject(JsExc_IOError,
                  Js_BuildValue("(i,s)", err, Pa_GetErrorText(err)));

#ifdef VERBOSE
  fprintf(stderr, "An error occured while using the portaudio stream\n");
  fprintf(stderr, "Error number: %d\n", err);
  fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
#endif

  return NULL;
}

static JsObject *pa_get_stream_write_available(JsObject *self, JsObject *args)
{
  signed long frames;
  JsObject *stream_arg;
  _jsAudio_Stream *streamObject;
  PaStream *stream;

  if (!JsArg_ParseTuple(args, "O!", &_jsAudio_StreamType, &stream_arg))
    {
      return NULL;
    }

  streamObject = (_jsAudio_Stream *) stream_arg;

  if (!_is_open(streamObject))
    {
      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", paBadStreamPtr, "Stream closed"));
      return NULL;
    }

  stream = streamObject->stream;
  frames = Pa_GetStreamWriteAvailable(stream);
  return JsLong_FromLong(frames);
}

static JsObject *pa_get_stream_read_available(JsObject *self, JsObject *args)
{
  signed long frames;
  JsObject *stream_arg;
  _jsAudio_Stream *streamObject;
  PaStream *stream;

  if (!JsArg_ParseTuple(args, "O!", &_jsAudio_StreamType, &stream_arg))
    {
      return NULL;
    }

  streamObject = (_jsAudio_Stream *) stream_arg;

  if (!_is_open(streamObject))
    {
      JsErr_SetObject(JsExc_IOError,
                      Js_BuildValue("(i,s)", paBadStreamPtr, "Stream closed"));
      return NULL;
    }

  stream = streamObject->stream;
  frames = Pa_GetStreamReadAvailable(stream);
  return JsLong_FromLong(frames);
}

/************************************************************
*
* IV. V8 Module Init
*
************************************************************/

#if PY_MAJOR_VERSION >= 3
# define ERROR_INIT NULL
#else
# define ERROR_INIT /**/
#endif

#if PY_MAJOR_VERSION >= 3
static struct JsModuleDef moduledef = {  //
  JsModuleDef_HEAD_INIT,
  "_portaudio",
  NULL,
  -1,
  paMethods,
  NULL,
  NULL,
  NULL,
  NULL
};
#endif

JsMODINIT_FUNC
#if PY_MAJOR_VERSION >= 3
JsInit__portaudio(void)
#else
init_portaudio(void)
#endif
{
  JsObject *m;

  JsEval_InitThreads();

  _jsAudio_StreamType.tp_new = JsType_GenericNew;
  if (JsType_Ready(&_jsAudio_StreamType) < 0)
    {
      return ERROR_INIT;
    }

  _jsAudio_paDeviceInfoType.tp_new = JsType_GenericNew;
  if (JsType_Ready(&_jsAudio_paDeviceInfoType) < 0)
    {
      return ERROR_INIT;
    }

  _jsAudio_paHostApiInfoType.tp_new = JsType_GenericNew;
  if (JsType_Ready(&_jsAudio_paHostApiInfoType) < 0)
    {
      return ERROR_INIT;
    }

#ifdef MACOSX
  _jsAudio_MacOSX_hostApiSpecificStreamInfoType.tp_new = JsType_GenericNew;
  if (JsType_Ready(&_jsAudio_MacOSX_hostApiSpecificStreamInfoType) < 0)
    {
      return ERROR_INIT;
    }
#endif

#if PY_MAJOR_VERSION >= 3
  m = JsModule_Create(&moduledef);
#else
  m = Js_InitModule("_portaudio", paMethods);
#endif

  Js_INCREF(&_jsAudio_StreamType);
  Js_INCREF(&_jsAudio_paDeviceInfoType);
  Js_INCREF(&_jsAudio_paHostApiInfoType);

#ifdef MACOSX
  Js_INCREF(&_jsAudio_MacOSX_hostApiSpecificStreamInfoType);
  JsModule_AddObject(
    m, "paMacCoreStreamInfo",
    (JsObject *) &_jsAudio_MacOSX_hostApiSpecificStreamInfoType);
#endif

  /* Add PortAudio constants */

  /* host apis */
  JsModule_AddIntConstant(m, "paInDevelopment", paInDevelopment);
  JsModule_AddIntConstant(m, "paDirectSound", paDirectSound);
  JsModule_AddIntConstant(m, "paMME", paMME);
  JsModule_AddIntConstant(m, "paASIO", paASIO);
  JsModule_AddIntConstant(m, "paSoundManager", paSoundManager);
  JsModule_AddIntConstant(m, "paCoreAudio", paCoreAudio);
  JsModule_AddIntConstant(m, "paOSS", paOSS);
  JsModule_AddIntConstant(m, "paALSA", paALSA);
  JsModule_AddIntConstant(m, "paAL", paAL);
  JsModule_AddIntConstant(m, "paBeOS", paBeOS);
  JsModule_AddIntConstant(m, "paWDMKS", paWDMKS);
  JsModule_AddIntConstant(m, "paJACK", paJACK);
  JsModule_AddIntConstant(m, "paWASAPI", paWASAPI);
  JsModule_AddIntConstant(m, "paNoDevice", paNoDevice);

  /* formats */
  JsModule_AddIntConstant(m, "paFloat32", paFloat32);
  JsModule_AddIntConstant(m, "paInt32", paInt32);
  JsModule_AddIntConstant(m, "paInt24", paInt24);
  JsModule_AddIntConstant(m, "paInt16", paInt16);
  JsModule_AddIntConstant(m, "paInt8", paInt8);
  JsModule_AddIntConstant(m, "paUInt8", paUInt8);
  JsModule_AddIntConstant(m, "paCustomFormat", paCustomFormat);

  /* error codes */
  JsModule_AddIntConstant(m, "paNoError", paNoError);
  JsModule_AddIntConstant(m, "paNotInitialized", paNotInitialized);
  JsModule_AddIntConstant(m, "paUnanticipatedHostError",
                          paUnanticipatedHostError);
  JsModule_AddIntConstant(m, "paInvalidChannelCount", paInvalidChannelCount);
  JsModule_AddIntConstant(m, "paInvalidSampleRate", paInvalidSampleRate);
  JsModule_AddIntConstant(m, "paInvalidDevice", paInvalidDevice);
  JsModule_AddIntConstant(m, "paInvalidFlag", paInvalidFlag);
  JsModule_AddIntConstant(m, "paSampleFormatNotSupported",
                          paSampleFormatNotSupported);
  JsModule_AddIntConstant(m, "paBadIODeviceCombination",
                          paBadIODeviceCombination);
  JsModule_AddIntConstant(m, "paInsufficientMemory", paInsufficientMemory);
  JsModule_AddIntConstant(m, "paBufferTooBig", paBufferTooBig);
  JsModule_AddIntConstant(m, "paBufferTooSmall", paBufferTooSmall);
  JsModule_AddIntConstant(m, "paNullCallback", paNullCallback);
  JsModule_AddIntConstant(m, "paBadStreamPtr", paBadStreamPtr);
  JsModule_AddIntConstant(m, "paTimedOut", paTimedOut);
  JsModule_AddIntConstant(m, "paInternalError", paInternalError);
  JsModule_AddIntConstant(m, "paDeviceUnavailable", paDeviceUnavailable);
  JsModule_AddIntConstant(m, "paIncompatibleHostApiSpecificStreamInfo",
                          paIncompatibleHostApiSpecificStreamInfo);
  JsModule_AddIntConstant(m, "paStreamIsStopped", paStreamIsStopped);
  JsModule_AddIntConstant(m, "paStreamIsNotStopped", paStreamIsNotStopped);
  JsModule_AddIntConstant(m, "paInputOverflowed", paInputOverflowed);
  JsModule_AddIntConstant(m, "paOutputUnderflowed", paOutputUnderflowed);
  JsModule_AddIntConstant(m, "paHostApiNotFound", paHostApiNotFound);
  JsModule_AddIntConstant(m, "paInvalidHostApi", paInvalidHostApi);
  JsModule_AddIntConstant(m, "paCanNotReadFromACallbackStream",
                          paCanNotReadFromACallbackStream);
  JsModule_AddIntConstant(m, "paCanNotWriteToACallbackStream",
                          paCanNotWriteToACallbackStream);
  JsModule_AddIntConstant(m, "paCanNotReadFromAnOutputOnlyStream",
                          paCanNotReadFromAnOutputOnlyStream);
  JsModule_AddIntConstant(m, "paCanNotWriteToAnInputOnlyStream",
                          paCanNotWriteToAnInputOnlyStream);
  JsModule_AddIntConstant(m, "paIncompatibleStreamHostApi",
                          paIncompatibleStreamHostApi);

  /* callback constants */
  JsModule_AddIntConstant(m, "paContinue", paContinue);
  JsModule_AddIntConstant(m, "paComplete", paComplete);
  JsModule_AddIntConstant(m, "paAbort", paAbort);

  /* callback status flags */
  JsModule_AddIntConstant(m, "paInputUnderflow", paInputUnderflow);
  JsModule_AddIntConstant(m, "paInputOverflow", paInputOverflow);
  JsModule_AddIntConstant(m, "paOutputUnderflow", paOutputUnderflow);
  JsModule_AddIntConstant(m, "paOutputOverflow", paOutputOverflow);
  JsModule_AddIntConstant(m, "paPrimingOutput", paPrimingOutput);

#ifdef MACOSX
  JsModule_AddIntConstant(m, "paMacCoreChangeDeviceParameters",
                          paMacCoreChangeDeviceParameters);
  JsModule_AddIntConstant(m, "paMacCoreFailIfConversionRequired",
                          paMacCoreFailIfConversionRequired);
  JsModule_AddIntConstant(m, "paMacCoreConversionQualityMin",
                          paMacCoreConversionQualityMin);
  JsModule_AddIntConstant(m, "paMacCoreConversionQualityMedium",
                          paMacCoreConversionQualityMedium);
  JsModule_AddIntConstant(m, "paMacCoreConversionQualityLow",
                          paMacCoreConversionQualityLow);
  JsModule_AddIntConstant(m, "paMacCoreConversionQualityHigh",
                          paMacCoreConversionQualityHigh);
  JsModule_AddIntConstant(m, "paMacCoreConversionQualityMax",
                          paMacCoreConversionQualityMax);
  JsModule_AddIntConstant(m, "paMacCorePlayNice", paMacCorePlayNice);
  JsModule_AddIntConstant(m, "paMacCorePro", paMacCorePro);
  JsModule_AddIntConstant(m, "paMacCoreMinimizeCPUButPlayNice",
                          paMacCoreMinimizeCPUButPlayNice);
  JsModule_AddIntConstant(m, "paMacCoreMinimizeCPU", paMacCoreMinimizeCPU);
#endif

#if PY_MAJOR_VERSION >= 3
  return m;
#endif
}
