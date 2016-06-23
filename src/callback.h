#ifndef CALLBACK_H
#define CALLBack_H

#include "jsaudio.h"

class JsPaStreamCallbackBridge : public AsyncWorker {
public:
  explicit JsPaStreamCallbackBridge(Callback *callback_);
  ~JsPaStreamCallbackBridge();
  
  int sendToCallback(const void* input, unsigned long frameCount);
  void dispatchJSCallback();
  
  void consumeAudioData(void* output, unsigned long frameCount);
  
private:
  NAN_INLINE static NAUV_WORK_CB(UVCallback) {
    JsPaStreamCallbackBridge *callback =
            static_cast<JsPaStreamCallbackBridge*>(async->data);
    callback->dispatchJSCallback();
  }
  void Execute() {}
  
  uv_async_t *async;
  uv_mutex_t async_lock;
  unsigned long m_frameCount;
  void* m_inputBuffer;
  void* m_outputBuffer;
};

#endif