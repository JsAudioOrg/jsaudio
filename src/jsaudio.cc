/*
  http://portaudio.com/docs/v19-doxydocs/api_overview.html
*/

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
using String = v8::String;
using Number = v8::Number;
using Object = v8::Object;
using LocalString = v8::Local<String>;
using LocalNumber = v8::Local<Number>;
using LocalObject = v8::Local<Object>;

/* BEGIN Initialization, termination, and utility */
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

/* BEGIN Host APIs */
NAN_METHOD(getHostApiCount) {
  info.GetReturnValue().Set(Pa_GetHostApiCount());
}

NAN_METHOD(getDefaultHostApi) {
  info.GetReturnValue().Set(Pa_GetDefaultHostApi());
}

NAN_METHOD(getHostApiInfo) {
  HandleScope scope;
  int api = info[0].IsEmpty() ? Pa_GetDefaultHostApi() : info[0]->Uint32Value();
  const PaHostApiInfo* hai = Pa_GetHostApiInfo(api);
  LocalObject obj = New<Object>();
  LocalString haiIndex = New("apiIndex").ToLocalChecked();
  LocalString haiType = New("type").ToLocalChecked();
  LocalString haiName = New("name").ToLocalChecked();
  LocalString haiDefC = New("deviceCount").ToLocalChecked();
  LocalString haiDefIn = New("defaultInputDevice").ToLocalChecked();
  LocalString haiDefOut = New("defaultOutputDevice").ToLocalChecked();
  obj->Set(haiIndex, New<Number>(api));
  obj->Set(haiType, New<Number>(hai->type));
  obj->Set(haiName, New<String>(hai->name).ToLocalChecked());
  obj->Set(haiDefC, New<Number>(hai->deviceCount));
  obj->Set(haiDefIn, New<Number>(hai->defaultInputDevice));
  obj->Set(haiDefOut, New<Number>(hai->defaultOutputDevice));
  info.GetReturnValue().Set(obj);
}

/* BEGIN Host APIs */

NAN_MODULE_INIT(Init) {
  NAN_EXPORT(target, initialize);
  NAN_EXPORT(target, terminate);
  NAN_EXPORT(target, getVersion);
  NAN_EXPORT(target, getHostApiCount);
  NAN_EXPORT(target, getDefaultHostApi);
  NAN_EXPORT(target, getHostApiInfo);
}

NODE_MODULE(jsaudio, Init)
