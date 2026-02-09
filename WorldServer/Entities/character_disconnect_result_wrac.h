#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(1)) _character_disconnect_result_wrac
{
  unsigned __int16 wClientIndex;
  unsigned __int8 byResult;

  unsigned __int16 size() const;
};

inline unsigned __int16 _character_disconnect_result_wrac::size() const
{
  return 3;
}
