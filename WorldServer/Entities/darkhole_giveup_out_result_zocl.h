#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(1)) _darkhole_giveup_out_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 wTarMapIndex;
  __int16 zTarPos[3];

  unsigned __int16 size() const;
};

inline unsigned __int16 _darkhole_giveup_out_result_zocl::size() const
{
  return 9;
}
