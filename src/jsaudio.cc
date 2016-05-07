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

// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a443ad16338191af364e3be988014cbbe
NAN_METHOD(openStream) {
  HandleScope scope;
  PaError err;
    
  // Get params objects
  LocalObject obj = info[0]->ToObject();
  JsPaStream* stream = ObjectWrap::Unwrap<JsPaStream>(ToLocObject(Get(obj, ToLocString("stream"))));
    
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
  err = Pa_OpenStream(
    stream->streamPtrRef(),
    &paramsIn,
    &paramsOut,
    sampleRate,
    framesPerBuffer,
    streamFlags,
    NULL,
    NULL
  );
  if (err != paNoError) {
    printf("%s\n", "OpenStream: ");
    printf("%s\n", Pa_GetErrorText(err));
    // ThrowError(Pa_GetErrorText(err));
  }
  // Testing that params are set right
  info.GetReturnValue().Set(New<Number>(err));
}

/*
// http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a0a12735ac191200f696a43b87667b714
NAN_METHOD(openDefaultStream) {
  // ToDo: implement this
}
*/

//http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a7432aadd26c40452da12fa99fc1a047b
NAN_METHOD(startStream) {
  HandleScope scope;
  PaError err;
    
  // Get stream object
  LocalObject obj = info[0]->ToObject();
  JsPaStream* stream = ObjectWrap::Unwrap<JsPaStream>(info[0]->ToObject());
  
  // Start stream
  err = Pa_StartStream(stream->streamPtr());
  if (err != paNoError) {
    printf("%s\n", "StartStream: ");
    printf("%s\n", Pa_GetErrorText(err));
    // ThrowError(Pa_GetErrorText(err));
  }
  // Testing that params are set right
  info.GetReturnValue().Set(New<Number>(err));  
}

NAN_METHOD(getStreamWriteAvailable) {
  HandleScope scope;
  long retVal;
    
  // Get stream object
  LocalObject obj = info[0]->ToObject();
  JsPaStream* stream = ObjectWrap::Unwrap<JsPaStream>(info[0]->ToObject());
  
  // Start stream
  retVal = Pa_GetStreamWriteAvailable(stream->streamPtr());
  
  // Testing that params are set right
  info.GetReturnValue().Set(New<Number>(retVal));  
}

NAN_METHOD(writeStream) {
  HandleScope scope;
  long retVal;
    
  // Get stream object
  LocalObject obj = info[0]->ToObject();
  JsPaStream* stream = ObjectWrap::Unwrap<JsPaStream>(info[0]->ToObject());
    
  // Get the buffer data
  TypedArrayContents<float> buf(info[1]);
  unsigned long bufFrames = static_cast<unsigned long>(buf.length()) / 2;
  
  // Start stream
  retVal = Pa_WriteStream(stream->streamPtr(), *buf, bufFrames);
  
  // Testing that params are set right
  info.GetReturnValue().Set(New<Number>(retVal));  
}
