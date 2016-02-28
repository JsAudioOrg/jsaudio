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
  Pa_Initialize();
}

NAN_MODULE_INIT(Init) {
  NAN_EXPORT(target, initialize);
}

NODE_MODULE(jsaudio, Init)
