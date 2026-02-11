#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __cppobj _pt_request_punishment_clzo
{
  unsigned __int8 byType;
  __unaligned __declspec(align(1)) unsigned __int16 wContentSize;
  char wszCharName[17];
  char wszContent[1280];
};
#pragma pack(pop)
