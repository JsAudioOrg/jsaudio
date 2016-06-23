#include "callback.h"

JsPaStreamCallbackBridge::JsPaStreamCallbackBridge(Callback *callback_)
  : AsyncWorker(callback_) {
  async = new uv_async_t;
  uv_async_init(
    uv_default_loop()
    , async
    , UVCallback
  );
  async->data = this;

  uv_mutex_init(&async_lock);  
}

JsPaStreamCallbackBridge::~JsPaStreamCallbackBridge() {
  uv_mutex_destroy(&async_lock);
}

int JsPaStreamCallbackBridge::sendToCallback(unsigned long frameCount) {
  uv_async_send(async);
  return 0;
}

void JsPaStreamCallbackBridge::dispatchJSCallback() {
  HandleScope scope;
  callback->Call(0, NULL);
}
  
void* JsPaStreamCallbackBridge::consumeAudioData(unsigned long frameCount) {
  //stub
}