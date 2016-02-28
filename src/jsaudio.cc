#include "portaudio.h"
#include "pa_asio.h"
#include <nan.h>

// Platform specific includes
#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
#endif

using namespace Nan;

NAN_METHOD(initialize) {
  PaError err = Pa_Initialize();
  if (err != paNoError) {
    ThrowError(Pa_GetErrorText(err));
  }
}

NAN_METHOD(terminate) {
  Pa_Terminate();
}

NAN_METHOD(getVersion) {
  info.GetReturnValue().Set(Pa_GetVersion());
}

NAN_MODULE_INIT(Init) {
  NAN_EXPORT(target, initialize);
  NAN_EXPORT(target, terminate);
  NAN_EXPORT(target, getVersion);
}

NODE_MODULE(jsaudio, Init)
