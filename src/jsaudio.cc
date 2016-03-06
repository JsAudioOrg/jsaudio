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

double LocalizeDouble (MaybeLocalValue lvIn) {
  return static_cast<double>(lvIn.ToLocalChecked()->IntegerValue());
}

unsigned long LocalizeULong (MaybeLocalValue lvIn) {
  return static_cast<unsigned long>(lvIn.ToLocalChecked()->IntegerValue());
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
// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#abed859482d156622d9332dff9b2d89da
NAN_METHOD(initialize) {
  PaError err = Pa_Initialize();
  if (err != paNoError) {
    ThrowError(Pa_GetErrorText(err));
  }
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a0db317604e916e8bd6098e60e6237221
NAN_METHOD(terminate) {
  PaError err = Pa_Terminate();
  if (err != paNoError) {
    ThrowError(Pa_GetErrorText(err));
  }
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a66da08bcf908e0849c62a6b47f50d7b4
NAN_METHOD(getVersion) {
  info.GetReturnValue().Set(Pa_GetVersion());
}

/* BEGIN Host APIs */
// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a19dbdb7c8702e3f4bfc0cdb99dac3dd9
NAN_METHOD(getHostApiCount) {
  info.GetReturnValue().Set(Pa_GetHostApiCount());
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#ae55c77f9b7e3f8eb301a6f1c0e2347ac
NAN_METHOD(getDefaultHostApi) {
  info.GetReturnValue().Set(Pa_GetDefaultHostApi());
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a7c650aede88ea553066bab9bbe97ea90
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
// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#acfe4d3c5ec1a343f459981bfa2057f8d
NAN_METHOD(getDeviceCount) {
  info.GetReturnValue().Set(Pa_GetDeviceCount());
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#abf9f2f82da95553d5adb929af670f74b
NAN_METHOD(getDefaultInputDevice) {
  info.GetReturnValue().Set(Pa_GetDefaultInputDevice());
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#adc955dfab007624000695c48d4f876dc
NAN_METHOD(getDefaultOutputDevice) {
  info.GetReturnValue().Set(Pa_GetDefaultOutputDevice());
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#ac7d8e091ffc1d1d4a035704660e117eb
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
// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a443ad16338191af364e3be988014cbbe
NAN_METHOD(openStream) {
  HandleScope scope;
  // Get params objects
  LocalObject obj = info[0]->ToObject();
  LocalObject objInput = ToLocObject(Get(obj, ToLocString("input")));
  LocalObject objOutput = ToLocObject(Get(obj, ToLocString("output")));
  PaStreamParameters paramsIn = LocObjToPaStreamParameters(objInput);
  PaStreamParameters paramsOut = LocObjToPaStreamParameters(objOutput);
  // Get stream options
  double sampleRate = LocalizeDouble(Get(obj, ToLocString("sampleRate")));
	unsigned long framesPerBuffer = LocalizeULong(
    Get(obj, ToLocString("framesPerBuffer")));
  PaStreamFlags streamFlags = static_cast<PaStreamFlags>(
    Get(obj, ToLocString("streamFlags")).ToLocalChecked()->IntegerValue());
  // Callback
  // http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a8a60fb2a5ec9cbade3f54a9c978e2710
  // Start stream
  PaStream *stream;
  PaError err = Pa_OpenStream(
    &stream,
    &paramsIn,
    &paramsOut,
    sampleRate,
    framesPerBuffer,
    streamFlags,
    NULL,
    NULL
  );
  // Testing that params are set right
  info.GetReturnValue().Set(New<Number>(streamFlags));
}

/*
// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a0a12735ac191200f696a43b87667b714
NAN_METHOD(openDefaultStream) {
  // ToDo: implement this
}
*/

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
  // NAN_EXPORT(target, openDefaultStream);
}

NODE_MODULE(jsaudio, Init)
