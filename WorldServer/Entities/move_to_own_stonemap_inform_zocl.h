#pragma once

#include "IdaCompat.h"

struct _move_to_own_stonemap_inform_zocl
{
  unsigned __int8 byStoneMapMoveInfo;

  unsigned __int16 size() const;
};

inline unsigned __int16 _move_to_own_stonemap_inform_zocl::size() const
{
  return 1;
}
