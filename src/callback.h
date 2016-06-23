#ifndef CALLBACK_H
#define CALLBack_H

#include "jsaudio.h"

class JsPaStreamCallbackBridge : public AsyncWorker {
public:
  explicit JsPaStreamCallbackBridge(Callback *callback_);
  ~JsPaStreamCallbackBridge();
  
  int sendToCallback(unsigned long frameCount);
  void dispatchJSCallback();
  
  void* consumeAudioData(unsigned long frameCount);
  
private:
  NAN_INLINE static NAUV_WORK_CB(UVCallback) {
    JsPaStreamCallbackBridge *callback =
            static_cast<JsPaStreamCallbackBridge*>(async->data);
    callback->dispatchJSCallback();
  }
  void Execute() {}
  
  uv_async_t *async;
  uv_mutex_t async_lock;
};

#endif