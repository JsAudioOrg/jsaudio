#ifndef CALLBACK_H
#define CALLBack_H

#include "jsaudio.h"

class JsPaStreamCallbackBridge : public AsyncWorker {
public:
  explicit JsPaStreamCallbackBridge(Callback *callback_,
                                    size_t bytesPerFrameIn,
                                    size_t bytesPerFrameOut,
                                    const LocalValue &userData);
  explicit JsPaStreamCallbackBridge(Callback *callback_, size_t bytesPerFrame,
                                    const LocalValue &userData)
    : JsPaStreamCallbackBridge(callback_, bytesPerFrame, bytesPerFrame, userData) {}
  
  ~JsPaStreamCallbackBridge();
  
  int sendToCallback(const void* input, unsigned long frameCount);
  void dispatchJSCallback(); 
  void consumeAudioData(void* output, unsigned long frameCount);
  int getCallbackResult();

  
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
  size_t m_bytesPerFrameIn;
  size_t m_bytesPerFrameOut;
  void* m_inputBuffer;
  void* m_outputBuffer;
  int m_callbackResult;
  
};

#endif