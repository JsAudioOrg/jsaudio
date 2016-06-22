/*
  http://portaudio.com/docs/v19-doxydocs/api_overview.html
*/
#ifndef JSAUDIO_H
#define JSAUDIO_H


/* BEGIN Setup */
#include "main.h"

/* BEGIN Initialization, termination, and utility */
NAN_METHOD(initialize);
NAN_METHOD(terminate);
NAN_METHOD(getVersion);
NAN_METHOD(getErrorText);
/* BEGIN Host APIs */
NAN_METHOD(getHostApiCount);
NAN_METHOD(getDefaultHostApi);
NAN_METHOD(getHostApiInfo);
NAN_METHOD(getLastHostErrorInfo);
NAN_METHOD(hostApiTypeIdToHostApiIndex);
NAN_METHOD(hostApiDeviceIndexToDeviceIndex);
/* BEGIN Device APIs */
NAN_METHOD(getDeviceCount);
NAN_METHOD(getDefaultInputDevice);
NAN_METHOD(getDefaultOutputDevice);
NAN_METHOD(getDeviceInfo);
/* BEGIN Stream APIs */
NAN_METHOD(isFormatSupported);
NAN_METHOD(whyIsFormatUnsupported);
NAN_METHOD(openStream);
NAN_METHOD(openDefaultStream);
NAN_METHOD(closeStream);
// NAN_METHOD(setStreamFinishedCallback);
NAN_METHOD(startStream);
NAN_METHOD(stopStream);
NAN_METHOD(abortStream);
NAN_METHOD(isStreamStopped);
NAN_METHOD(isStreamActive);
NAN_METHOD(getStreamInfo);
NAN_METHOD(getStreamTime);
NAN_METHOD(getStreamCpuLoad);
NAN_METHOD(readStream);
NAN_METHOD(writeStream);
NAN_METHOD(getStreamReadAvailable);
NAN_METHOD(getStreamWriteAvailable);
NAN_METHOD(getSampleSize);

#endif
