#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(1)) _darkhole_channel_close_inform_zocl
{
  char sDum;

  unsigned __int16 size() const;
};

inline unsigned __int16 _darkhole_channel_close_inform_zocl::size() const
{
  return 1;
}
