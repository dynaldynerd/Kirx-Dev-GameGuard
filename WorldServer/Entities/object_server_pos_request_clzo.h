#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _object_server_pos_request_clzo
{
  unsigned __int8 byKind;
  unsigned __int8 byID;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
};
#pragma pack(pop)
