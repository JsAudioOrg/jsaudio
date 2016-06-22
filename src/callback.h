#ifndef CALLBACK_H
#define CALLBack_H

#include "jsaudio.h"

class JsPaStreamCallback : public AsyncWorker {
public:
  explicit JsPaStreamCallback(Callback *callback_);
  ~JsPaStreamCallback();
  
  int sendToCallback(unsigned long frameCount);
  void dispatchJSCallback();
  
  void* consumeAudioData(unsigned long frameCount);
  
private:
  NAN_INLINE static NAUV_WORK_CB(UVCallback) {
    JsPaStreamCallback *callback =
            static_cast<JsPaStreamCallback*>(async->data);
    callback->dispatchJSCallback();
  }
  void Execute() {}
  
  uv_async_t *async;
  uv_mutex_t async_lock;
};

#endif