#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(2)) _darkhole_destroy_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;

  unsigned __int16 size() const;
};

inline unsigned __int16 _darkhole_destroy_zocl::size() const
{
  return 6;
}
