#include "callback.h"

JsPaStreamCallbackBridge::JsPaStreamCallbackBridge(Callback *callback_,
                                                   size_t bytesPerFrame)
  : AsyncWorker(callback_), m_frameCount(0), 
    m_bytesPerFrameIn(bytesPerFrame), m_bytesPerFrameOut(bytesPerFrame),
    m_inputBuffer(nullptr), m_outputBuffer(nullptr) {
  async = new uv_async_t;
  uv_async_init(
    uv_default_loop()
    , async
    , UVCallback
  );
  async->data = this;

  uv_mutex_init(&async_lock);  
}

JsPaStreamCallbackBridge::JsPaStreamCallbackBridge(Callback *callback_,
                                                   size_t bytesPerFrameIn,
                                                   size_t bytesPerFrameOut)
  : AsyncWorker(callback_), m_frameCount(0),
    m_bytesPerFrameIn(bytesPerFrameIn), m_bytesPerFrameOut(bytesPerFrameOut),
    m_inputBuffer(nullptr), m_outputBuffer(nullptr) {
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
  
  //free buffer memory
  if(m_inputBuffer != nullptr)
      free(m_inputBuffer);
  if(m_outputBuffer != nullptr)
      free(m_outputBuffer);
}

int JsPaStreamCallbackBridge::sendToCallback(const void* input, unsigned long frameCount) {
  uv_mutex_lock(&async_lock);
    m_frameCount = frameCount;
  
    if(m_inputBuffer != nullptr)
      free(m_inputBuffer);
    m_inputBuffer = malloc(sizeof(float) * frameCount * 2);
  
    memmove(
      m_inputBuffer,
      input,
      m_bytesPerFrameIn * frameCount
    );
  uv_mutex_unlock(&async_lock);
  
  uv_async_send(async);
  return 0;
}

void JsPaStreamCallbackBridge::dispatchJSCallback() {
  HandleScope scope;
  unsigned long frameCount;
  v8::Local<v8::ArrayBuffer> input;
  v8::Local<v8::ArrayBuffer> output;
  v8::Local<v8::Value> callbackReturn;
  
  uv_mutex_lock(&async_lock);
  
    frameCount = m_frameCount;
    
    // Setup ArrayBuffer for input audio data
    input = v8::ArrayBuffer::New(
      v8::Isolate::GetCurrent(),
      m_bytesPerFrameIn * frameCount
    );
    // Copy input audio data from bridge buffer to ArrayBuffer
    memmove(
      input->GetContents().Data(),
      m_inputBuffer,
      input->ByteLength()
    );
  
    // Setup ArrayBuffer for output audio data
    output = v8::ArrayBuffer::New(
      v8::Isolate::GetCurrent(),
      m_bytesPerFrameOut * frameCount
    );
  
    LocalValue argv[] = {
      input,
      output,
      New<Number>(frameCount)
    };
    callbackReturn = callback->Call(3, argv);
    
    if(m_outputBuffer != nullptr)
      free(m_outputBuffer);
    m_outputBuffer = malloc(output->ByteLength());
    // Copy output audio data from bridge buffer to ArrayBuffer
    memmove(
      m_outputBuffer,
      output->GetContents().Data(),
      output->ByteLength()
    );
    
  
  uv_mutex_unlock(&async_lock); 
  
}
  
void JsPaStreamCallbackBridge::consumeAudioData(void* output, unsigned long frameCount) {
  
  if(m_outputBuffer != nullptr) {
    memmove(
      output,
      m_outputBuffer,
      m_bytesPerFrameOut * frameCount
    );
    
    // Free the output buffer and set it to nullptr to prevent it from sending the same output data twice
    free(m_outputBuffer);
    m_outputBuffer = nullptr;
  }
}