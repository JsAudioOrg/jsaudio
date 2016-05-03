#ifndef STREAM_H
#define STREAM_H

#include "jsaudio.h"

class StreamContainer : public ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

 private:
  explicit MyObject(double value = 0);
  ~MyObject();

  static void New(const FunctionCallbackInfo<v8::Value>& info);
  static Persistent<v8::Function> constructor;
  PaStream *stream;
};

#endif