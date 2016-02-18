/**
   JsAudio : Node Bindings for PortAudio.

   JsAudio : API Header File

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

#ifndef __PAMODULE_H__
#define __PAMODULE_H__

/* version */
static JsObject *pa_get_version(JsObject *self, JsObject *args);

static JsObject *pa_get_version_text(JsObject *self, JsObject *args);

/* framework init */
static JsObject *pa_initialize(JsObject *self, JsObject *args);

static JsObject *pa_terminate(JsObject *self, JsObject *args);

/* host api */
static JsObject *pa_get_host_api_count(JsObject *self, JsObject *args);

static JsObject *pa_get_default_host_api(JsObject *self, JsObject *args);

static JsObject *pa_host_api_type_id_to_host_api_index(JsObject *self, JsObject *args);

static JsObject *pa_host_api_device_index_to_device_index(JsObject *self, JsObject *args);

static JsObject *pa_get_host_api_info(JsObject *self, JsObject *args);

/* device api */
static JsObject *pa_get_device_count(JsObject *self, JsObject *args);

static JsObject *pa_get_default_input_device(JsObject *self, JsObject *args);

static JsObject *pa_get_default_output_device(JsObject *self, JsObject *args);

static JsObject *pa_get_device_info(JsObject *self, JsObject *args);

/* stream open/close */

static JsObject *pa_open(JsObject *self, JsObject *args, JsObject *kwargs);

static JsObject *pa_close(JsObject *self, JsObject *args);

static JsObject *pa_get_sample_size(JsObject *self, JsObject *args);

static JsObject *pa_is_format_supported(JsObject *self, JsObject *args,
                                        JsObject *kwargs);

/* stream start/stop/info */

static JsObject *pa_start_stream(JsObject *self, JsObject *args);

static JsObject *pa_stop_stream(JsObject *self, JsObject *args);

static JsObject *pa_abort_stream(JsObject *self, JsObject *args);

static JsObject *pa_is_stream_stopped(JsObject *self, JsObject *args);

static JsObject *pa_is_stream_active(JsObject *self, JsObject *args);

static JsObject *pa_get_stream_time(JsObject *self, JsObject *args);

static JsObject *pa_get_stream_cpu_load(JsObject *self, JsObject *args);

/* stream write/read */

static JsObject *pa_write_stream(JsObject *self, JsObject *args);

static JsObject *pa_read_stream(JsObject *self, JsObject *args);

static JsObject *pa_get_stream_write_available(JsObject *self, JsObject *args);

static JsObject *pa_get_stream_read_available(JsObject *self, JsObject *args);

#endif
