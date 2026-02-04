#pragma once

#include "IdaCompat.h"

#pragma pack(push, 2)
struct __unaligned __declspec(align(2)) _notify_race_boss_winrate_zocl
{
  unsigned __int8 byTotalCnt;
  unsigned __int8 byCurWinCnt;
  unsigned int dwAccTotalCnt;
  unsigned int dwAccWinCnt;

  int size() const
  {
    return 10;
  }
};
#pragma pack(pop)
