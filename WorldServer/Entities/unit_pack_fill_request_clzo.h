#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _unit_pack_fill_request_clzo
{
  struct __unaligned __declspec(align(1)) __list
  {
    unsigned __int8 bySpareIndex;
    unsigned __int16 wBulletIndex;
  };

  unsigned __int8 bySlotIndex;
  unsigned __int8 byFillNum;
  int bUseNPCLinkIntem;
  __list List[8];
};
#pragma pack(pop)
