#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _personal_automine_download_zocl
{
  struct __unaligned __declspec(align(1)) __list
  {
    unsigned __int8 byClientSlotIndex;
    unsigned __int8 byTblCode;
    unsigned __int16 wItemTblIndex;
    unsigned __int8 byDur;
  };

  unsigned __int8 byHaveBag;
  unsigned __int8 byCnt;
  __list list[40];

  unsigned __int64 size();
};
#pragma pack(pop)
