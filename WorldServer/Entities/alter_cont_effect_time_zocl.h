#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _alter_cont_effect_time_zocl
{
  struct __list
  {
    unsigned __int16 wEffectCode;
    __int16 zLeftSec;
  };

  unsigned __int8 byEffectNum;
  __list List[8];

  _alter_cont_effect_time_zocl() : byEffectNum(0) {}

  int size()
  {
    if (byEffectNum > 8u)
    {
      byEffectNum = 0;
    }
    return 33 - 4 * (8 - byEffectNum);
  }
};
#pragma pack(pop)
