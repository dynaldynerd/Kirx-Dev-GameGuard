#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _STORAGE_POS
{
  unsigned __int8 byStorageCode;
};

struct  _STORAGE_POS_INDIV : _STORAGE_POS
{
   unsigned __int16 wItemSerial;
  unsigned __int8 byNum;
};
#pragma pack(pop)
