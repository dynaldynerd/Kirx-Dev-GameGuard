#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _animus_target_request_clzo
{
  unsigned __int8 byObjectID;
  unsigned __int16 wObjectIndex;
  unsigned int dwObjectSerial;
};
#pragma pack(pop)
