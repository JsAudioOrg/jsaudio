#include "callback.h"
#include "helpers.h"

JsPaStreamCallbackBridge::JsPaStreamCallbackBridge(Callback *callback_,
                                                   size_t bytesPerFrameIn,
                                                   size_t bytesPerFrameOut,
                                                   const LocalValue &userData)
  : AsyncWorker(callback_), m_frameCount(0), m_callbackResult(0),
    m_bytesPerFrameIn(bytesPerFrameIn), m_bytesPerFrameOut(bytesPerFrameOut),
    m_inputBuffer(nullptr), m_outputBuffer(nullptr) {
  async = new uv_async_t;
  uv_async_init(
    uv_default_loop()
    , async
    , UVCallback
  );
  async->data = this;
  uv_barrier_init(&async_barrier, 2);
      
  // Save userData to persistent object
  SaveToPersistent(ToLocString("userData"), userData);
}

JsPaStreamCallbackBridge::~JsPaStreamCallbackBridge() {
  uv_barrier_destroy(&async_barrier);
  uv_close((uv_handle_t*)async, NULL);
  
}

void JsPaStreamCallbackBridge::dispatchJSCallback() {
  HandleScope scope;
  unsigned long frameCount;
  v8::Local<v8::ArrayBuffer> input;
  v8::Local<v8::ArrayBuffer> output;
  v8::Local<v8::Value> callbackReturn;
  
  
  frameCount = m_frameCount;

  // Setup ArrayBuffer for input audio data
  input = v8::ArrayBuffer::New(
    v8::Isolate::GetCurrent(),
    const_cast<void*>(m_inputBuffer),
    m_bytesPerFrameIn * frameCount
  );

  // Setup ArrayBuffer for output audio data
  output = v8::ArrayBuffer::New(
    v8::Isolate::GetCurrent(),
    m_outputBuffer,
    m_bytesPerFrameOut * frameCount
  );

  // Create array of arguments and call the javascript callback
  LocalValue argv[] = {
    input,
    output,
    New<Number>(frameCount),
    GetFromPersistent(ToLocString("userData"))
  };
  m_callbackResult = LocalizeInt(callback->Call(4, argv));
  
  uv_barrier_wait(&async_barrier);
}

int JsPaStreamCallbackBridge::Execute(const void* input, void* output, unsigned long frameCount) {
  m_frameCount = frameCount;

  m_inputBuffer = input;
  m_outputBuffer = output;

  // Dispatch the asyncronous callback
  uv_async_send(async);
  
  // Wait for the asyncronous callback
  uv_barrier_wait(&async_barrier);
  
  return m_callbackResult;
}

