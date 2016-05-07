#ifndef HELPERS_H
#define HELPERS_H

#include "jsaudio.h"

int LocalizeInt (MaybeLocalValue lvIn);
double LocalizeDouble (MaybeLocalValue lvIn);
unsigned long LocalizeULong (MaybeLocalValue lvIn); 
LocalString ToLocString (std::string str);
LocalObject ToLocObject (MaybeLocalValue lvIn);
void HostApiInfoToLocalObject (LocalObject obj, const PaHostApiInfo* hai); 
void DeviceInfoToLocalObject (LocalObject obj, const PaDeviceInfo* di);
PaStreamParameters LocObjToPaStreamParameters (LocalObject obj);

#endif