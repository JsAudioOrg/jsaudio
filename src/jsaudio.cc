/*
  http://portaudio.com/docs/v19-doxydocs/api_overview.html
*/


/* BEGIN Setup */
#include "jsaudio.h"
#include "helpers.h"
#include "stream.h"

/* Initialize stream and jsStreamCb as global */
LocalFunction jsStreamCb;

/* BEGIN Initialization, termination, and utility */
// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#abed859482d156622d9332dff9b2d89da
NAN_METHOD(initialize) {
  ThrowIfPaError(Pa_Initialize());
  info.GetReturnValue().Set(true);
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a0db317604e916e8bd6098e60e6237221
NAN_METHOD(terminate) {
  ThrowIfPaError(Pa_Terminate());
  info.GetReturnValue().Set(true);
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a66da08bcf908e0849c62a6b47f50d7b4
NAN_METHOD(getVersion) {
  info.GetReturnValue().Set(Pa_GetVersion());
}

/* BEGIN Host APIs */
// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a19dbdb7c8702e3f4bfc0cdb99dac3dd9
NAN_METHOD(getHostApiCount) {
  int hostApiCount = Pa_GetHostApiCount();
  ThrowIfPaErrorInt(hostApiCount);
  info.GetReturnValue().Set(hostApiCount);
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#ae55c77f9b7e3f8eb301a6f1c0e2347ac
NAN_METHOD(getDefaultHostApi) {
  int api = Pa_GetDefaultHostApi();
  ThrowIfPaErrorInt(api);
  info.GetReturnValue().Set(api);
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a7c650aede88ea553066bab9bbe97ea90
NAN_METHOD(getHostApiInfo) {
  HandleScope scope;
  int api = info[0].IsEmpty() ? Pa_GetDefaultHostApi() : info[0]->Uint32Value();
  ThrowIfPaErrorInt(api);
  const PaHostApiInfo* hai = Pa_GetHostApiInfo(api);
  if (hai == NULL) {
    ThrowError(ToLocString("Unable to retrieve Host API info"));
  }
  LocalObject obj = New<Object>();
  obj->Set(ToLocString("apiIndex"), New<Number>(api));
  HostApiInfoToLocalObject(obj, hai);
  info.GetReturnValue().Set(obj);
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#aad573f208b60577f21d2777a7c5054e0
NAN_METHOD(getLastHostErrorInfo) {
  HandleScope scope;
  const PaHostErrorInfo* hei = Pa_GetLastHostErrorInfo();
  if (hei == NULL) return info.GetReturnValue().Set(NULL);
  LocalObject obj = New<Object>();
  obj->Set(ToLocString("hostApiType"), New<Number>(hei->hostApiType));
  obj->Set(ToLocString("errorCode"), New<Number>(hei->errorCode));
  obj->Set(ToLocString("errorText"), New<String>(hei->errorText).ToLocalChecked());
  info.GetReturnValue().Set(obj);
}

/* BEGIN Device APIs */
// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#acfe4d3c5ec1a343f459981bfa2057f8d
NAN_METHOD(getDeviceCount) {
  int dvcCount = Pa_GetDeviceCount();
  ThrowIfPaErrorInt(dvcCount);
  info.GetReturnValue().Set(dvcCount);
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#abf9f2f82da95553d5adb929af670f74b
NAN_METHOD(getDefaultInputDevice) {
  PaDeviceIndex dvcIndex = Pa_GetDefaultInputDevice();
  ThrowIfPaNoDevice(dvcIndex);
  info.GetReturnValue().Set(dvcIndex);
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#adc955dfab007624000695c48d4f876dc
NAN_METHOD(getDefaultOutputDevice) {
  PaDeviceIndex dvcIndex = Pa_GetDefaultOutputDevice();
  ThrowIfPaNoDevice(dvcIndex);
  info.GetReturnValue().Set(dvcIndex);
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#ac7d8e091ffc1d1d4a035704660e117eb
NAN_METHOD(getDeviceInfo) {
  HandleScope scope;
  PaDeviceIndex dvc = info[0].IsEmpty()
    ? Pa_GetDefaultInputDevice()
    : info[0]->Uint32Value();
  ThrowIfPaNoDevice(dvc);
  const PaDeviceInfo* di = Pa_GetDeviceInfo(dvc);
  if (di == NULL) {
    ThrowError(ToLocString("Unable to retrieve device info"));
  }
  LocalObject obj = New<Object>();
  obj->Set(ToLocString("deviceIndex"), New<Number>(dvc));
  DeviceInfoToLocalObject(obj, di);
  info.GetReturnValue().Set(obj);
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a443ad16338191af364e3be988014cbbe
NAN_METHOD(openStream) {
  HandleScope scope;
  // Get params objects
  LocalObject obj = info[0]->ToObject();
  JsPaStream* stream = ObjectWrap::Unwrap<JsPaStream>(
    ToLocObject(Get(obj, ToLocString("stream"))));
  // Prepare in / out params
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
  // Start stream
  PaError err = Pa_OpenStream(
    stream->streamPtrRef(),
    &paramsIn,
    &paramsOut,
    sampleRate,
    framesPerBuffer,
    streamFlags,
    NULL,
    NULL
  );
  ThrowIfPaError(err);
  info.GetReturnValue().Set(true);
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a0a12735ac191200f696a43b87667b714
NAN_METHOD(openDefaultStream) {
  HandleScope scope;
  // Get params objects
  LocalObject obj = info[0]->ToObject();
  JsPaStream* stream = ObjectWrap::Unwrap<JsPaStream>(
    ToLocObject(Get(obj, ToLocString("stream"))));
  // Get stream options
  int inputChannels = LocalizeInt(Get(obj, ToLocString("numInputChannels")));
  int outputChannels = LocalizeInt(Get(obj, ToLocString("numOutputChannels")));
  double sampleRate = LocalizeDouble(Get(obj, ToLocString("sampleRate")));
  PaSampleFormat sampleFormat = static_cast<PaSampleFormat>(LocalizeULong(
    Get(obj, ToLocString("sampleFormat"))));
  unsigned long framesPerBuffer = LocalizeULong(
    Get(obj, ToLocString("framesPerBuffer")));
  // Start stream
  PaError err = Pa_OpenDefaultStream(
    stream->streamPtrRef(),
    inputChannels,
    outputChannels,
    sampleFormat,
    sampleRate,
    framesPerBuffer,
    NULL,
    NULL
  );
  ThrowIfPaError(err);
  info.GetReturnValue().Set(true);
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a92f56f88cbd14da0e8e03077e835d104
NAN_METHOD(closeStream) {
  HandleScope scope;
  // Get stream object
  JsPaStream* stream = ObjectWrap::Unwrap<JsPaStream>(info[0]->ToObject());
  // Close stream
  ThrowIfPaError(Pa_CloseStream(stream->streamPtr()));
  info.GetReturnValue().Set(true);
}


// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a7432aadd26c40452da12fa99fc1a047b
NAN_METHOD(startStream) {
  HandleScope scope;
  // Get stream object
  JsPaStream* stream = ObjectWrap::Unwrap<JsPaStream>(info[0]->ToObject());
  // Start stream
  ThrowIfPaError(Pa_StartStream(stream->streamPtr()));
  info.GetReturnValue().Set(true);
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#af18dd60220251286c337631a855e38a0
NAN_METHOD(stopStream) {
  HandleScope scope;
  // Get stream object
  JsPaStream* stream = ObjectWrap::Unwrap<JsPaStream>(info[0]->ToObject());
  // Stop stream
  ThrowIfPaError(Pa_StopStream(stream->streamPtr()));
  info.GetReturnValue().Set(true);
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a25595acf48733ec32045aa189c3caa61
NAN_METHOD(getStreamWriteAvailable) {
  HandleScope scope;
  // Get stream object
  JsPaStream* stream = ObjectWrap::Unwrap<JsPaStream>(info[0]->ToObject());
  // Check that write stream is ready
  ThrowIfPaErrorInt(Pa_GetStreamWriteAvailable(stream->streamPtr()));
  info.GetReturnValue().Set(true);
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#ad04c33f045fa58d7b705b56b1fd3e816
NAN_METHOD(getStreamReadAvailable) {
  HandleScope scope;
  // Get stream object
  JsPaStream* stream = ObjectWrap::Unwrap<JsPaStream>(info[0]->ToObject());
  // Check that read stream is ready
  ThrowIfPaErrorInt(Pa_GetStreamReadAvailable(stream->streamPtr()));
  info.GetReturnValue().Set(true);
}

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a075a6efb503a728213bdae24347ed27d
NAN_METHOD(writeStream) {
  HandleScope scope;
  // Get stream object
  JsPaStream* stream = ObjectWrap::Unwrap<JsPaStream>(info[0]->ToObject());
  // Get the buffer data
  TypedArrayContents<float> buf(info[1]);
  unsigned long bufFrames = static_cast<unsigned long>(buf.length()) / 2;
  // Start stream
  ThrowIfPaError(Pa_WriteStream(stream->streamPtr(), *buf, bufFrames));
  info.GetReturnValue().Set(true);
}

//http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a0b62d4b74b5d3d88368e9e4c0b8b2dc7
NAN_METHOD(readStream) {
  HandleScope scope;
  // Get stream object
  JsPaStream* stream = ObjectWrap::Unwrap<JsPaStream>(info[0]->ToObject());
  // Get the buffer data
  TypedArrayContents<float> buf(info[1]);
  unsigned long bufFrames = static_cast<unsigned long>(buf.length()) / 2;
  // Start stream
  ThrowIfPaError(Pa_ReadStream(stream->streamPtr(), *buf, bufFrames));
  info.GetReturnValue().Set(true);
}
