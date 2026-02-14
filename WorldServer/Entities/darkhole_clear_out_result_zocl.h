#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _darkhole_clear_out_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 wTarMapIndex;
  __int16 zTarPos[3];

  unsigned __int16 size() const;
};
#pragma pack(pop)

inline unsigned __int16 _darkhole_clear_out_result_zocl::size() const
{
  return 9;
}
