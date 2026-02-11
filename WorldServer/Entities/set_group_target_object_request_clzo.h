#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _set_group_target_object_request_clzo
{
  unsigned __int8 byGroupType;
  unsigned __int8 byID;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
};
#pragma pack(pop)
