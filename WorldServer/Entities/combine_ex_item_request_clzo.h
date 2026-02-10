#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _combine_ex_item_request_clzo
{
  struct _list
  {
    unsigned __int8 byStorageCode;
    __unaligned __declspec(align(1)) unsigned __int16 wItemSerial;
    unsigned __int8 byAmount;
  };

  unsigned int wManualIndex;
  unsigned __int8 byCombineSlotNum;
  _list iCombineSlot[5];
  int bUseNpcLink;
};
#pragma pack(pop)
