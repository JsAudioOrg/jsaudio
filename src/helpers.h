#ifndef HELPERS_H
#define HELPERS_H

#include "jsaudio.h"

// PortAudio helpers
PaError ThrowIfPaError (PaError err);
int ThrowIfPaErrorInt (int err);
PaDeviceIndex ThrowIfPaNoDevice (PaDeviceIndex dvcIndex);
// Cast helpers
int LocalizeInt (MaybeLocalValue lvIn);
double LocalizeDouble (MaybeLocalValue lvIn);
unsigned long LocalizeULong (MaybeLocalValue lvIn);
LocalString ToLocString (std::string str);
LocalObject ToLocObject (MaybeLocalValue lvIn);
LocalFunction ToLocFunction (MaybeLocalValue lvIn);
LocalString ConstCharPointerToLocString (const char* constCharPointer);
void HostApiInfoToLocalObject (LocalObject obj, const PaHostApiInfo* hai);
void DeviceInfoToLocalObject (LocalObject obj, const PaDeviceInfo* di);
PaStreamParameters LocObjToPaStreamParameters (LocalObject obj);
size_t bytesPerFrame (PaSampleFormat sampleFormat);

#endif
