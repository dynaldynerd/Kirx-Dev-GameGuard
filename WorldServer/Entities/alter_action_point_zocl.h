#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _alter_action_point_zocl
{
  unsigned __int8 byActionCode;
  unsigned int dwActionPoint;

  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(_alter_action_point_zocl));
  }
};
#pragma pack(pop)
