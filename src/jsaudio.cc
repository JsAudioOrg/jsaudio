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

/* BEGIN Stream APIs */
NAN_METHOD(openStream) {
  HandleScope scope;
  LocalObject obj = info[0]->ToObject();
  LocalString input = New("input").ToLocalChecked();
  LocalString output = New("output").ToLocalChecked();
  LocalObject objInput = Get(obj, input).ToLocalChecked()->ToObject();
  LocalObject objOutput = Get(obj, output).ToLocalChecked()->ToObject();
  LocalString device = New("device").ToLocalChecked();
  LocalString channelCount = New("channelCount").ToLocalChecked();
  LocalString sampleFormat = New("sampleFormat").ToLocalChecked();
  LocalString suggestedLatency = New("suggestedLatency").ToLocalChecked();

  PaStreamParameters paramsIn = {
    static_cast<PaDeviceIndex>(LocalizeInt(Get(objInput, device))),
    static_cast<int>(LocalizeInt(Get(objInput, channelCount))),
    static_cast<PaSampleFormat>(LocalizeInt(Get(objInput, sampleFormat))),
    static_cast<PaTime>(LocalizeInt(Get(objInput, suggestedLatency))),
    NULL
  };

  PaStreamParameters paramsOut = {
    static_cast<PaDeviceIndex>(LocalizeInt(Get(objOutput, device))),
    static_cast<int>(LocalizeInt(Get(objOutput, channelCount))),
    static_cast<PaSampleFormat>(LocalizeInt(Get(objOutput, sampleFormat))),
    static_cast<PaTime>(LocalizeInt(Get(objOutput, suggestedLatency))),
    NULL
  };

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
