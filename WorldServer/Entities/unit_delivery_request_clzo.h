#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _unit_delivery_request_clzo
{
  unsigned __int8 bySlotIndex;
  unsigned int dwStoreIndex;
  __int16 zUnitNewPos[3];
  int bUseNPCLinkIntem;
};
#pragma pack(pop)
