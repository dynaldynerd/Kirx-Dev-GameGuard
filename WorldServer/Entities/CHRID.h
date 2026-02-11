#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _CHRID
{
  unsigned __int8 byID;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
};
#pragma pack(pop)
