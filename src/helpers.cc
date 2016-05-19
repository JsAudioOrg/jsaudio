#include "helpers.h"

// PortAudio helpers
void ThrowIfPaError (PaError err) {
  if (err != paNoError) {
    ThrowError(Pa_GetErrorText(err));
  }
  return err;
}

void ThrowIfPaErrorInt (int err) {
  if (err < 0) {
    ThrowError(Pa_GetErrorText(err));
  }
  return err;
}

void ThrowIfPaNoDevice (PaDeviceIndex dvcIndex) {
  if (dvcIndex == paNoDevice) {
    ThrowError(ToLocString("No available devices"));
  }
  return dvcIndex;
}

// Cast helpers
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
