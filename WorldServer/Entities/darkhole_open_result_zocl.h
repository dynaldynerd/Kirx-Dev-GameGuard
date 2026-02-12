#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(1)) _darkhole_open_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int16 wHoleIndex;
  unsigned int dwHoleSerial;

  unsigned __int16 size() const;
};

inline unsigned __int16 _darkhole_open_result_zocl::size() const
{
  return 7;
}
