#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __cppobj __unaligned __declspec(align(1)) _alter_item_slot_request_clzo
{
  struct __unaligned __declspec(align(2)) __list
  {
    unsigned __int8 byStorageIndex;
    unsigned int dwItemSerial;
    unsigned __int8 byClientSlotIndex;
  };

  unsigned __int8 byNum;
  __list list[100];
};
#pragma pack(pop)
