/*
  http://portaudio.com/docs/v19-doxydocs/api_overview.html
*/


/* BEGIN Setup */
// #include "helpers.cc"
#include "portaudio.h"
#include <nan.h>
#ifdef _WIN32
  #include "pa_asio.h"
#endif

using namespace Nan;
using String = v8::String;
using Number = v8::Number;
using Object = v8::Object;
using Value = v8::Value;
using Value = v8::Value;
using LocalString = v8::Local<String>;
using LocalNumber = v8::Local<Number>;
using LocalObject = v8::Local<Object>;
using LocalValue = v8::Local<Value>;
using MaybeLocalValue = v8::MaybeLocal<Value>;

/* BEGIN Helpers */
int LocalizeInt (MaybeLocalValue lvIn) {
  return lvIn.ToLocalChecked()->Uint32Value();
}

LocalString ToLocString (std::string str) {
  return New(str).ToLocalChecked();
}

LocalObject ToLocObject (MaybeLocalValue lvIn) {
  return lvIn.ToLocalChecked()->ToObject();
}

void HostApiInfoToLocalObject (LocalObject obj, const PaHostApiInfo* hai) {
  obj->Set(
    ToLocString("type"), New<Number>(hai->type));
  obj->Set(
    ToLocString("name"), New<String>(hai->name).ToLocalChecked());
  obj->Set(
    ToLocString("deviceCount"), New<Number>(hai->deviceCount));
  obj->Set(
    ToLocString("defaultInputDevice"), New<Number>(hai->defaultInputDevice));
  obj->Set(
    ToLocString("defaultOutputDevice"), New<Number>(hai->defaultOutputDevice));
  return;
}

void DeviceInfoToLocalObject (LocalObject obj, const PaDeviceInfo* di) {
  obj->Set(
    ToLocString("hostApi"),
    New<Number>(di->hostApi));
  obj->Set(
    ToLocString("name"),
    New<String>(di->name).ToLocalChecked());
  obj->Set(
    ToLocString("maxInputChannels"),
    New<Number>(di->maxInputChannels));
  obj->Set(
    ToLocString("maxOutputChannels"),
    New<Number>(di->maxOutputChannels));
  obj->Set(
    ToLocString("defaultLowInputLatency"),
    New<Number>(di->defaultLowInputLatency));
  obj->Set(
    ToLocString("defaultLowOutputLatency"),
    New<Number>(di->defaultLowOutputLatency));
  obj->Set(
    ToLocString("defaultHighInputLatency"),
    New<Number>(di->defaultHighInputLatency));
  obj->Set(
    ToLocString("defaultHighOutputLatency"),
    New<Number>(di->defaultHighOutputLatency));
  obj->Set(
    ToLocString("defaultSampleRate"),
    New<Number>(di->defaultSampleRate));
  return;
}

PaStreamParameters LocObjToPaStreamParameters (LocalObject obj) {
  PaStreamParameters params = {
    static_cast<PaDeviceIndex>(
      LocalizeInt(Get(obj, ToLocString("device")))),
    static_cast<int>(
      LocalizeInt(Get(obj, ToLocString("channelCount")))),
    static_cast<PaSampleFormat>(
      LocalizeInt(Get(obj, ToLocString("sampleFormat")))),
    static_cast<PaTime>(
      LocalizeInt(Get(obj, ToLocString("suggestedLatency")))),
    NULL
  };
  return params;
}

/* BEGIN Initialization, termination, and utility */
NAN_METHOD(initialize) {
  PaError err = Pa_Initialize();
  if (err != paNoError) {
    ThrowError(Pa_GetErrorText(err));
  }
}

NAN_METHOD(terminate) {
  PaError err = Pa_Terminate();
  if (err != paNoError) {
    ThrowError(Pa_GetErrorText(err));
  }
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
  obj->Set(ToLocString("apiIndex"), New<Number>(api));
  HostApiInfoToLocalObject(obj, hai);
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
  obj->Set(ToLocString("deviceIndex"), New<Number>(dvc));
  DeviceInfoToLocalObject(obj, di);
  info.GetReturnValue().Set(obj);
}

/* BEGIN Stream APIs */
NAN_METHOD(openStream) {
  HandleScope scope;
  LocalObject obj = info[0]->ToObject();
  LocalObject objInput = ToLocObject(Get(obj, ToLocString("input")));
  LocalObject objOutput = ToLocObject(Get(obj, ToLocString("output")));
  PaStreamParameters paramsIn = LocObjToPaStreamParameters(objInput);
  PaStreamParameters paramsOut = LocObjToPaStreamParameters(objOutput);
  // Testing that params are set right
  info.GetReturnValue().Set(New<Number>(paramsIn.device));
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
  NAN_EXPORT(target, openStream);
}

NODE_MODULE(jsaudio, Init)
