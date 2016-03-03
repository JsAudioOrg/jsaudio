/*
  http://portaudio.com/docs/v19-doxydocs/api_overview.html
*/


/* BEGIN Setup */
#include "portaudio.h"
#include "pa_asio.h"
#include <nan.h>

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

/* BEGIN Device APIs */
NAN_METHOD(getDeviceCount) {
  info.GetReturnValue().Set(Pa_GetDeviceCount());
}

NAN_METHOD(getDefaultInputDevice) {
  info.GetReturnValue().Set(Pa_GetDefaultInputDevice());
}

NAN_METHOD(getDefaultOutputDevice) {
  info.GetReturnValue().Set(Pa_GetDefaultOutputDevice());
}

NAN_METHOD(getDeviceInfo) {
  HandleScope scope;
  int dvc = info[0].IsEmpty()
    ? Pa_GetDefaultInputDevice()
    : info[0]->Uint32Value();
  const PaDeviceInfo* di = Pa_GetDeviceInfo(dvc);
  LocalObject obj = New<Object>();
  LocalString diIndex = New("deviceIndex").ToLocalChecked();
  LocalString diHost = New("hostApi").ToLocalChecked();
  LocalString diName = New("name").ToLocalChecked();
  LocalString diMaxI = New("maxInputChannels").ToLocalChecked();
  LocalString diMaxO = New("maxOutputChannels").ToLocalChecked();
  LocalString diDefLIL = New("defaultLowInputLatency").ToLocalChecked();
  LocalString diDefLOL = New("defaultLowOutputLatency").ToLocalChecked();
  LocalString diDefHIL = New("defaultHighInputLatency").ToLocalChecked();
  LocalString diDefHOL = New("defaultHighOutputLatency").ToLocalChecked();
  LocalString diDefSR = New("defaultSampleRate").ToLocalChecked();
  obj->Set(diIndex, New<Number>(dvc));
  obj->Set(diHost, New<Number>(di->hostApi));
  obj->Set(diName, New<String>(di->name).ToLocalChecked());
  obj->Set(diMaxI, New<Number>(di->maxInputChannels));
  obj->Set(diMaxO, New<Number>(di->maxOutputChannels));
  obj->Set(diDefLIL, New<Number>(di->defaultLowInputLatency));
  obj->Set(diDefLOL, New<Number>(di->defaultLowOutputLatency));
  obj->Set(diDefHIL, New<Number>(di->defaultHighInputLatency));
  obj->Set(diDefHOL, New<Number>(di->defaultHighOutputLatency));
  obj->Set(diDefSR, New<Number>(di->defaultSampleRate));
  info.GetReturnValue().Set(obj);
}

/* BEGIN Init & Exports */
NAN_MODULE_INIT(Init) {
  NAN_EXPORT(target, initialize);
  NAN_EXPORT(target, terminate);
  NAN_EXPORT(target, getVersion);
  NAN_EXPORT(target, getHostApiCount);
  NAN_EXPORT(target, getDefaultHostApi);
  NAN_EXPORT(target, getHostApiInfo);
  NAN_EXPORT(target, getDeviceCount);
  NAN_EXPORT(target, getDefaultInputDevice);
  NAN_EXPORT(target, getDefaultOutputDevice);
  NAN_EXPORT(target, getDeviceInfo);
}

NODE_MODULE(jsaudio, Init)
