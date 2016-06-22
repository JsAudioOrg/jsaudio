#include "callback.h"

JsPaStreamCallback::JsPaStreamCallback(Callback *callback_)
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

JsPaStreamCallback::~JsPaStreamCallback() {
  uv_mutex_destroy(&async_lock);
}

int JsPaStreamCallback::sendToCallback(unsigned long frameCount) {
  uv_async_send(async);
  return 0;
}

void JsPaStreamCallback::dispatchJSCallback() {
  HandleScope scope;
  callback->Call(0, NULL);
}
  
void* JsPaStreamCallback::consumeAudioData(unsigned long frameCount) {
  //stub
}