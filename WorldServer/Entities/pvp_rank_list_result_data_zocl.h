#pragma once

#include "IdaCompat.h"

struct __cppobj __unaligned __declspec(align(1)) _pvp_rank_list_result_data_zocl
{
  unsigned __int8 byRace;
  unsigned __int8 byVersion;
  unsigned __int8 byPage;
  unsigned __int16 wRankDataLen;
  char szPvpRankData[4000];

  _pvp_rank_list_result_data_zocl();
  __int64 size();
};
