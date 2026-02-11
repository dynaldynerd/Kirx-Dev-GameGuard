#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __cppobj _alter_link_slot_request_clzo
{
  struct __list
  {
    unsigned __int8 bySlotIndex;
    unsigned __int8 byLinkCode;
    unsigned __int16 wIndex;
  };

  unsigned __int8 byLinkLock;
  unsigned __int8 byNum;
  __list list[50];
};
#pragma pack(pop)
