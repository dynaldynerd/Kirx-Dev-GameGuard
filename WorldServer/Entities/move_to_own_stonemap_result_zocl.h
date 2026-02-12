#pragma once

#include "IdaCompat.h"

struct _move_to_own_stonemap_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 byMapIndex;
  __int16 sNewPos[3];

  unsigned __int16 size() const;
};

inline unsigned __int16 _move_to_own_stonemap_result_zocl::size() const
{
  return 8;
}
