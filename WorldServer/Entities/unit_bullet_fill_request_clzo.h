#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _unit_bullet_fill_request_clzo
{
  unsigned __int8 bySlotIndex;
  int bUseNPCLinkIntem;
  unsigned __int16 wBulletIndex[2];
};
#pragma pack(pop)
