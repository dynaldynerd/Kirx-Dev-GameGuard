#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _notify_cont_play_time_zocl
{
  unsigned __int8 byContTime;
  unsigned __int8 byContMin;

  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(*this));
  }
};
#pragma pack(pop)
