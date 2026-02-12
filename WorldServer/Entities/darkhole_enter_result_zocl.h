#pragma once

#include "IdaCompat.h"

struct _darkhole_enter_result_zocl
{
  unsigned __int8 byRetCode;
  __unaligned __declspec(align(1)) unsigned int dwHoleSerial;
  unsigned __int8 byMapCode;
  __int16 zPos[3];

  unsigned __int16 size() const;
};

inline unsigned __int16 _darkhole_enter_result_zocl::size() const
{
  return 12;
}
