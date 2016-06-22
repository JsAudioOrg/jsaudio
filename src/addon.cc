#include "jsaudio.h"
#include "stream.h"

/* BEGIN Init & Exports */
NAN_MODULE_INIT(InitAll) {
  /* BEGIN Initialization, termination, and utility */
  NAN_EXPORT(target, initialize);
  NAN_EXPORT(target, terminate);
  NAN_EXPORT(target, getVersion);
  NAN_EXPORT(target, getErrorText);
  /* BEGIN Host APIs */
  NAN_EXPORT(target, getHostApiCount);
  NAN_EXPORT(target, getDefaultHostApi);
  NAN_EXPORT(target, getHostApiInfo);
  NAN_EXPORT(target, getLastHostErrorInfo);
  NAN_EXPORT(target, hostApiTypeIdToHostApiIndex);
  NAN_EXPORT(target, hostApiDeviceIndexToDeviceIndex);
  /* BEGIN Device APIs */
  NAN_EXPORT(target, getDeviceCount);
  NAN_EXPORT(target, getDefaultInputDevice);
  NAN_EXPORT(target, getDefaultOutputDevice);
  NAN_EXPORT(target, getDeviceInfo);
  /* BEGIN Stream APIs */
  NAN_EXPORT(target, isFormatSupported);
  NAN_EXPORT(target, whyIsFormatUnsupported);
  NAN_EXPORT(target, openStream);
  NAN_EXPORT(target, openDefaultStream);
  NAN_EXPORT(target, closeStream);
  // NAN_EXPORT(target, setStreamFinishedCallback);
  NAN_EXPORT(target, startStream);
  NAN_EXPORT(target, stopStream);
  NAN_EXPORT(target, abortStream);
  NAN_EXPORT(target, isStreamStopped);
  NAN_EXPORT(target, isStreamActive);
  NAN_EXPORT(target, getStreamInfo);
  NAN_EXPORT(target, getStreamTime);
  NAN_EXPORT(target, getStreamCpuLoad);
  NAN_EXPORT(target, readStream);
  NAN_EXPORT(target, writeStream);
  NAN_EXPORT(target, getStreamReadAvailable);
  NAN_EXPORT(target, getStreamWriteAvailable);
  NAN_EXPORT(target, getSampleSize);

  JsPaStream::Init(target);
}

NODE_MODULE(jsaudio, InitAll)
