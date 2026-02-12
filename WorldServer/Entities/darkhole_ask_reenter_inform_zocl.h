#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(2)) _darkhole_ask_reenter_inform_zocl
{
  unsigned __int16 wChannelIndex;
  unsigned int dwChannelSerial;

  unsigned __int16 size() const;
};

inline unsigned __int16 _darkhole_ask_reenter_inform_zocl::size() const
{
  return 6;
}
