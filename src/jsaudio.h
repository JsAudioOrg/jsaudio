/*
  http://portaudio.com/docs/v19-doxydocs/api_overview.html
*/
#ifndef JSAUDIO_H
#define JSAUDIO_H


/* BEGIN Setup */
#include "portaudio.h"
#include <nan.h>
#ifdef _WIN32
  #include "pa_asio.h"
#endif

using namespace Nan;
using Isolate = v8::Isolate;
using String = v8::String;
using Number = v8::Number;
using Object = v8::Object;
using Value = v8::Value;
using Function = v8::Function;
using LocalString = v8::Local<String>;
using LocalNumber = v8::Local<Number>;
using LocalObject = v8::Local<Object>;
using LocalValue = v8::Local<Value>;
using LocalFunction = v8::Local<Function>;
using MaybeLocalValue = v8::MaybeLocal<Value>;

#endif