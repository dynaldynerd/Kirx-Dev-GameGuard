#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(1)) _darkhole_open_portal_by_react_inform_zocl
{
  unsigned __int16 wPortalIndex;

  unsigned __int16 size() const;
};

inline unsigned __int16 _darkhole_open_portal_by_react_inform_zocl::size() const
{
  return 2;
}
