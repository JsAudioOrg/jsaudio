/*
  http://portaudio.com/docs/v19-doxydocs/api_overview.html
*/
#ifndef JSAUDIO_H
#define JSAUDIO_H


/* BEGIN Setup */
#include "main.h"

NAN_METHOD(initialize);
NAN_METHOD(terminate);
NAN_METHOD(getVersion);
NAN_METHOD(getHostApiCount);
NAN_METHOD(getDefaultHostApi);
NAN_METHOD(getHostApiInfo);
NAN_METHOD(getLastHostErrorInfo);
NAN_METHOD(getDeviceCount);
NAN_METHOD(getDefaultInputDevice);
NAN_METHOD(getDefaultOutputDevice);
NAN_METHOD(getDeviceInfo);
NAN_METHOD(openStream);
NAN_METHOD(openDefaultStream);
NAN_METHOD(closeStream);
NAN_METHOD(startStream);
NAN_METHOD(stopStream);
NAN_METHOD(getStreamWriteAvailable);
NAN_METHOD(getStreamReadAvailable);
NAN_METHOD(writeStream);
NAN_METHOD(readStream);

#endif
