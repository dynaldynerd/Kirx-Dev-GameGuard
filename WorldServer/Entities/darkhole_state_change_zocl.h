#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(1)) _darkhole_state_change_zocl
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int8 bHurry;

  _darkhole_state_change_zocl();
  unsigned __int16 size() const;
};

inline _darkhole_state_change_zocl::_darkhole_state_change_zocl()
{
  memset_0(this, 0, sizeof(_darkhole_state_change_zocl));
}

inline unsigned __int16 _darkhole_state_change_zocl::size() const
{
  return 7;
}
